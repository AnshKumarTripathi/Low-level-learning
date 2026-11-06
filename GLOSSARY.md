# Glossary

Plain-language definitions of terms used across the course. The "First seen" column says where the course introduces each one.

[Roadmap](ROADMAP.md) · [Setup](00-setup/README.md)

| Term | Meaning | First seen |
|------|---------|------------|
| **address** | The number that says where a piece of data sits in memory. `&x` gives the address of `x`. | C 00-B, 00-H |
| **array** | A fixed number of values of the same type stored side by side in memory, numbered from `0`. | C 00-F |
| **ASLR** | Address space layout randomization: the OS places stack, heap and code at different addresses each run, which makes attacks harder. | C 01-F |
| **assembly** | A human-readable form of the machine instructions the CPU runs. | C 01-D, C 12 |
| **backtrace** | The list of function calls that led to the current point, newest first. `bt` in gdb. | C 02 |
| **binary search** | Finding a value in a sorted array by checking the middle and discarding half each step: O(log n). | C 06 |
| **bit / byte** | A bit is a single 0 or 1. A byte is 8 bits, the smallest unit with its own address. | C 01-A |
| **buffer** | A block of memory used to hold data, usually an array of `char` or `unsigned char`. | C 00-G |
| **buffer overflow** | Writing past the end of a buffer, which corrupts whatever memory comes next. | C 00-F |
| **compiler** | The program (`gcc`, `clang`) that turns `.c` source text into machine code. | Setup |
| **dangling pointer** | A pointer to memory that is no longer valid (the variable ended or the block was freed). | C 00-E |
| **declaration / definition** | A declaration says a name and its type exist; a definition also creates it (allocates storage or gives the function body). | C 00-E, 00-I |
| **depth buffer** | One extra value per pixel that records the nearest surface drawn so far, so far objects cannot cover near ones. Also called a z-buffer. | G5 |
| **dereference** | Follow a pointer to the value it points at: `*p`. | C 00-H |
| **endianness** | The order a multi-byte number's bytes are stored in. Little-endian puts the lowest byte first (x86). | C 01-C |
| **executable** | The runnable program file the compiler and linker produce. | Setup |
| **exit code** | The number a program returns to the OS when it ends. `0` means success. | C 00-A |
| **format string** | The first argument to `printf`/`scanf`, which says how to print or read values (`%d`, `%s`, …). | C 00-B |
| **framebuffer** | A block of memory holding the color of every pixel of an image or screen. | G1 |
| **game loop** | The repeating cycle of a real-time program: input, update, draw, present. | G7 |
| **header file** | A `.h` file of declarations that other files `#include`. | C 00-I |
| **heap** | Memory you request at run time with `malloc` and give back with `free`. | C 01-E |
| **hexadecimal** | Base-16 numbers (`0`–`9`, `A`–`F`); each hex digit is exactly 4 bits. Written `0x…` in C. | C 01-A |
| **HTTP** | The text-based protocol browsers and web servers use: a request with headers, then a response with a status, headers and a body. | G9 |
| **linker** | The tool that joins object files and libraries into one executable. "undefined reference" errors come from it. | C 01-D |
| **Makefile** | A file listing build targets, their dependencies and commands. `make` rebuilds only what is out of date. | C 02 |
| **matrix** | A grid of numbers. In 3D graphics, a 4 × 4 matrix stores a rotation, scale or move, and multiplying matrices combines them. | G4 |
| **memory leak** | Heap memory that is never freed after it is no longer needed. | C 02 |
| **null terminator** | The `'\0'` byte that marks the end of a C string. | C 00-G |
| **`NULL`** | A pointer value that points to nothing. Dereferencing it is undefined behavior. | C 00-H |
| **object file** | The `.o`/`.obj` output of compiling one source file, before linking. | C 01-D |
| **pixel** | One dot of an image, usually three bytes: red, green and blue. | C 01-G |
| **pointer** | A variable that holds an address. | C 00-H |
| **PPM** | A very simple image file format: a short text header followed by pixel values. | C 01-G |
| **preprocessor** | The first compile step: handles `#include` and `#define` by editing the source text. | C 01-D |
| **prototype** | A function declaration without the body, so the compiler knows the function before it is defined. | C 00-E |
| **RAII** | C++ idea: an object acquires a resource in its constructor and releases it in its destructor, so cleanup is automatic. | C++ B2 |
| **recursion** | A function calling itself on a smaller problem, until a base case stops it. | C 00-E |
| **sanitizer** | Extra checks the compiler adds to a program to catch memory and undefined-behavior bugs while it runs. | C 02 |
| **scope** | The part of the code where a name can be used. | C 00-E |
| **segmentation fault** | The OS stopping a program that touched memory it may not use. | C 00-B |
| **`size_t`** | Unsigned integer type for sizes and counts; what `sizeof` returns. Printed with `%zu`. | C 00-C |
| **stack** | Memory for function calls: each call gets a frame for its local variables, freed automatically on return. | C 00-E, C 01-E |
| **stack frame** | The block of stack memory for one function call: locals, saved return address, and more. | C 00-E |
| **stderr / stdout / stdin** | The three standard streams: error output, normal output, and input. | C 00-A |
| **struct** | A type that groups named fields into one record. | C 00-I |
| **thread pool** | A fixed set of worker threads that take jobs from a shared queue. | C 10, G9 |
| **translation unit** | One `.c` file after the preprocessor has handled its `#include`s; the unit the compiler works on. | C 01-D |
| **two's complement** | How CPUs store negative integers: flip all bits and add 1. | C 05 |
| **undefined behavior (UB)** | Code the C standard gives no meaning to (e.g. out-of-bounds access, signed overflow). Anything can happen, including "seems to work". | C 00-B |
| **unit test** | A small program that calls a function with known inputs and checks the answers automatically. | C 02 |
| **use-after-free** | Using heap memory after it was freed. | C 02 |
| **warning** | A compiler message about likely mistakes. The program still builds, but you should treat it as a bug. | Setup |
