# Setup & getting unstuck

[Roadmap](../ROADMAP.md) · [Glossary](../GLOSSARY.md) · [Next: C 00 — Module A →](../c/00-foundation/modules/A-what-a-program-is.md)

> Time: about 1–2 hours · Prerequisites: none · You will finish with: a working C compiler, a terminal you can use, and a plan for when things go wrong.

## Learning objectives

By the end of this page you will be able to:
- Install a C compiler and an editor on your computer.
- Move around folders in a terminal and run programs from it.
- Compile and run a C program, and check its exit code.
- Read a compiler error and fix the first one.
- Debug a small program by printing values.

---

## 1. What you are installing (plain words)

- **Editor**: where you type your program. It saves a text file such as `hello.c`.
- **Compiler** (`gcc` or `clang`): a program that reads your `.c` text file and produces a **program** (an executable file) your computer can run.
- **Terminal**: a window where you type commands instead of clicking. You use it to run the compiler and your programs.
- **Debugger** (`gdb` or `lldb`): a tool to pause a running program and look inside it. You will need it from chapter 02 on.

---

## 2. Install a compiler

Pick the section for your operating system.

### Windows

Choose **one** option.

**Option A — MSYS2 (recommended for this course)**

1. Download the installer from [msys2.org](https://www.msys2.org/) and install it with the default folder `C:\msys64`.
2. Open **MSYS2 UCRT64** from the Start menu.
3. Install the compiler and debugger:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb
   ```
4. To use `gcc` from PowerShell too, add `C:\msys64\ucrt64\bin` to your **PATH** environment variable (Start → "Edit the system environment variables" → Environment Variables → Path → New). Close and reopen PowerShell afterwards.

**Option B — WSL2 (a Linux system inside Windows)**

1. Open PowerShell **as Administrator** and run `wsl --install`. Restart when asked.
2. Open **Ubuntu** from the Start menu, create a user, then run:
   ```bash
   sudo apt update
   sudo apt install build-essential gdb
   ```
3. From now on, follow the **Linux** instructions inside the Ubuntu window.

WSL2 is the easiest way to get AddressSanitizer and Valgrind on Windows, which chapter 02 uses.

### macOS

Open **Terminal** (Applications → Utilities) and run:

```bash
xcode-select --install
```

This installs `clang`. The command `gcc` also works, but on macOS it is Clang under another name. For debugging, macOS uses `lldb` instead of `gdb`.

### Linux

Debian or Ubuntu:

```bash
sudo apt update
sudo apt install build-essential gdb
```

Fedora:

```bash
sudo dnf install gcc gdb make
```

### Check it worked

Open a **new** terminal and run:

```bash
gcc --version
```

You should see a version line such as `gcc (...) 13.2.0`. If you see `command not found` or `not recognized`, the compiler is not on your PATH: reopen the terminal, then recheck the install step.

---

## 3. Install an editor

Any plain-text editor works. A good free choice is **[Visual Studio Code](https://code.visualstudio.com/)** with the **C/C++** extension by Microsoft (Extensions panel → search "C/C++").

Do **not** use Word or other document editors: they save formatting, not plain text.

---

## 4. Terminal basics

| What you want | Linux / macOS / WSL | Windows PowerShell |
|---------------|---------------------|--------------------|
| Where am I? | `pwd` | `pwd` |
| List files here | `ls` | `ls` (or `dir`) |
| Go into a folder | `cd c-practice` | `cd c-practice` |
| Go up one folder | `cd ..` | `cd ..` |
| Make a folder | `mkdir c-practice` | `mkdir c-practice` |
| Run a program in this folder | `./hello` | `.\hello.exe` |
| Exit code of the last program | `echo $?` | `echo $LASTEXITCODE` |

Tips:
- If a folder name has spaces, put it in quotes: `cd "My Code"`.
- Press **Tab** to auto-complete file and folder names.
- Press the **Up arrow** to bring back your previous command.

Make a practice folder now and go into it:

```bash
mkdir c-practice
cd c-practice
```

---

## 5. Your first compile and run

1. In your editor, create `hello.c` inside `c-practice` with this text:

   ```c
   #include <stdio.h>

   int main(void)
   {
       printf("Hello, world\n");
       return 0;
   }
   ```

2. Save the file. Then, in the terminal (inside `c-practice`), compile it:

   ```bash
   gcc -std=c17 -Wall -Wextra -g hello.c -o hello
   ```

   What each part means:

   | Part | Meaning |
   |------|---------|
   | `gcc` | run the compiler |
   | `-std=c17` | use the C17 version of the language |
   | `-Wall -Wextra` | turn on warnings — the compiler points out likely mistakes |
   | `-g` | keep debug information (for the debugger later) |
   | `hello.c` | your source file |
   | `-o hello` | name the output program `hello` (`hello.exe` on Windows) |

   No output means success.

3. Run it:
   - Linux / macOS / WSL: `./hello`
   - Windows PowerShell: `.\hello.exe`

   You should see `Hello, world`.

4. Check the exit code (`echo $?` or `echo $LASTEXITCODE`). It prints `0`, because `main` returned `0`.

**Remember**: after every change to `hello.c`, **save and compile again**. Running without recompiling runs the old program.

**Windows note**: with MinGW-w64 GCC, `printf("%zu", ...)` can produce a false warning `unknown conversion type character 'z' in format`. Add `-D__USE_MINGW_ANSI_STDIO=1` to the compile command to fix it.

---

## 6. Reading compiler errors

Every message has the same shape:

```text
file:line:column: error: what went wrong
```

- **error** — no program was made. You must fix it.
- **warning** — a program *was* made, but something is probably wrong. Treat warnings as bugs too.
- **note** — extra help for the message above it.

**Golden rule: fix the first error first, then compile again.** One mistake often causes a chain of later messages that disappear once the first is fixed.

### Example 1 — missing semicolon

```c
    printf("Hello, world\n")
    return 0;
```

```text
hello.c: In function 'main':
hello.c:5:29: error: expected ';' before 'return'
    5 |     printf("Hello, world\n")
      |                             ^
      |                             ;
```

The `^` points to exactly where the `;` belongs: the end of line 5. GCC even shows the fix underneath. Note that the mistake is often at the end of the line *before* the line the message mentions.

### Example 2 — variable used before it is declared

```c
    count = 5;
    printf("%d\n", count);
```

```text
count.c:5:5: error: 'count' undeclared (first use in this function)
    5 |     count = 5;
      |     ^~~~~
```

C must be told a variable's type before you use it. Fix: `int count = 5;`.

### Example 3 — missing `#include`

```text
int main(void)
{
    printf("Hello, world\n");
    return 0;
}
```

```text
noinc.c:3:5: warning: implicit declaration of function 'printf' [-Wimplicit-function-declaration]
noinc.c:1:1: note: include '<stdio.h>' or provide a declaration of 'printf'
```

On GCC 13 this is only a **warning**, so a program is still built — but it is built on a guess. GCC 14 and newer, and Clang, report it as an **error**. The note tells you the fix: add `#include <stdio.h>`. This is why the course always compiles with `-Wall -Wextra`, and why you should never ignore a warning.

---

## 7. When you are stuck

Work through this list in order:

1. **Did you save the file?** Then compile again.
2. **Are you compiling the right file, in the right folder?** Run `ls` / `dir` to check.
3. **Read the first error or warning** only. Look at the line it names *and the line before it*.
4. **Print what is happening** (printf debugging). Add temporary lines like:
   ```c
   printf("DEBUG: n=%d i=%d\n", n, i);
   ```
   at key points, so you can see the real values instead of guessing. Delete them when the bug is fixed.
5. **Make it smaller.** Remove code until the problem disappears; the last thing you removed is the suspect.
6. **Search the exact error text** (in quotes) on the web.
7. **Ask for help** with three things: your code, the exact compile command, and the full output.

From chapter 02 on, you will add a **debugger** and **sanitizers** to this list.

---

## Recap

- You type code in an **editor**, compile it with **gcc**, and run the result in a **terminal**.
- Always compile with `gcc -std=c17 -Wall -Wextra -g file.c -o name`.
- Recompile after every change.
- Errors stop the build; warnings do not, but you should treat both as bugs.
- Fix the first message first.

**Next:** [Module A — What a C program is](../c/00-foundation/modules/A-what-a-program-is.md)
