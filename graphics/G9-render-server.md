# G9 — Render Server

[← G8 — Engine milestone](G8-engine-milestone.md) · [Graphics track](README.md)

> Time: about 10–14 hours · Prerequisites: G6 (ray tracer), C 05 (byte order), C 09 (sockets), C 10 (threads, work queue) · You will build: an HTTP server that ray traces an image for every request, with a thread pool, input validation and a browser page.

## Learning objectives

By the end of this chapter you will be able to:
- Read an HTTP request and send a correct response with headers and a body.
- Encode an image as a BMP file that every browser can show.
- Serve many clients at once with a fixed pool of worker threads and a bounded queue.
- Protect a server from requests that would use too much memory, CPU or time.

## Why it matters

"Rendering as a service" is how much real graphics is delivered: map tiles, thumbnails, chart images, cloud rendering for films. It joins three parts of this course — the ray tracer, the network chapter and the concurrency chapter — and adds the question every server must answer: **what happens when a client sends something huge, broken, or nothing at all?**

---

## Learn

### 1. HTTP in one screen

HTTP/1.0 is plain text over a TCP connection (C 09). The browser sends:

```text
GET /render?w=320&h=200&angle=0.5 HTTP/1.1\r\n
Host: localhost:8080\r\n
User-Agent: ...\r\n
\r\n                          ← an empty line ends the headers
```

The server answers with a status line, headers, an empty line, then exactly `Content-Length` bytes of body, and closes the connection:

```text
HTTP/1.0 200 OK\r\n
Content-Type: image/bmp\r\n
Content-Length: 192054\r\n
Connection: close\r\n
\r\n
<192054 bytes of image>
```

The status codes you need: `200 OK`, `400 Bad Request` (invalid parameters), `404 Not Found`, `405 Method Not Allowed` (not `GET`), `431 Request Header Fields Too Large`, `503 Service Unavailable` (out of memory).

Reading the request is C 07 parsing discipline over a socket: `recv` returns **whatever has arrived**, which may be half a line. Keep reading into a fixed buffer until you see `\r\n\r\n`, and give up with `431` when the buffer is full.

### 2. A browser-friendly image: BMP

Browsers cannot show PPM, and PNG needs compression. **BMP** is uncompressed and every browser shows it. A 24-bit BMP is a 54-byte header, then the pixels:

| Offset | Size | Field | Value |
|--------|------|-------|-------|
| 0 | 2 | magic | `'B' 'M'` |
| 2 | 4 | file size | `54 + row_size * h` |
| 10 | 4 | pixel data offset | `54` |
| 14 | 4 | info header size | `40` |
| 18 | 4 | width | `w` |
| 22 | 4 | height | `h` (positive = bottom row first) |
| 26 | 2 | planes | `1` |
| 28 | 2 | bits per pixel | `24` |

All numbers are **little-endian** (C 05): write them byte by byte with shifts, never by copying a struct (padding and byte order would differ between machines). Three traps:
- pixels are stored **blue, green, red**;
- rows go **bottom to top**;
- every row is padded to a multiple of 4 bytes: `row_size = (w * 3 + 3) & ~3`.

### 3. A thread pool

Rendering takes milliseconds to seconds. With one thread, every client waits for everyone before them. One thread **per connection** is simple, but 10,000 connections would mean 10,000 threads. The standard design is the work queue from C 10 Exercise 2:

```text
main thread:   accept() → push socket into a bounded queue (waits while it is full)
N workers:     pop socket → read request → render → send → close → repeat
```

- **N** is about the number of CPU cores: rendering is CPU-bound, so more threads than cores only adds switching.
- A **bounded** queue gives back-pressure: when workers are busy, `accept` pauses and new clients wait in the OS's listen backlog instead of eating memory.
- Shared counters (requests served) need a mutex, like any shared data (C 10).

Measured with the reference solution on a 12-core laptop, 8 parallel requests for 1024 × 768 images: **1 worker 0.92 s, 4 workers 0.48 s**. Your numbers will differ; measure your own with `curl` in parallel.

### 4. Defending the server

Every request is untrusted input from the network. The reference solution's rules:

| Attack or accident | Defence |
|--------------------|---------|
| `w=100000&h=100000` (40 GB of pixels, minutes of CPU) | limit `w` and `h` to `1`–`1024` → `400` |
| `w=abc`, `w=`, `w=1e999`, `w=nan` | parse with `strtod`, check `end` and `isfinite` → `400` |
| a request line longer than the buffer | fixed 8 KB buffer, `431` when full, never overflow |
| a client connects and sends nothing ("slowloris") | `SO_RCVTIMEO` of 5 seconds on each client socket |
| `malloc` fails for the image | `503`, and the server keeps running |
| `POST`, `DELETE`, … | `405` |

None of these may crash or stop the server. Test each one with `curl` (Exercise 3).

### 5. Shutting down cleanly

