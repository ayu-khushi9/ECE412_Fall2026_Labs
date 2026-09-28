# Lab 2 — Structs, Opaque Types & a Record ADT 

| | |
|---|---|
| **Assigned** | Tue Sep 15 |
| **Due** | **Sun Sep 20, 11:59 pm** (Gradescope) |
| **Weight** | 1% |
| **Builds on** | L7 (structs, ADTs & multi-file programs, Sep 15) — and your Lab 1 `IntArray` and `my_strdup` |
| **Language** | **C-compatible subset of C++11** |
| **Work** | Individual |

## Goal

Build a real **abstract data type** with the tools you have: a `struct`, a set of free functions, and a header that **does not reveal the representation**. Lab 1 gave you a growable buffer whose fields the caller could poke at freely (`a->cap = 9999;` compiles). This week the caller *physically cannot* — the type is **opaque**, declared in the header and defined only in the `.cpp`. That is information hiding enforced by the compiler, three lectures before you meet `private:`.

The second half of the lesson is the multi-file build: three translation units, a header each, and a `Makefile` that compiles each to a `.o` and links them.

## The assignment

A small **student record database**. Two types, and the split between them is the design lesson:

- **`Record`** is a plain value type. It is **public** — it appears in the header in full — because clients need to read the fields of a record they found.
- **`DB`** is **opaque**. Clients hold a `DB *` and pass it around; they cannot see, size, allocate on the stack, or modify its internals.

### `db.h`

```cpp
#ifndef DB_H
#define DB_H

#include <cstddef>   /* size_t */

/* A record. The DB owns the `name` buffer; clients must not free it. */
typedef struct {
    int     id;      /* unique, >= 0                        */
    char   *name;    /* malloc'd copy owned by the DB       */
    double  gpa;     /* 0.0 .. 4.0                          */
} Record;

/* Incomplete ("opaque") type: clients may hold a DB *, but they cannot
   see what is inside, cannot declare `DB db;` on the stack, and cannot
   write db->count. Only db.cpp knows the layout. */
typedef struct DB DB;

DB     *DB_create(size_t initial_capacity);
                /* returns NULL on allocation failure */

void    DB_destroy(DB *db);
                /* frees every record's name, the record array, and the DB
                   itself. DB_destroy(NULL) is a no-op. */

int     DB_add(DB *db, int id, const char *name, double gpa);
                /* deep-copies name. Returns 1 on success, 0 on allocation
                   failure or duplicate id. Grows the array when full. */

const Record *DB_find(const DB *db, int id);
                /* returns a pointer to the stored record, or NULL if absent.
                   The pointer is const: clients read, they do not mutate,
                   and they never free it. */

int     DB_remove(DB *db, int id);
                /* frees that record's name, removes it, keeps the array
                   contiguous. Returns 1 if removed, 0 if not present. */

int     DB_update_gpa(DB *db, int id, double gpa);
                /* 1 if updated, 0 if id absent */

size_t  DB_size(const DB *db);
size_t  DB_capacity(const DB *db);

void    DB_print(const DB *db);
                /* one line per record, printf, in insertion order */

int     DB_save(const DB *db, const char *path);
int     DB_load(DB *db, const char *path);
                /* text format, one record per line: id,name,gpa
                   Returns 1 on success, 0 on I/O or parse failure. */

#endif /* DB_H */
```

### `db.cpp`

```cpp
struct DB {
    Record *items;    /* malloc'd array of Record, grows by doubling */
    size_t  count;
    size_t  cap;
};
```

That definition lives **only** in `db.cpp`. Nothing in `main.cpp` may know it exists.

Growth is Lab 1's doubling policy applied to an array of `struct` instead of an array of `int` — start at `initial_capacity` (use 4 if the caller passes 0), double when `count == cap`. `sizeof(Record)` is not `sizeof(int)`; use `sizeof(Record)` and let the compiler tell you how big it is, do not hard-code a number.

### `main.cpp`

A driver that:

1. Creates a DB, adds ≥20 records (forcing at least two growths), prints size and capacity as it goes.
2. Finds a present id and an absent id; prints both outcomes.
3. Removes the **first**, a **middle**, and the **last** record, and prints the DB after each. Removing from the middle without leaving a hole is the interesting case — say in your README whether you shift the tail down or swap the last element into the gap.4. Attempts a duplicate `DB_add` and checks that it fails.
5. Saves to a file, destroys the DB, creates a fresh one, loads the file back, and prints it — the round trip must produce the same records.
6. Feeds `DB_load` a deliberately malformed file and shows that it fails cleanly without leaking or crashing.

## Requirements

