/* Solution: C 09 Exercise 3 — Length-prefixed protocol. Attempt the exercise first.
 * One program, two modes (Windows: link with -lws2_32):
 *   frames server <port>                    prints payload_len=<n> per frame, rejects frames over 8 MiB
 *   frames client <host> <port> <file>      sends the file as one frame
 * Frame = 4-byte little-endian length + that many payload bytes.
 */
#define _POSIX_C_SOURCE 200809L   /* with -std=c17, Linux hides getaddrinfo without this */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET sock_t;
#define CLOSE_SOCKET closesocket
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int sock_t;
#define INVALID_SOCKET (-1)
#define CLOSE_SOCKET close
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAME (8u * 1024u * 1024u)

static int send_all(sock_t s, const uint8_t *data, size_t len)
{
    while (len > 0) {
        int chunk = len > 65536 ? 65536 : (int)len;
        int n = (int)send(s, (const char *)data, chunk, 0);
        if (n <= 0) {
            return -1;
        }
        data += n;
        len -= (size_t)n;
    }
    return 0;
}

/* Receives exactly `len` bytes. Returns 0, or -1 if the peer closed early or an error happened. */
static int recv_all(sock_t s, uint8_t *data, size_t len)
{
    while (len > 0) {
        int chunk = len > 65536 ? 65536 : (int)len;
        int n = (int)recv(s, (char *)data, chunk, 0);
        if (n <= 0) {
            return -1;
        }
        data += n;
        len -= (size_t)n;
    }
    return 0;
}

static sock_t open_socket(const char *host, const char *port, int server)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = server ? AI_PASSIVE : 0;
    struct addrinfo *list;
    if (getaddrinfo(host, port, &hints, &list) != 0) {
        return INVALID_SOCKET;
    }
    sock_t s = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if (s != INVALID_SOCKET) {
        int ok = server ? (bind(s, list->ai_addr, (int)list->ai_addrlen) == 0 && listen(s, 8) == 0)
                        : (connect(s, list->ai_addr, (int)list->ai_addrlen) == 0);
        if (!ok) {
            CLOSE_SOCKET(s);
            s = INVALID_SOCKET;
        }
    }
    freeaddrinfo(list);
    return s;
}

static void serve_client(sock_t c)
{
    uint8_t hdr[4];
    while (recv_all(c, hdr, 4) == 0) {             /* one connection may send several frames */
        uint32_t len = (uint32_t)hdr[0] | ((uint32_t)hdr[1] << 8) |
                       ((uint32_t)hdr[2] << 16) | ((uint32_t)hdr[3] << 24);
        if (len > MAX_FRAME) {                     /* check BEFORE allocating */
            printf("rejected: frame of %lu bytes is over the limit\n", (unsigned long)len);
            return;
        }
        uint8_t *payload = malloc(len ? len : 1);
        if (payload == NULL) {
            printf("rejected: out of memory\n");
            return;
        }
        int ok = recv_all(c, payload, len) == 0;
        free(payload);
        if (!ok) {
            printf("rejected: connection closed in the middle of a frame\n");
            return;
        }
        printf("payload_len=%lu\n", (unsigned long)len);
        fflush(stdout);
    }
}

static int run_server(const char *port)
{
    sock_t s = open_socket(NULL, port, 1);
    if (s == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot listen on port %s\n", port);
        return 1;
    }
    for (;;) {
        sock_t c = accept(s, NULL, NULL);
        if (c == INVALID_SOCKET) {
            continue;
        }
        serve_client(c);
        fflush(stdout);
        CLOSE_SOCKET(c);
    }
}

static int run_client(const char *host, const char *port, const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        perror(path);
        return 1;
    }
    uint8_t *data = malloc(MAX_FRAME + 1);
    size_t len = data ? fread(data, 1, MAX_FRAME + 1, f) : 0;
    int read_failed = data == NULL || ferror(f);
    fclose(f);
    if (read_failed || len > MAX_FRAME) {
        fprintf(stderr, "error: cannot read %s, or it is larger than %u bytes\n", path, MAX_FRAME);
        free(data);
        return 1;
    }

    sock_t s = open_socket(host, port, 0);
    if (s == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot connect to %s:%s\n", host, port);
        free(data);
        return 1;
    }
    uint8_t hdr[4] = {(uint8_t)(len & 0xFF), (uint8_t)((len >> 8) & 0xFF),
                      (uint8_t)((len >> 16) & 0xFF), (uint8_t)(len >> 24)};
    int rc = (send_all(s, hdr, 4) == 0 && send_all(s, data, len) == 0) ? 0 : 1;
    if (rc != 0) {
        fprintf(stderr, "error: send failed\n");
    } else {
        printf("sent_len=%zu\n", len);
    }
    CLOSE_SOCKET(s);
    free(data);
    return rc;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 1;
    }
#endif
    int rc;
    if (argc == 3 && strcmp(argv[1], "server") == 0) {
        rc = run_server(argv[2]);
    } else if (argc == 5 && strcmp(argv[1], "client") == 0) {
        rc = run_client(argv[2], argv[3], argv[4]);
    } else {
        fprintf(stderr, "usage: frames server <port> | client <host> <port> <file>\n");
        rc = 2;
    }
#ifdef _WIN32
    WSACleanup();
#endif
    return rc;
}
