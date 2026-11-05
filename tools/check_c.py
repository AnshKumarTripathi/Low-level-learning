#!/usr/bin/env python3
"""Checks the C side of the course.

1. Every runnable C example in the Markdown (a ```c block containing `int main`)
   must compile. Warnings are reported but allowed: some examples show bugs on purpose.
2. Every C solution under solutions/c must compile. Lab files (marked
   "LOCAL LEARNING ONLY") are built without -Werror; everything else with -Werror.
3. The chapter 00 solutions are run against the desired outputs in the modules.

Usage:  python tools/check_c.py [--cc gcc] [--sanitize]
Run from the repository root. Exits non-zero if anything fails.
"""
import argparse
import os
import pathlib
import platform
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
IS_WINDOWS = platform.system() == "Windows"
EXE = ".exe" if IS_WINDOWS else ""

failures = []


def fail(msg):
    failures.append(msg)
    print("FAIL:", msg)


def base_flags(cc, werror, sanitize):
    flags = ["-std=c17", "-Wall", "-Wextra", "-Wpedantic", "-g"]
    if werror:
        flags.append("-Werror")
    if IS_WINDOWS:
        flags.append("-D__USE_MINGW_ANSI_STDIO=1")      # see 00-setup: MinGW's %zu false warning
    if sanitize:
        if IS_WINDOWS:   # MinGW GCC has no ASan runtime: use UBSan trap mode
            flags += ["-fsanitize=undefined,bounds", "-fsanitize-undefined-trap-on-error"]
        else:
            flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    return [cc] + flags


def libs_for(text):
    libs = []
    if "pthread.h" in text:
        libs.append("-pthread")
    if "winsock2.h" in text and IS_WINDOWS:
        libs.append("-lws2_32")
    if "math.h" in text:
        libs.append("-lm")
    if "SDL2/SDL.h" in text:
        libs += sdl2_flags()
    return libs


def sdl2_flags():
    """Compiler flags for SDL2: from SDL2_DIR (an unpacked SDL2 dev package), else sdl2-config, else []."""
    d = os.environ.get("SDL2_DIR")
    if d:
        return ["-I", os.path.join(d, "include"), "-L", os.path.join(d, "lib"), "-lSDL2"]
    if shutil.which("sdl2-config"):
        r = subprocess.run(["sdl2-config", "--cflags", "--libs"], capture_output=True, text=True)
        return r.stdout.split()
    return []


def compile_files(cc, sources, out, werror, sanitize, include_dir=None):
    text = "".join(pathlib.Path(s).read_text(encoding="utf-8") for s in sources)
    cmd = base_flags(cc, werror, sanitize)
    if include_dir:
        cmd += ["-I", str(include_dir)]
    cmd += [str(s) for s in sources] + ["-o", str(out)] + libs_for(text)
    r = subprocess.run(cmd, capture_output=True, text=True)
    return r.returncode, r.stderr


def check_markdown(cc, work):
    count = 0
    for md in sorted(ROOT.rglob("*.md")):
        if ".git" in md.parts or md.name == "course-review.md":
            continue
        lines = md.read_text(encoding="utf-8").splitlines()
        i = 0
        while i < len(lines):
            if lines[i].strip() == "```c":
                indent = len(lines[i]) - len(lines[i].lstrip())
                j = i + 1
                while lines[j].strip() != "```":
                    j += 1
                body = "\n".join(l[indent:] for l in lines[i + 1:j]) + "\n"
                if "int main" in body and '#include "' not in body:   # skip multi-file fragments
                    count += 1
                    src = work / f"md_{count}.c"
                    src.write_text(body, encoding="utf-8")
                    rc, err = compile_files(cc, [src], work / f"md_{count}{EXE}", False, False)
                    where = f"{md.relative_to(ROOT)}:{i + 2}"
                    if rc != 0:
                        fail(f"example does not compile: {where}\n{err[:500]}")
                    elif "warning" in err:
                        print(f"note: {where} compiles with warnings (may be intentional)")
                i = j + 1
            else:
                i += 1
    print(f"markdown examples compiled: {count}")


