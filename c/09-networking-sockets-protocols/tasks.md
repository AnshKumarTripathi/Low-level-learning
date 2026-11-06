# 09 — Networking: Sockets & Protocols (C)

[← 08 — OS I/O](../08-os-io-processes/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 10 — Concurrency →](../10-concurrency-synchronization/tasks.md) · [C++ twin: c_plus/06](../../c_plus/06-networking-sockets-protocols/tasks.md)

> Time: about 15–20 hours · Prerequisites: C 07 (bounds-checked parsing), C 08 (error reporting) · You will build: a TCP echo server and client, a UDP ping with timeout, and a length-prefixed protocol.

## Learning objectives

By the end of this chapter you will be able to:
- Choose between TCP and UDP and explain the trade-off.
- Write a TCP client and server with the sockets API, on Windows (Winsock) and POSIX.
- Explain why TCP is a **byte stream**, and handle partial `send`/`recv` correctly.
- Design a framed protocol and parse it safely from untrusted input.
- Add a timeout so a silent peer cannot hang your program forever.

## Why it matters

Network input is the most hostile input there is: anyone on the network can send any bytes, in any amount, at any speed. Every assumption ("the message arrives in one piece", "the length is reasonable") is a potential bug or vulnerability.

## Session plan

This chapter is long. Work through it in 5 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–6 |
| 3 | Exercise 1 — TCP echo |
| 4 | Exercise 2 — UDP ping |
| 5 | Exercise 3 — Length-prefixed protocol (framing), then the quiz and recap |

---

## Learn

### 1. TCP vs UDP

| | TCP | UDP |
|---|-----|-----|
| connection | yes: connect, then talk | no: each datagram is sent alone |
| delivery | reliable, in order, no duplicates | may be lost, duplicated, reordered |
| data shape | a continuous **stream of bytes** | separate **messages** (datagrams) |
| typical use | web, file transfer, SSH | DNS, games, video calls |

An **address** picks the machine (`127.0.0.1` is "this machine"); a **port** (0–65535) picks the program on it.

### 2. The socket lifecycle

```text
        SERVER                               CLIENT
  getaddrinfo(NULL, port)              getaddrinfo(host, port)
  socket()                             socket()
  bind()     ← claim the port
  listen()   ← start queueing clients
  accept()   ← waits ............. ◄── connect()
     │ returns a NEW socket for this client
  recv() / send()  ◄═══ bytes ═══►     send() / recv()
  close()                              close()
```

`getaddrinfo` turns a host name and port text into addresses, and works for both IPv4 and IPv6. Always use it instead of filling address structs by hand.

### 3. TCP is a byte stream

If the client calls `send` once with 10,000 bytes, the server may get them as one `recv` of 10,000 bytes — or as 3 `recv`s of 1,448, 7,000 and 1,552 bytes. Also, `send` may accept fewer bytes than you gave it. So:

- **Always loop** until all bytes are sent (`send_all` below).
- **Never assume one `recv` = one message.** The protocol itself must say where a message ends. That is called **framing**:
  - a length prefix: `[4-byte length][payload]` (Exercise 3), or
  - a delimiter: lines ending in `\n`, or
  - the end of the connection: "everything until the sender shuts down" (Exercise 1).

> **Session 1 of 5 complete: a good place to stop.** Check: close this page and explain in your own words: TCP vs UDP; The socket lifecycle; TCP is a byte stream. Stuck on one? Re-read it before moving on.

### 4. Windows vs POSIX differences

| | Windows (Winsock) | POSIX |
|---|-------------------|-------|
| headers | `<winsock2.h>`, `<ws2tcpip.h>` | `<sys/socket.h>`, `<netdb.h>`, `<unistd.h>` |
| before first use | `WSAStartup(MAKEWORD(2,2), &wsa)` | nothing |
| socket type / bad value | `SOCKET` / `INVALID_SOCKET` | `int` / `-1` |
| close | `closesocket(s)` | `close(s)` |
| stop sending | `shutdown(s, SD_SEND)` | `shutdown(s, SHUT_WR)` |
| error code | `WSAGetLastError()` | `errno` |
| link with | `-lws2_32` | nothing |
| at exit | `WSACleanup()` | nothing |

### 5. Worked example: a TCP client

Connects, sends a message, tells the server it is done sending, then prints everything the server sends back until the server closes. The first line matters on Linux: with `-std=c17`, the system headers hide POSIX functions such as `getaddrinfo` unless you ask for them.

