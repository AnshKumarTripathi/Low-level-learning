/* Solution: C 09 Exercise 1 — TCP echo server. Attempt the exercise first.
 * Usage: echo_server <port>     (Windows: link with -lws2_32)
 * The client is the worked example in the chapter's Learn §5.
 * Serves one client at a time, forever; stop it with Ctrl+C.
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

#include <stdio.h>
#include <string.h>

static int send_all(sock_t s, const char *data, size_t len)
{
    while (len > 0) {
        int n = (int)send(s, data, (int)len, 0);
        if (n <= 0) {
            return -1;
        }
        data += n;
        len -= (size_t)n;
    }
    return 0;
}

/* Creates a socket bound to `port` on all local addresses and listening. */
static sock_t listen_on(const char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;            /* IPv4 keeps the example simple */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;          /* "an address I can bind to" */

    struct addrinfo *list;
    if (getaddrinfo(NULL, port, &hints, &list) != 0) {
        return INVALID_SOCKET;
    }
    sock_t s = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if (s != INVALID_SOCKET) {
#ifndef _WIN32
        int yes = 1;                      /* allow an immediate restart on the same port */
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
#endif
        if (bind(s, list->ai_addr, (int)list->ai_addrlen) != 0 || listen(s, 8) != 0) {
            CLOSE_SOCKET(s);
            s = INVALID_SOCKET;
        }
    }
    freeaddrinfo(list);
    return s;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: echo_server <port>\n");
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
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    for (;;) {
        sock_t client = accept(server, NULL, NULL);
        if (client == INVALID_SOCKET) {
            continue;
        }
        printf("client_connected\n");
        unsigned long long total = 0;
        char buf[4096];
        int n;
        while ((n = (int)recv(client, buf, sizeof buf, 0)) > 0) {
            total += (unsigned long long)n;
            if (send_all(client, buf, (size_t)n) != 0) {
                break;
            }
        }
        printf("bytes_received=%llu\n", total);
        fflush(stdout);                   /* show each connection right away */
        CLOSE_SOCKET(client);
    }
}
