# ECE 412 — Lab 1: Pointers & Dynamic Memory

**Due Sun Sep 13, 11:59 pm on Gradescope.** 

## Build & run

```
make                          # builds ./toolkit with the realloc growth version
make GROW_WITH_REALLOC=0     # same binary, the manual malloc+copy+free version
make test                     # runs it
make valgrind                 # the graded check -- paste the summary into WRITEUP.md
make clean
```

(`GROW_WITH_REALLOC` is the same name as the `#define` in `intarray.cpp` — the Makefile just passes it through with `-D`.)

## What goes where

| File | You write |
|---|---|
| `mystr.cpp` | Part A — the four string functions (headers are fixed; don't edit `.h` files) |
| `intarray.cpp` | Part B — `IntArray`, **both** growth versions (`GROW_WITH_REALLOC` 0 and 1) |
| `main.cpp` | Part C — the driver: growth demo, name sort, every failure path |
| `WRITEUP.md` | every prompt answered — this carries the explanation points |

## Ground rules (the short version)

- `g++ -std=c++11 -Wall -Wextra -Werror` — zero warnings; the autograder builds your `intarray.cpp` **both** ways (`-DGROW_WITH_REALLOC=0` and `=1`).
- **valgrind-clean is graded:** 0 errors, 0 bytes definitely or indirectly lost.
- C-subset only. Banned: `std::string`, `std::vector`, `std::cout`, classes, references, `new`/`delete`, templates, the STL, `auto`, range-`for`, `<cstring>`, etc.  If  in doubt, ASK instead of losing points.
- Every allocation NULL-checked; never `exit()` from a library function; `ia_destroy` twice must not crash.
- AI-assisted lines carry the citation comment from the syllabus, and `WRITEUP.md` names one bug you fixed in AI-suggested code (or states you
  wrote it unaided).

Submit to Gradescope: `mystr.cpp`, `intarray.cpp`, `main.cpp`, `Makefile`, `WRITEUP.md` (the `.h` files are fixed and provided; submitting them unchanged is fine).
