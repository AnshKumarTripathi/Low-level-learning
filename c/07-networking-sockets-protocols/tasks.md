# 07 — Networking: Sockets & Protocols (C)

## Objectives
- Learn basic socket programming (TCP/UDP).
- Learn framing (how to split a byte stream into messages).
- Practice robust parsing for untrusted network input.

## References (concept-only)
- **Sockets overview**: [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/)
- **Winsock (Windows)**: [Windows Sockets start page](https://learn.microsoft.com/windows/win32/winsock/windows-sockets-start-page-2)
- **TCP vs UDP (concept)**: [Beej — intro sections](https://beej.us/guide/bgnet/html/#intro)
- **Framing & partial reads (concept)**: [Beej — send/recv](https://beej.us/guide/bgnet/html/#sendrecv)

## Global constraints (this chapter)
- Validate every length before allocating/reading.
- Timeouts are recommended (avoid infinite hangs).

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

## C track notes
- On Windows: use Winsock (WSAStartup/WSACleanup), and be strict about cleanup paths.
- Treat all received bytes as untrusted.