def check_solutions(cc, work, sanitize):
    count = 0
    sol = ROOT / "solutions" / "c"
    for path in sorted(sol.rglob("*.c")):
        if "ex2-warnings" in path.parts:
            continue                      # bad.c must FAIL to compile: that is the exercise
        text = path.read_text(encoding="utf-8")
        if "int main" not in text:
            continue                      # library file of a multi-file solution
        if "SDL2/SDL.h" in text and not sdl2_flags():
            print(f"note: skipped {path.relative_to(ROOT)} (SDL2 not found; set SDL2_DIR or install sdl2-config)")
            continue
        siblings = [p for p in path.parent.glob("*.c") if p != path and "int main" not in p.read_text(encoding="utf-8")]
        sources = [path] + siblings
        lab = "LOCAL LEARNING ONLY" in text or "Build WITHOUT -Werror" in text
        out = work / f"sol_{count}{EXE}"
        count += 1
        rc, err = compile_files(cc, sources, out, not lab, sanitize, include_dir=path.parent)
        if rc != 0:
            fail(f"solution does not compile: {path.relative_to(ROOT)}\n{err[:500]}")
    print(f"solutions compiled: {count}")


def run(exe, stdin="", args=()):
    r = subprocess.run([str(exe), *args], input=stdin.encode(), capture_output=True, timeout=20)
    norm = lambda b: b.decode(errors="replace").replace("\r\n", "\n")
    return r.returncode, norm(r.stdout), norm(r.stderr)