- **The opaque type is graded.** `struct DB { ... }` appears in `db.cpp` and nowhere else. `main.cpp` must contain no `db->` of any kind, and must not `#include "db.cpp"`. If your `main.cpp` compiles after someone deletes the struct definition from `db.h`, you did it right — because it was never there.
- **Deep copy of names.** `DB_add` stores its own `malloc`'d copy of `name` (reuse `my_strdup` from Lab 1 — bring the file along, or reimplement it). Storing the caller's `const char *` directly is a dangling-pointer bug the moment the caller's buffer goes out of scope; your README must say so in one sentence.
- **`DB_destroy` frees everything**: every `name`, then the `items` array, then the `DB` itself, in that order. Freeing `items` first orphans every `name` — that is the leak valgrind will report as *indirectly lost*.
- **`DB_remove` frees the removed record's `name`** before removing the slot. This is the single most commonly missed `free` in the lab.
- Every allocation NULL-checked; a failed allocation leaves the DB usable and returns 0.
- Read-only functions take `const DB *`. `DB_find` returns `const Record *` — clients read, they do not mutate. Explain in the README why returning a **non**-`const` `Record *` would break the ADT (hint: the client could `free(r->name)` or overwrite `r->id` behind the DB's back).
- **Multi-file build**, graded: `db.h`/`db.cpp`, `mystr.h`/`mystr.cpp` (from Lab 1), `main.cpp`. Header guards in every header. The `Makefile` compiles each `.cpp` to a `.o` and links the objects — editing `main.cpp` must not rebuild `db.cpp`.

## Constraints

- **Compiler:** `g++ -std=c++11 -Wall -Wextra -Werror -g -c` each file, then link — zero warnings.
- **valgrind-clean (graded, 15 pts):** `valgrind --leak-check=full --show-leak-kinds=all ./db` → 0 errors, 0 bytes lost. Paste the summary in your README.
- **C-subset only.** `<cstdio>`, `<cstdlib>`, `<cstddef>`, raw arrays, `char *`, `struct` + free functions, pointers, `typedef`. **Banned:** classes, member functions, `std::string`, `std::vector`, `std::cout`, references, `new`/`delete`, templates, the STL, `auto`, range-`for`.
- **Makefile** with `all`, `test`, `clean`, real per-object rules.
- **AI-citation** comment block on any AI-assisted lines.

## Deliverables

| File | Contents |
|---|---|
| `db.h`, `db.cpp` | the ADT; `struct DB` defined only in the `.cpp` |
| `mystr.h`, `mystr.cpp` | your Lab 1 string helpers (or your `my_strdup` at minimum) |
| `main.cpp` | driver + tests, including the save/load round trip and the malformed file |
| `records.txt`, `bad_records.txt` | one valid data file, one deliberately malformed |
| `Makefile` | `all`, `test`, `clean`, separate compilation |
| `README.md` | your removal strategy and its cost; why names are deep-copied; why `DB_find` returns `const Record *`; the destroy order; the pasted valgrind summary |

## Rubric (100 pts → 1%)

| Criterion | Pts |
|---|---|
| ADT operations correct: `create`, `add` (incl. duplicate rejection), `find`, `remove` (first/middle/last/absent), `update_gpa`, `size`, `capacity`, `print` | 30 |
| **Opaque type & information hiding**: `struct DB` defined only in `db.cpp`; `main.cpp` never touches internals; `DB_find` returns `const Record *` | 15 |
| Growable array of structs: doubling policy, correct `sizeof(Record)` arithmetic, capacity reported correctly | 12 |
| Owned strings deep-copied on add, freed on remove **and** on destroy; correct destroy order | 10 |
| **Valgrind-clean** (0 errors, 0 definitely/indirectly lost) | 15 |
| Multi-file build: header guards, per-object `Makefile` rules, no `#include` of a `.cpp` | 10 |
| `DB_save`/`DB_load` round trip + clean failure on the malformed file | 3 |
| `README.md` with the required explanations + AI citation | 5 |

## AI note

Ask an assistant for a "student database in C++" and you get `class Database { std::vector<Student> students; };` — **classes and `std::vector` are banned this week**, and more to the point they would skip the entire lesson. The lesson is that you can get encapsulation *without* a class, using nothing but a header that declines to describe the struct. State the constraint in your prompt and re-check the output every few edits; assistants drift back to STL C++ almost immediately. If you don't know what a `class` is, good. You're not supposed to yet. 

Two subtler failure modes on this lab. First, Codex will happily put `struct DB { Record *items; size_t count; size_t cap; };` **in the header** because that is what most example code does — and the moment it is there, the opacity (and 15 points) is gone. Second, it consistently forgets that `DB_remove` must `free(items[i].name)` before shifting the array down; the resulting leak is invisible without valgrind, and the shift-down loop it writes is frequently off by one at the last element. Trace the removal of the last record by hand on a 3-element array before you run it.

Also expect it to reach for C++14/17 (`std::optional<Record>` for `DB_find`, structured bindings in the loop) — all banned. Cite AI-generated lines.
