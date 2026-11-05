/* Solution: C 09 Exercise 2 — UDP ping. Attempt the exercise first.
 * One program, two modes (Windows: link with -lws2_32):
 *   udp_ping server <port>                    replies to every datagram with the same bytes
 *   udp_ping client <host> <port> <payload>   prints reply=<payload>, or "timeout" after 2 s
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

#include <stdio.h>
#include <string.h>

#define MAX_DATAGRAM 1024

/* Resolves host:port (host NULL = any local address) and returns a UDP socket.
 * If `bind_it`, binds to that address; otherwise copies the address into *peer. */
static sock_t make_udp(const char *host, const char *port, int bind_it,
                       struct sockaddr_storage *peer, int *peer_len)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = bind_it ? AI_PASSIVE : 0;
    struct addrinfo *list;
    if (getaddrinfo(host, port, &hints, &list) != 0) {
        return INVALID_SOCKET;
    }
    sock_t s = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if (s != INVALID_SOCKET) {
        if (bind_it) {
            if (bind(s, list->ai_addr, (int)list->ai_addrlen) != 0) {
                CLOSE_SOCKET(s);
                s = INVALID_SOCKET;
            }
        } else {
            memcpy(peer, list->ai_addr, list->ai_addrlen);
            *peer_len = (int)list->ai_addrlen;
        }
    }
    freeaddrinfo(list);
    return s;
}

static void set_timeout_2s(sock_t s)
{
#ifdef _WIN32
    DWORD ms = 2000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&ms, sizeof ms);
#else
    struct timeval tv = {2, 0};
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
#endif
}

static int run_server(const char *port)
{
    sock_t s = make_udp(NULL, port, 1, NULL, NULL);
    if (s == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot bind UDP port %s\n", port);
        return 1;
    }
    for (;;) {
        char buf[MAX_DATAGRAM];
        struct sockaddr_storage from;
        socklen_t from_len = sizeof from;
        int n = (int)recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&from, &from_len);
        if (n < 0) {
            continue;
        }
        sendto(s, buf, n, 0, (struct sockaddr *)&from, from_len);   /* echo back to the sender */
        printf("ping_bytes=%d\n", n);
        fflush(stdout);
    }
}

static int run_client(const char *host, const char *port, const char *payload)
{
    size_t len = strlen(payload);
    if (len > MAX_DATAGRAM) {
        fprintf(stderr, "error: payload longer than %d bytes\n", MAX_DATAGRAM);
        return 2;
    }
    struct sockaddr_storage peer;
    int peer_len = 0;
    sock_t s = make_udp(host, port, 0, &peer, &peer_len);
    if (s == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot resolve %s:%s\n", host, port);
        return 1;
    }
    set_timeout_2s(s);
    int rc = 1;
    if (sendto(s, payload, (int)len, 0, (struct sockaddr *)&peer, peer_len) < 0) {
        fprintf(stderr, "error: send failed\n");
    } else {
        char buf[MAX_DATAGRAM + 1];
        int n = (int)recvfrom(s, buf, MAX_DATAGRAM, 0, NULL, NULL);
        if (n < 0) {
            printf("timeout\n");                   /* also reached when nothing listens */
        } else {
            buf[n] = '\0';
            printf("reply=%s\n", buf);
            rc = 0;
        }
    }
    CLOSE_SOCKET(s);
    return rc;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "error: WSAStartup failed\n");
        return 1;
    }
#endif
    int rc;
    if (argc == 3 && strcmp(argv[1], "server") == 0) {
        rc = run_server(argv[2]);
    } else if (argc == 5 && strcmp(argv[1], "client") == 0) {
        rc = run_client(argv[2], argv[3], argv[4]);
    } else {
        fprintf(stderr, "usage: udp_ping server <port> | client <host> <port> <payload>\n");
        rc = 2;
    }
#ifdef _WIN32
    WSACleanup();
#endif
    return rc;
}