```c
#define _POSIX_C_SOURCE 200809L   /* with -std=c17, Linux hides getaddrinfo without this */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET sock_t;
#define CLOSE_SOCKET closesocket
#define SHUT_WR SD_SEND
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

/* Sends all `len` bytes, looping over partial sends. Returns 0 or -1. */
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

/* Connects to the first address for host:port that works. */
static sock_t connect_to(const char *host, const char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;          /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;      /* TCP */

    struct addrinfo *list;
    if (getaddrinfo(host, port, &hints, &list) != 0) {
        return INVALID_SOCKET;
    }

    sock_t s = INVALID_SOCKET;
    for (struct addrinfo *a = list; a != NULL; a = a->ai_next) {
        s = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (s == INVALID_SOCKET) {
            continue;
        }
        if (connect(s, a->ai_addr, (int)a->ai_addrlen) == 0) {
            break;                        /* connected */
        }
        CLOSE_SOCKET(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(list);
    return s;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: tcpclient <host> <port> <message>\n");
        return 2;
    }
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "error: WSAStartup failed\n");
        return 1;
    }
#endif

    int rc = 1;
    sock_t s = connect_to(argv[1], argv[2]);
    if (s == INVALID_SOCKET) {
        fprintf(stderr, "error: cannot connect to %s:%s\n", argv[1], argv[2]);
    } else {
        if (send_all(s, argv[3], strlen(argv[3])) == 0) {
            shutdown(s, SHUT_WR);         /* "I am done sending" */
            char buf[1024];
            int n;
            while ((n = (int)recv(s, buf, sizeof buf, 0)) > 0) {   /* 0 = server closed */
                fwrite(buf, 1, (size_t)n, stdout);
            }
            rc = (n == 0) ? 0 : 1;
        }
        CLOSE_SOCKET(s);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return rc;
}
```

Build on Windows with `gcc -std=c17 -Wall -Wextra tcpclient.c -o tcpclient -lws2_32`; on Linux/macOS without `-lws2_32`. It was tested on Windows against a small local echo server; the POSIX branch follows Beej's Guide.

Things to notice:
- `connect_to` tries every address `getaddrinfo` returns (for example IPv6 first, then IPv4).
- One `rc` variable and one cleanup block at the end: every path closes the socket and calls `WSACleanup`.
- The receive loop treats `recv` returning `0` as "the other side closed": that is the end of the data, not an error.

### 6. Timeouts

