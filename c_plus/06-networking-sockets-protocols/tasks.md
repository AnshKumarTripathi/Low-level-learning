# 06 — Networking: Sockets & Protocols (C++)

## Objectives
- Learn sockets (TCP/UDP) with strong resource cleanup.
- Learn framing: how to split a byte stream into messages.
- Practice robust parsing for untrusted network input.

## References (concept-only)
- **Sockets overview**: [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/)
- **Winsock (Windows)**: [Windows Sockets start page](https://learn.microsoft.com/windows/win32/winsock/windows-sockets-start-page-2)
- **RAII cleanup pattern (concept)**: [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)

## Global constraints (this chapter)
- Validate every length before allocating/reading.
- Timeouts are recommended (avoid infinite hangs).

## Exercise 1 — TCP echo
### Task
Write a TCP echo server and a TCP client.

### Inputs
- Server: `echo_server <port>`
- Client: `echo_client <host> <port> <message>`

### Desired output
- Client prints exactly what the server echoed back.
- Server prints one line per connection:
  - `client_connected`
  - `bytes_received=<n>`

## Exercise 2 — UDP ping
### Task
Implement a UDP “ping” message that receives a reply with timeout.

### Desired output
- Client prints `reply=<payload>` or `timeout` (non-zero exit).

## Exercise 3 — Length-prefixed protocol (framing)
### Task
Implement a protocol over TCP:
- frame = `len` (4 bytes, little-endian) + `payload` (`len` bytes)

### Desired output
- Client sends file bytes as one payload.
- Server prints `payload_len=<n>` and rejects too-large frames (define a max).

## C++ track notes
- Use RAII to ensure sockets are closed on all paths.
- Avoid copying payload buffers unnecessarily; use spans/views where possible.