def check_foundation_outputs(cc, work, sanitize):
    sol = ROOT / "solutions" / "c" / "00-foundation"
    cases = [
        ("A1", "", (), "name=Ansh\ncity=Delhi\ngoal=learn C\n", 0),
        ("A3", "", ("--help",), "usage: prog [--help | --fail]\n", 0),
        ("A3", "", ("--fail",), "", 2),
        ("B1", "42\n", (), "n=42\nright=[   42]\nleft=[42   ]\n", 0),
        ("B2", "9.5 3\n", (), "price=9.50\nqty=3\n", 0),
        ("C2", "-7 2\n", (), "quot=-3\nrem=-1\nexact=-3.50\n", 0),
        ("C2", "7 0\n", (), "", 1),
        ("D1", "80\n", (), "grade=B\n", 0),
        ("D2", "15\n", (), "1\n2\nFizz\n4\nBuzz\nFizz\n7\n8\nFizz\nBuzz\n11\nFizz\n13\n14\nFizzBuzz\n", 0),
        ("D3", "pow 2 10\n", (), "result=1024\n", 0),
        ("E1", "4 9 -2\n", (), "max=9\nmin=-2\n", 0),
        ("E2", "20\n", (), "primes=2 3 5 7 11 13 17 19\ncount=8\n", 0),
        ("F1", "", (), "max=30\nmin=17\navg=23.4\n", 0),
        ("F2", "4\n1 2 3 4\n", (), "reversed=4 3 2 1\n", 0),
        ("F3", "5\n2 4 6 8 10\n", (), "avg=6.00\nabove=2\n", 0),
        ("G1", "Hello World 42\n", (), "vowels=3\nspaces=2\ndigits=2\n", 0),
        ("G2", "Level\n", (), "palindrome=yes\n", 0),
        ("G3", "the quick brown fox\n", (), "longest=quick len=5\n", 0),
        ("G3", "abc", (), "longest=abc len=3\n", 0),
        ("H1", "17 5\n", (), "quot=3 rem=2\n", 0),
        ("H2", "5\n4 9 2 9 1\n", (), "max=9 index=1\n", 0),
        ("I1", "-1 2 2 -2\n", (), "dist2=25\n", 0),
        ("I2", "averyveryverylongname 3\n", (), "", 1),
        ("E3", "2\n", (), "move disk 1 from A to B\nmove disk 2 from A to C\nmove disk 1 from B to C\nmoves=3\n", 0),
        ("E3", "0\n", (), "", 1),
        ("B-mini", "30 4 2.5\n", (), "age, number of cats, price of one cat: \nAt age 30, I adopted 4 cats.\n"
            "Each cat cost 2.50 coins, so the bill was 10.00 coins.\nBy age 40 I will have 8 cats.\n", 0),
        ("C-mini", "7 3 8 12.50\n", (), "pizzas=3\nleftover=3\ntotal=37.50\neach=5.36\n", 0),
        ("C-mini", "0 3 8 12.50\n", (), "", 1),
        ("D-mini", "42\n50 25 40 42\n", (), "too high\ntoo low\ntoo low\ncorrect! tries=4\n", 0),
        ("D-mini", "7\n1 2 3 4 5 6 8\n", (), "too low\n" * 6 + "too high\nout of tries, it was 7\n", 0),
        ("E-mini", "1 4 3\n2 3\n0\n", (), "+--+\n|  |\n+--+\n  *\n ***\n*****\n", 0),
        ("F-mini", "4\n3 7 0 5\n", (), " 1 |### 3\n 2 |####### 7\n 3 | 0\n 4 |##### 5\nbest=2\n", 0),
        ("G-mini", "3\nHello, World!\n", (), "Khoor, Zruog!\n", 0),
        ("G-mini", "-3\nKhoor, Zruog!\n", (), "Hello, World!\n", 0),
        ("H-mini", "1 15\n1 10\n2 7\n1 10\n0\n", (), "coins=5\nnot enough coins (have 5)\ncoins=12\ncoins=2\nfinal coins=2\n", 0),
        ("I-mini", "w\nn\ns\ne\nn\n", (), "You are in the Hall.\nYou can't go that way.\nYou are in the Library.\n"
            "You are in the Hall.\nYou are in the Kitchen.\nYou are in the Vault.\nYou found the treasure in 4 moves!\n", 0),
        # Chapter 01 Subchapter G (PPM images)
        ("../01-low-level-mental-model/G1", "3 1\n", (), "P3\n3 1\n255\n255 153 51\n255 255 255\n19 136 8\n", 0),
        ("../01-low-level-mental-model/G1", "2 1\n", (), "", 1),
        ("../01-low-level-mental-model/G2", "2 1\n#FF8800 #000000\n", (),
            "P3\n2 2\n255\n255 136 0\n0 0 0\n0 0 0\n255 136 0\n", 0),
        ("../01-low-level-mental-model/G2", "2 1\nFF8800\n", (), "", 1),
    ]
    built = {}
    for name, stdin, args, expected_out, expected_rc in cases:
        if name not in built:
            exe = work / f"f00_{name.replace('/', '_').replace('.', '')}{EXE}"
            rc, err = compile_files(cc, [sol / f"{name}.c"], exe, True, sanitize)
            if rc != 0:
                fail(f"solution {name} does not compile\n{err[:300]}")
                continue
            built[name] = exe
        rc, out, _ = run(built[name], stdin, args)
        if rc != expected_rc or out != expected_out:
            fail(f"solution {name} with input {stdin!r} {args}: got rc={rc} out={out!r}")
    # The Module J tests describe the roster contract; their inputs must exist.
    for f in ("j_input.txt", "j_expected.txt", "j_edge_input.txt", "j_edge_expected.txt"):
        if not (ROOT / "c" / "00-foundation" / "tests" / f).exists():
            fail(f"missing Module J test file {f}")
    print(f"chapter 00-01 output checks: {len(cases)}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cc", default=os.environ.get("CC", "gcc"))
    ap.add_argument("--sanitize", action="store_true", help="build solutions with sanitizers")
    a = ap.parse_args()
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        check_markdown(a.cc, work)
        check_solutions(a.cc, work, a.sanitize)
        check_foundation_outputs(a.cc, work, a.sanitize)
    if failures:
        print(f"\n{len(failures)} failure(s)")
        sys.exit(1)
    print("\nall C checks passed")


if __name__ == "__main__":
    main()