A server that only ever exits with Ctrl+C is hard to test and leaks by design. The reference solution takes `--max-requests M`: after `M` connections it **closes** the queue, wakes all workers with `pthread_cond_broadcast`, lets them finish what is queued, `pthread_join`s them, and prints its counters. That makes a scripted test possible: start the server, run `M` `curl` commands, check the final line.

---

## Exercises

## Exercise 1 — BMP writer
### Task
Write `uint8_t *bmp_encode(const uint8_t *rgb, int w, int h, size_t *size)` that turns a G1 canvas (top row first, RGB) into a BMP file image in memory. Save the G6 scene as `scene.bmp` and open it in an image viewer.

### Desired output
The same picture as the PPM version. Check with widths `1`, `2`, `3` and `4` (each has different padding: 1, 2, 3 and 0 bytes); the `file` command on Linux prints `PC bitmap, Windows 3.x format, <w> x <h> x 24`.

## Exercise 2 — The render server
### Task
Write `render_server <port> [--threads N] [--max-requests M]` with the routes `/` (an HTML page with four `<img>` renders at different angles), `/render?w=&h=&angle=` (BMP) and `/stats` (`served=… rendered=… errors=…`). Use a thread pool with a bounded queue.

### Desired output
Open `http://localhost:8080/` in a browser and see four renders of the scene from different sides. Then:

```text
curl -s -o r.bmp -w "%{http_code} %{content_type}\n" "http://localhost:8080/render?w=320&h=200&angle=0.6"
200 image/bmp
```

### Hints
1. Start single-threaded: accept, handle, close. Add the pool only when every route works.
2. To place the camera on a circle around the scene: `eye = (7 sin(angle), 2.2, -7 cos(angle))`, then build `forward`, `right` and `up` vectors pointing at the center. If the picture is upside down, `up` points down: swap the order of the cross product.
3. On Windows link `-lws2_32` and call `WSAStartup` (C 09).

## Exercise 3 — Break it, then prove it holds
### Task
Write a test script (Bash or Python) that starts the server with `--max-requests`, then sends: a valid render, `w=5000`, `w=abc`, a missing `h`, a `POST`, an unknown path, a 10 KB request line, a connection that sends nothing, and 8 parallel valid renders. It checks every status code and the final `served=` line.

### Desired output
Statuses `200 400 400 400 405 404 431 (timeout, no response)` and eight `200`s; the server exits normally with the right counts. Then compare the time for the 8 parallel renders with `--threads 1` and `--threads 4`.

## Exercise 4 — A cache *(stretch)*
### Task
Many clients ask for the same image. Add a cache: a hash map (C 06) from the normalised query (`w`, `h`, `angle` rounded to 2 decimals) to the encoded BMP, with a total size limit and least-recently-used eviction. Protect it with a mutex. Report hits and misses in `/stats`.

---

## Checkpoint quiz

1. What ends the headers of an HTTP request?
2. Why can one `recv` return only part of a request?
3. Name the three layout traps of a 24-bit BMP.
4. Why use a fixed pool of N threads instead of one thread per connection?
5. What does a bounded queue give the server when all workers are busy?
6. Which two limits stop a single request from using unbounded memory and CPU?
7. What is a "slowloris" client, and how does the server defend against it?

## Recap

- HTTP/1.0: request line + headers + empty line; response = status + headers (`Content-Length`) + body.
- BMP: 54-byte little-endian header, BGR, bottom-up rows padded to 4 bytes.
- Thread pool + bounded queue = parallel work with back-pressure.
- Validate every parameter, cap sizes, time out idle clients, never crash.

## Spaced review

1. (C 05) How do you write a 32-bit value in little-endian order byte by byte?
2. (C 10) Why must `pthread_cond_wait` be called in a `while` loop?

## Answers

Quiz: 1. An empty line: `\r\n\r\n`. 2. TCP is a byte stream; data arrives in pieces of any size. 3. Blue-green-red order, bottom row first, rows padded to a multiple of 4 bytes. 4. Threads cost memory and switching time; about one per core is enough for CPU-bound work, and the count stays bounded however many clients connect. 5. Back-pressure: the accept loop waits, so clients queue in the OS backlog instead of using server memory. 6. Maximum width and height (`1024`), and the fixed request buffer (8 KB). 7. A client that connects and sends nothing (or very slowly) to tie up a worker; the receive timeout closes it after 5 seconds.

Spaced review: 1. `b[0] = v & 0xFF; b[1] = (v >> 8) & 0xFF; b[2] = (v >> 16) & 0xFF; b[3] = (v >> 24) & 0xFF;`. 2. Wake-ups can be spurious, or another thread may have taken the item first, so the condition must be checked again.

## References (concept-only)
- [MDN — An overview of HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)
- [BMP file format](https://en.wikipedia.org/wiki/BMP_file_format)
- Beej's Guide to Network Programming — `recv`, `send`, partial reads
