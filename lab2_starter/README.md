# ECE 412 -- Lab 2: Structs, Opaque Types & a Record ADT

**Due Sun Sep 20, 11:59 pm on Gradescope.**

## Build & run

```
make          # compiles mystr.o, db.o, main.o and links ./db
make test     # runs ./db
make valgrind # graded check -- paste the summary into WRITEUP.md
make clean
```

## What goes where

| File | You write |
|---|---|
| `mystr.cpp` | From Lab 1 -- bring your file, or reimplement. `db.cpp` uses `my_strdup`. |
| `db.cpp` | The ADT. The `struct DB { ... }` definition MUST stay in this file only. |
| `main.cpp` | The driver: the six numbered activities from the handout. |
| `records.txt`, `bad_records.txt` | A valid save file and a deliberately malformed one. |
| `WRITEUP.md` | Every prompt answered -- these carry the explanation points. |

The two `.h` files are fixed and provided. **Do not modify them.** The autograder compiles its tests against those exact prototypes.

## Ground rules (the short version)

- `g++ -std=c++11 -Wall -Wextra -Werror` -- zero warnings.
- **Opacity is graded (15 pts).** `struct DB { ... }` lives ONLY in `db.cpp`. `main.cpp` may not contain `db->` of any kind and may not `#include "db.cpp"`. A quick self-check: if someone deletes the struct definition from `db.h` (there shouldn't be one there anyway), your `main.cpp` must still compile.
- **valgrind-clean is graded (15 pts):** 0 errors, 0 bytes definitely or indirectly lost.
- **Separate compilation is graded (10 pts).** The Makefile in this starter already has per-object rules -- do not collapse it back into a single `g++ *.cpp -o db` line.
- C-subset only. Banned: `class`, member functions, `std::string`, `std::vector`, `std::cout`, references, `new`/`delete`, templates, the STL, `auto`, range-`for`. If in doubt, ask.
- Every allocation NULL-checked; `DB_destroy(NULL)` must be a no-op; `DB_destroy` frees names first, then the array, then the DB.
- AI-assisted lines carry the citation comment from the syllabus, and `WRITEUP.md` names one bug you fixed in AI-suggested code (or states you wrote it unaided).

## Submit to Gradescope

`mystr.cpp`, `db.cpp`, `main.cpp`, `Makefile`, `records.txt`, `bad_records.txt`, `WRITEUP.md`. The `.h` files are fixed; submitting them unchanged is fine.
