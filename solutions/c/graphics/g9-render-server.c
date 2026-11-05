/* G9 — Render server: an HTTP service that ray traces a scene per request and returns a BMP image.
 *
 * Usage: render_server <port> [--threads N] [--max-requests M]
 *   GET /                                  a small HTML page showing renders
 *   GET /render?w=320&h=200&angle=0.5      a ray traced BMP (w, h: 1-1024; angle in radians)
 *   GET /stats                             requests served so far
 * --max-requests M makes the server exit after M connections (for tests).
 *
 * Build: gcc -std=c17 -Wall -Wextra -O2 g9-render-server.c -o render_server -pthread -lm   (Windows: add -lws2_32)
 * Try:   ./render_server 8080, then open http://localhost:8080/ in a browser.
 */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET sock_t;
#define CLOSE_SOCKET closesocket
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int sock_t;
#define INVALID_SOCKET (-1)
#define CLOSE_SOCKET close
#endif

#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 32
#define QUEUE_CAP 16
#define MAX_REQUEST 8192
#define MAX_SIDE 1024

/* ------------------------------------------------------------------ */
/* Rendering: a compact version of the G6 ray tracer.                  */
/* ------------------------------------------------------------------ */

typedef struct { double x, y, z; } Vec3;

static Vec3 add(Vec3 a, Vec3 b) { return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static Vec3 sub(Vec3 a, Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static Vec3 scale(Vec3 a, double s) { return (Vec3){a.x * s, a.y * s, a.z * s}; }
static double dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static Vec3 norm(Vec3 a) { return scale(a, 1.0 / sqrt(dot(a, a))); }

typedef struct { Vec3 c; double r; Vec3 color; double mirror; } Sphere;

static const Sphere spheres[] = {
    {{0.0, 1.0, 0.0}, 1.0, {0.9, 0.2, 0.2}, 0.3},
    {{-2.2, 0.7, 1.0}, 0.7, {0.2, 0.5, 0.9}, 0.05},
    {{2.0, 0.8, -1.0}, 0.8, {0.9, 0.9, 0.9}, 0.7},
};
#define NSPHERES (sizeof spheres / sizeof spheres[0])
static const Vec3 light = {-4.0, 6.0, -5.0};

static double hit_sphere(const Sphere *s, Vec3 o, Vec3 d)
{
    Vec3 oc = sub(o, s->c);
    double b = dot(oc, d), k = dot(oc, oc) - s->r * s->r, disc = b * b - k;
    if (disc < 0) {
        return -1;
    }
    double t = -b - sqrt(disc);
    if (t > 1e-6) {
        return t;
    }
    t = -b + sqrt(disc);
    return t > 1e-6 ? t : -1;
}

static double nearest(Vec3 o, Vec3 d, int *which)
{
    double best = -1;
    *which = -2;
    for (size_t i = 0; i < NSPHERES; i++) {
        double t = hit_sphere(&spheres[i], o, d);
        if (t > 0 && (best < 0 || t < best)) {
            best = t;
            *which = (int)i;
        }
    }
    if (d.y < 0) {
        double t = -o.y / d.y;
        if (t > 1e-6 && (best < 0 || t < best)) {
            best = t;
            *which = -1;
        }
    }
    return best;
}

static Vec3 trace(Vec3 o, Vec3 d, int depth)
{
    int which;
    double t = nearest(o, d, &which);
    if (t < 0) {
        double s = 0.5 * (d.y + 1.0);
        return add(scale((Vec3){1, 1, 1}, 1 - s), scale((Vec3){0.45, 0.65, 1.0}, s));
    }
    Vec3 p = add(o, scale(d, t)), n, base;
    double mirror;
    if (which >= 0) {
        n = norm(sub(p, spheres[which].c));
        base = spheres[which].color;
        mirror = spheres[which].mirror;
    } else {
        n = (Vec3){0, 1, 0};
        base = (((int)floor(p.x) + (int)floor(p.z)) & 1) ? (Vec3){0.85, 0.85, 0.85} : (Vec3){0.15, 0.15, 0.18};
        mirror = 0.1;
    }
    Vec3 to_l = sub(light, p);
    double dist = sqrt(dot(to_l, to_l));
    to_l = scale(to_l, 1.0 / dist);
    int blocker;
    double ts = nearest(add(p, scale(n, 1e-4)), to_l, &blocker);
    double diffuse = (ts > 0 && ts < dist) ? 0.0 : fmax(0.0, dot(n, to_l));
    Vec3 col = scale(base, 0.12 + diffuse);
    if (mirror > 0 && depth > 0) {
        Vec3 rd = sub(d, scale(n, 2 * dot(d, n)));
        col = add(scale(col, 1 - mirror), scale(trace(add(p, scale(n, 1e-4)), rd, depth - 1), mirror));
    }
    return col;
}

static uint8_t to_byte(double v)
{
    return (uint8_t)lround(255 * sqrt(fmin(1.0, fmax(0.0, v))));
}

/* Renders into a freshly allocated 24-bit BMP file image. Returns NULL if out of memory. */
static uint8_t *render_bmp(int w, int h, double angle, size_t *out_size)
{
    size_t row = ((size_t)w * 3 + 3) & ~(size_t)3;           /* BMP rows are padded to 4 bytes */
    size_t size = 54 + row * (size_t)h;
    uint8_t *bmp = calloc(1, size);
    if (bmp == NULL) {
        return NULL;
    }
    /* 14-byte file header + 40-byte info header, all little-endian (C 05). */
    uint32_t fields[] = {(uint32_t)size, 0, 54, 40, (uint32_t)w, (uint32_t)h};
    bmp[0] = 'B';
    bmp[1] = 'M';
    for (int f = 0; f < 6; f++) {
        for (int b = 0; b < 4; b++) {
            bmp[2 + f * 4 + b] = (uint8_t)(fields[f] >> (8 * b));
        }
    }
    bmp[26] = 1;                                             /* planes */
    bmp[28] = 24;                                            /* bits per pixel */

    Vec3 eye = {7 * sin(angle), 2.2, -7 * cos(angle)};       /* orbit around the scene */
    Vec3 fwd = norm(sub((Vec3){0, 0.8, 0}, eye));
    Vec3 right = norm((Vec3){fwd.z, 0, -fwd.x});
    Vec3 up = {fwd.y * right.z - fwd.z * right.y, fwd.z * right.x - fwd.x * right.z, fwd.x * right.y - fwd.y * right.x};   /* fwd x right */
    double half = tan(50.0 * 3.14159265358979323846 / 180.0 / 2);

    for (int y = 0; y < h; y++) {
        uint8_t *dst = bmp + 54 + row * (size_t)(h - 1 - y);   /* BMP stores the bottom row first */
        for (int x = 0; x < w; x++) {
            double u = (2 * (x + 0.5) / w - 1) * half;
            double v = (1 - 2 * (y + 0.5) / h) * half * h / w;
            Vec3 d = norm(add(fwd, add(scale(right, u), scale(up, v))));
            Vec3 c = trace(eye, d, 2);
            dst[x * 3 + 0] = to_byte(c.z);                     /* BMP order is blue, green, red */
            dst[x * 3 + 1] = to_byte(c.y);
            dst[x * 3 + 2] = to_byte(c.x);
        }
    }
    *out_size = size;
    return bmp;
}

/* ------------------------------------------------------------------ */
/* HTTP                                                                */
/* ------------------------------------------------------------------ */

static pthread_mutex_t stats_lock = PTHREAD_MUTEX_INITIALIZER;
static long long served = 0, rendered = 0, errors = 0;

static int send_all(sock_t s, const void *data, size_t len)
{
    const char *p = data;
    while (len > 0) {
        int n = (int)send(s, p, (int)(len > 65536 ? 65536 : len), 0);
        if (n <= 0) {
            return -1;
        }
        p += n;
        len -= (size_t)n;
    }
    return 0;
}

static void send_response(sock_t s, int code, const char *reason, const char *type, const void *body, size_t len)
{
    char head[256];
    int n = snprintf(head, sizeof head,
                     "HTTP/1.0 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
                     code, reason, type, len);
    if (send_all(s, head, (size_t)n) == 0) {
        send_all(s, body, len);
    }
    pthread_mutex_lock(&stats_lock);
    served++;
    if (code != 200) {
        errors++;
    }
    pthread_mutex_unlock(&stats_lock);
}

static void send_error(sock_t s, int code, const char *reason)
{
    char body[128];
    int n = snprintf(body, sizeof body, "%d %s\n", code, reason);
    send_response(s, code, reason, "text/plain", body, (size_t)n);
}

/* Finds "name=" in the query string and parses its value. Returns 0 on success, -1 if missing or bad. */
static int query_double(const char *query, const char *name, double *out)
{
    size_t len = strlen(name);
    for (const char *p = query; p != NULL && *p != '\0'; p = strchr(p, '&') ? strchr(p, '&') + 1 : NULL) {
        if (strncmp(p, name, len) == 0 && p[len] == '=') {
            char *end;
            double v = strtod(p + len + 1, &end);
            if (end == p + len + 1 || (*end != '\0' && *end != '&') || !isfinite(v)) {
                return -1;
            }
            *out = v;
            return 0;
        }
    }
    return -1;
}

static void handle_client(sock_t s)
{
    char req[MAX_REQUEST + 1];
    size_t got = 0;
    while (got < MAX_REQUEST) {                              /* read until the blank line ends the headers */
        int n = (int)recv(s, req + got, (int)(MAX_REQUEST - got), 0);
        if (n <= 0) {
            return;                                          /* closed, error or timeout: nothing to answer */
        }
        got += (size_t)n;
        req[got] = '\0';
        if (strstr(req, "\r\n\r\n") != NULL) {
            break;
        }
    }
    if (strstr(req, "\r\n\r\n") == NULL) {
        send_error(s, 431, "Request Header Fields Too Large");
        return;
    }

    char method[8], target[1024];
    if (sscanf(req, "%7s %1023s", method, target) != 2) {
        send_error(s, 400, "Bad Request");
        return;
    }
    if (strcmp(method, "GET") != 0) {
        send_error(s, 405, "Method Not Allowed");
        return;
    }

    char *query = strchr(target, '?');
    if (query != NULL) {
        *query++ = '\0';
    }

    if (strcmp(target, "/") == 0) {
        static const char page[] =
            "<!doctype html><title>Render server</title><body style=\"background:#222;color:#eee;font-family:sans-serif\">"
            "<h1>Render server</h1><p>Each image is ray traced on request.</p>"
            "<img src=\"/render?w=320&h=200&angle=0\"> <img src=\"/render?w=320&h=200&angle=1.5\">"
            "<img src=\"/render?w=320&h=200&angle=3\"> <img src=\"/render?w=320&h=200&angle=4.5\">"
            "<p><a href=\"/stats\" style=\"color:#9cf\">stats</a></p></body>";
        send_response(s, 200, "OK", "text/html", page, sizeof page - 1);
    } else if (strcmp(target, "/stats") == 0) {
        char body[160];
        pthread_mutex_lock(&stats_lock);
        int n = snprintf(body, sizeof body, "served=%lld rendered=%lld errors=%lld\n", served, rendered, errors);
        pthread_mutex_unlock(&stats_lock);
        send_response(s, 200, "OK", "text/plain", body, (size_t)n);
    } else if (strcmp(target, "/render") == 0) {
        double w = 320, h = 200, angle = 0;
        if (query == NULL || query_double(query, "w", &w) != 0 || query_double(query, "h", &h) != 0) {
            send_error(s, 400, "Bad Request");
            return;
        }
        if (query_double(query, "angle", &angle) != 0) {
            angle = 0;                                       /* angle is optional */
        }
        if (w < 1 || h < 1 || w > MAX_SIDE || h > MAX_SIDE || w != floor(w) || h != floor(h)) {
            send_error(s, 400, "Bad Request");               /* also caps the work one request can cause */
            return;
        }
        size_t size;
        uint8_t *bmp = render_bmp((int)w, (int)h, angle, &size);
        if (bmp == NULL) {
            send_error(s, 503, "Service Unavailable");
            return;
        }
        pthread_mutex_lock(&stats_lock);
        rendered++;
        pthread_mutex_unlock(&stats_lock);
        send_response(s, 200, "OK", "image/bmp", bmp, size);
        free(bmp);
    } else {
        send_error(s, 404, "Not Found");
    }
}

/* ------------------------------------------------------------------ */
/* Thread pool: a bounded queue of client sockets (C 10 Exercise 2).   */
/* ------------------------------------------------------------------ */

typedef struct {
    sock_t items[QUEUE_CAP];
    size_t head, tail, count;
    int closed;
    pthread_mutex_t lock;
    pthread_cond_t not_empty, not_full;
} Queue;

static Queue queue = {.lock = PTHREAD_MUTEX_INITIALIZER, .not_empty = PTHREAD_COND_INITIALIZER,
                      .not_full = PTHREAD_COND_INITIALIZER};

static void queue_push(sock_t s)
{
    pthread_mutex_lock(&queue.lock);
    while (queue.count == QUEUE_CAP) {
        pthread_cond_wait(&queue.not_full, &queue.lock);     /* back-pressure: stop accepting while busy */
    }
    queue.items[queue.tail] = s;
    queue.tail = (queue.tail + 1) % QUEUE_CAP;
    queue.count++;
    pthread_cond_signal(&queue.not_empty);
    pthread_mutex_unlock(&queue.lock);
}

/* Returns 0 and a socket, or -1 when the queue is closed and empty. */
static int queue_pop(sock_t *out)
{
    pthread_mutex_lock(&queue.lock);
    while (queue.count == 0 && !queue.closed) {
        pthread_cond_wait(&queue.not_empty, &queue.lock);
    }
    if (queue.count == 0) {
        pthread_mutex_unlock(&queue.lock);
        return -1;
    }
    *out = queue.items[queue.head];
    queue.head = (queue.head + 1) % QUEUE_CAP;
    queue.count--;
    pthread_cond_signal(&queue.not_full);
    pthread_mutex_unlock(&queue.lock);
    return 0;
}

static void *worker(void *arg)
{
    (void)arg;
    sock_t s;
    while (queue_pop(&s) == 0) {
        handle_client(s);
        CLOSE_SOCKET(s);
    }
    return NULL;
}

static void set_recv_timeout(sock_t s, int seconds)
{
#ifdef _WIN32
    DWORD ms = (DWORD)seconds * 1000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&ms, sizeof ms);
#else
    struct timeval tv = {seconds, 0};
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
#endif
}

static sock_t listen_on(const char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *list;
    if (getaddrinfo(NULL, port, &hints, &list) != 0) {
        return INVALID_SOCKET;
    }
    sock_t s = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if (s != INVALID_SOCKET) {
#ifndef _WIN32
        int yes = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
#endif
        if (bind(s, list->ai_addr, (int)list->ai_addrlen) != 0 || listen(s, 32) != 0) {
            CLOSE_SOCKET(s);
            s = INVALID_SOCKET;
        }
    }
    freeaddrinfo(list);
    return s;
}

int main(int argc, char *argv[])
{
    int nthreads = 4;
    long max_requests = 0;                                   /* 0 = forever */
    if (argc < 2) {
        fprintf(stderr, "usage: %s <port> [--threads N] [--max-requests M]\n", argv[0]);
        return 2;
    }
    for (int i = 2; i + 1 < argc; i += 2) {
        if (strcmp(argv[i], "--threads") == 0) {
            nthreads = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--max-requests") == 0) {
            max_requests = atol(argv[i + 1]);
        }
    }
    if (nthreads < 1 || nthreads > MAX_THREADS) {
        fprintf(stderr, "error: --threads must be 1-%d\n", MAX_THREADS);
        return 2;
    }
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "error: WSAStartup failed\n");
        return 1;
    }
#endif
    sock_t server = listen_on(argv[1]);
    if (server == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot listen on port %s\n", argv[1]);
        return 1;
    }
    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < nthreads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }
    printf("listening on http://localhost:%s/ with %d threads\n", argv[1], nthreads);
    fflush(stdout);

    for (long accepted = 0; max_requests == 0 || accepted < max_requests; accepted++) {
        sock_t client = accept(server, NULL, NULL);
        if (client == INVALID_SOCKET) {
            continue;
        }
        set_recv_timeout(client, 5);         /* a client that sends nothing cannot hold a worker forever */
        queue_push(client);
    }

    /* Shutdown: let the workers finish what is queued, then join them. */
    pthread_mutex_lock(&queue.lock);
    queue.closed = 1;
    pthread_cond_broadcast(&queue.not_empty);
    pthread_mutex_unlock(&queue.lock);
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    CLOSE_SOCKET(server);
#ifdef _WIN32
    WSACleanup();
#endif
    printf("served=%lld rendered=%lld errors=%lld\n", served, rendered, errors);
    return 0;
}