`recv` waits forever by default. To give up after 2 seconds, set a receive timeout on the socket:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
#ifdef _WIN32
DWORD ms = 2000;
setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&ms, sizeof ms);
#else
struct timeval tv = {2, 0};                    /* 2 seconds, 0 microseconds */
setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
#endif
```

After a timeout, `recv` (or `recvfrom` for UDP) returns `-1` (Windows: `SOCKET_ERROR` with `WSAGetLastError() == WSAETIMEDOUT`; POSIX: `errno` is `EAGAIN` or `EWOULDBLOCK`).

> **Session 2 of 5 complete: a good place to stop.** Check: close this page and explain in your own words: Windows vs POSIX differences; Worked example: a TCP client; Timeouts. Stuck on one? Re-read it before moving on.

---

## Exercises

## Exercise 1 — TCP echo
### Task
Write two programs: a TCP echo server and a TCP client.

### Inputs
- Server: `echo_server <port>`
- Client: `echo_client <host> <port> <message>`

### Desired output
- Client prints exactly what the server echoed back.
- Server prints one line per connection:
  - `client_connected`
  - `bytes_received=<n>`

### Edge cases
- Empty message should still round-trip.
- Invalid host/port → error and non-zero exit.

### Hints
1. The worked example is a complete client. For the server, follow the left column of the diagram in Learn §2. Use `getaddrinfo(NULL, port, &hints, &list)` with `hints.ai_flags = AI_PASSIVE` to get an address to `bind` to.
2. Serve one client at a time: `accept`, then loop `recv` → `send_all` until `recv` returns 0, then close that client socket and `accept` the next.
3. On POSIX, set `SO_REUSEADDR` before `bind` so you can restart the server right away.
4. Test with your client, and with `nc 127.0.0.1 <port>` (Linux/WSL) or `Test-NetConnection` (PowerShell) to see that it listens.

> **Session 3 of 5 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — UDP ping
### Task
Implement a UDP “ping” message that receives a reply.

### Inputs
- Server: `udp_server <port>`
- Client: `udp_client <host> <port> <payload>`

### Desired output
- Client prints:
  - `reply=<payload>`
- If no reply within timeout: print `timeout` and exit non-zero.

### Hints
1. Use `SOCK_DGRAM` and `sendto` / `recvfrom`; there is no `listen`, `accept` or `connect`.
2. The server replies to the address `recvfrom` filled in.
3. Use the timeout recipe in Learn §6. Test the timeout by running the client with the server stopped.
4. One `recvfrom` = one datagram. A buffer smaller than the datagram silently loses the rest, so use a buffer as big as the largest payload you allow.

> **Session 4 of 5 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 3 — Length-prefixed protocol (framing)
### Task
Design and implement a tiny protocol over TCP:
- frame = `len` (4 bytes, little-endian) + `payload` (`len` bytes)

### Inputs
- Server: `frame_server <port>`
- Client: `frame_client <host> <port> <path_to_file>`

### Desired output
- Client sends file bytes as one payload.
- Server prints:
  - `payload_len=<n>`
  - `sha256=<hex>` (optional stretch if you implement hashing later)

### Edge cases
- If `len` is too large (define a max, e.g. 8 MiB): server rejects the connection.

### Hints
1. Write `recv_all(s, buf, n)`: loop until exactly `n` bytes have arrived, or fail if the peer closes early.
2. Server order: `recv_all` the 4-byte length → decode little-endian (C 07) → **check it against the maximum** → only then `malloc` and `recv_all` the payload.
3. Test the limit with a client that sends a length of `0xFFFFFFFF` and nothing else: the server must reject it without allocating 4 GB.

## Checkpoint quiz

1. Name two differences between TCP and UDP.
2. What does `accept` return?
3. The client sent 100 bytes in one `send`. Can the server's first `recv` return 40?
4. What does it mean when `recv` returns 0?
5. Why do you need `send_all`?
6. Give two ways to frame messages on a TCP stream.
7. What must every Windows sockets program call before using sockets?
8. Why check a received length **before** calling `malloc`?
9. How do you stop `recv` from waiting forever?
10. What does `shutdown(s, SHUT_WR)` tell the other side?

## Recap

- TCP is a reliable byte stream; UDP sends separate, unreliable messages.
- Server: `bind`, `listen`, `accept`; client: `getaddrinfo`, `connect`.
- Loop `send` and `recv`; frame messages yourself; treat `recv == 0` as "closed".
- Winsock needs `WSAStartup`/`WSACleanup`, `closesocket` and `-lws2_32`.
- Validate every length from the network before using it, and use timeouts.

## Spaced review

1. (Chapter 07) Decode the little-endian `u32` bytes `01 00 00 00`.
2. (Chapter 08) Where do error messages go?
3. (Chapter 04) Who frees a buffer that `recv_frame` allocates?

## Answers

Quiz: 1. TCP needs a connection and is reliable and ordered; UDP is connectionless, may lose or reorder data, and keeps message boundaries. 2. A new socket connected to that one client. 3. Yes — TCP does not keep message boundaries. 4. The other side closed (or shut down) its sending direction. 5. `send` may accept only part of the data. 6. Length prefix, delimiter (such as `\n`), or end of connection. 7. `WSAStartup`. 8. Otherwise an attacker can make you allocate huge amounts of memory with 4 bytes. 9. Set `SO_RCVTIMEO` (or use `select`/`poll`). 10. "I will send no more data" — their `recv` returns 0 after the data you already sent.

Spaced review: 1. 1. 2. stderr. 3. The caller, as documented by the function.

## References (concept-only)
- **Sockets overview**: [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/)
- **Winsock (Windows)**: [Windows Sockets start page](https://learn.microsoft.com/windows/win32/winsock/windows-sockets-start-page-2)
- **TCP vs UDP (concept)**: [Beej — intro sections](https://beej.us/guide/bgnet/html/#intro)
- **Framing & partial reads (concept)**: [Beej — send/recv](https://beej.us/guide/bgnet/html/#sendrecv)

## Global constraints (this chapter)
- Validate every length before allocating/reading.
- Timeouts are recommended (avoid infinite hangs).

## C track notes
- On Windows: use Winsock (WSAStartup/WSACleanup), and be strict about cleanup paths.
- Treat all received bytes as untrusted.
