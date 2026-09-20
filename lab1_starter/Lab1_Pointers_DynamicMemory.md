# Lab 1 — Pointers & Dynamic Memory: Build Your Own Vector 

| | |
|---|---|
| **Assigned** | Tue Sep 1 |
| **Due** | **Sun Sep 13, 11:59 pm** (Gradescope) |
| **Weight** | 1% |
| **Builds on** | L5 (pointers & the memory model, Sep 8), L6 (dynamic memory: `malloc`/`free`, leaks, valgrind, Sep 10) |
| **Language** | **C-compatible subset of C++11** |
| **Work** | Individual |
| **Oral checkout** | — |

> **Timing note.** L6 — `malloc`/`free`/valgrind — is **Thu Sep 10**, three days before this is due. Read the assignment on Tuesday, write the string utilities (Part A) first, and start Part B after Thursday's lecture. The turnaround is short here on purpose: pointers and `malloc` are 319K material and this lab is a refresher on tools you already have.

## Goal

This is the **memory-discipline gate** of the course. Everything after it — the linked list, the hash table, the BST, the destructor, RAII, the capstone — assumes you can allocate, resize, and release heap memory without leaking or corrupting it. So you build the two things every higher-level language hides from you:

1. a **growable array** that doubles its capacity when full, out of nothing but `malloc`, `realloc`, and `free`
2. the **C string functions**, from scratch, so you know what a `char*` actually is and why "just copy the string" is three lines and a `free`.

You will prove it is right with **valgrind**. From this lab forward, valgrind-clean is graded on every lab.

> **This is the first graded lab.** Lab 0 was the pipe-cleaner; if anything about cloning, building on `mario`, or submitting to Gradescope is still shaky, sort it out now rather than at 11 pm on Sep 13.

## The assignment

### Part A — string utilities, no `<cstring>`

In `mystr.h` / `mystr.cpp`, implement, **without including `<cstring>`** and without calling any library string function:

```cpp
size_t my_strlen(const char *s);
                /* number of chars before the '\0'; my_strlen("") == 0 */

char  *my_strdup(const char *s);
                /* malloc a new buffer of the right size, copy s into it
                   INCLUDING the terminator, return it. Returns NULL if
                   malloc fails. The CALLER owns the result and must free it. */

int    my_strcmp(const char *a, const char *b);
                /* <0 if a sorts before b, 0 if equal, >0 if after -- the
                   strcmp contract: compare unsigned char values at the first
                   position where they differ. */

int    my_strcpy_safe(char *dst, size_t dstsize, const char *src);
                /* copy src into dst, never writing more than dstsize bytes
                   (terminator included). Returns 1 on success, 0 if src did
                   not fit (in which case dst is left as a valid empty string). */
```

Points of substance, each worth explaining in your WRITEUP:

- `my_strdup` must allocate `my_strlen(s) + 1` bytes. The `+ 1` is the `'\0'`. Off-by-one here is a heap overflow, and valgrind will tell you about it in a way `-Wall` never will.
- In C++, `malloc` returns `void*` and **must be cast**: `char *p = (char *)malloc(n);`. In C the cast is optional; in C++ it is required. Note this once in your WRITEUP — it is a real difference between the languages and it will likely be on a quiz.
- `my_strcmp` compares as **`unsigned char`**, not `char`. On `mario`, plain `char` is signed, so comparing `"\xFF"` against `"a"` gives the wrong sign if you get this wrong.

### Part B — `IntArray`: the growable array

In `intarray.h` / `intarray.cpp`:

```cpp
typedef struct {
    int    *data;   /* owning pointer, from malloc/realloc; NULL when cap == 0 */
    size_t  size;   /* elements in use          */
    size_t  cap;    /* elements allocated       */
} IntArray;

void   ia_init(IntArray *a);                  /* size = cap = 0, data = NULL   */
int    ia_push_back(IntArray *a, int v);      /* grows if full; 1 ok, 0 on OOM */
int    ia_get(const IntArray *a, size_t i);   /* precondition: i < a->size     */
int    ia_set(IntArray *a, size_t i, int v);  /* 1 ok, 0 if i out of range     */
int    ia_pop_back(IntArray *a, int *out);    /* 1 ok, 0 if empty              */
size_t ia_size(const IntArray *a);
size_t ia_capacity(const IntArray *a);
void   ia_clear(IntArray *a);                 /* size = 0, keeps the buffer    */
void   ia_destroy(IntArray *a);               /* free(data); data = NULL;
                                                 size = cap = 0                */
```

**The growth policy.** Start at `cap == 0` with `data == NULL`. On the first `ia_push_back`, allocate capacity 4. Whenever `size == cap`, grow to `2 * cap`. Implement growth **both ways** and keep both in your submission:

```cpp
/* Version 1 -- malloc + copy + free, so you see every step */
int *bigger = (int *)malloc(newcap * sizeof(int));
 /* YOU FILL IN THE REST IN YOUR CODE */

/* Version 2 -- using realloc */
int *bigger = (int *)realloc(a->data, newcap * sizeof(int));

 /* YOU FILL IN THE REST IN YOUR CODE */ 
```

Select between them with a `#define GROW_WITH_REALLOC 1` (or a `Makefile` variable) and say in your WRITEUP what `realloc` did that the manual version did not — in particular, whether the pointer address changed, which you can check by printing `(void *)a->data` before and after.

**The `realloc` trap, graded:** `a->data = (int *)realloc(a->data, n);` is a memory leak whenever `realloc` fails, because you have overwritten the only pointer to the still-allocated old block. Assign to a temporary first, check it, then commit. Your code must do it correctly and your WRITEUP must say why.

### Part C — a driver

`main.cpp` exercises Parts A and B:

- Push at least **40** values so the array grows through **at least three** doublings (4 → 8 → 16 → 32 → 64), printing `size`, `cap`, and `(void *)data` after each push so the doubling and the possible address change are visible.
- Build an array of `my_strdup`'d names, sort it with `my_strcmp` (an insertion sort you write is fine), print it, and **free every string plus the array of pointers**. Forgetting the individual strings while freeing the array is the classic "I freed it, why does valgrind still complain" bug — do it right, and describe the two-level ownership in your WRITEUP.
- Exercise every failure path you can reach: `ia_pop_back` on empty, `ia_get`/`ia_set` out of range, `ia_destroy` called twice, `my_strcpy_safe` with a destination that is too small, etc. Get creative!  Your users certainly will.

## Requirements

- **Every allocation is NULL-checked.** `malloc`/`realloc` returning `NULL` must be handled, never dereferenced. On failure, leave the structure in a valid state and return a failure code — do not `exit()` from inside a library function.
- **Exactly one owner per allocation.** After `ia_destroy`, `data == NULL` and `size == cap == 0`. Calling `ia_destroy` twice must not crash (this is why you null it out).
- **`free` pairs with `malloc`/`realloc`/`calloc`.** No `delete`, no `new` — those are L10 and they are not yours yet.
- **No stale pointers across a growth.** After `ia_push_back` causes a reallocation, any `int *` you were holding into the old buffer is dangling. Your WRITEUP must explain, in two or three sentences, why holding `int *p = &a->data[0];` across a `push_back` is a use-after-free — **this is the same hazard `std::vector` has, and it is the reason iterator invalidation exists.**
- Read-only parameters are `const`-qualified pointers (`const IntArray *`, `const char *`). If a `const` slipped off because the code would not compile with it, that is a bug, not a style nit.
- Header guards in both headers. No `using namespace std;` anywhere (there is no `std` in this lab).

## Constraints

- **Compiler:** `g++ -std=c++11 -Wall -Wextra -Werror -g mystr.cpp intarray.cpp main.cpp -o toolkit` — zero warnings.
- **valgrind-clean (graded, 20 pts):**
  `valgrind --leak-check=full --show-leak-kinds=all ./toolkit`
  → **0 errors**, **0 bytes definitely lost**, **0 bytes indirectly lost**. Paste the summary in your WRITEUP.
- **C-subset only.** Banned: `std::string`, `std::vector`, `std::cout`, classes, member functions, references (`T&`), `new`/`delete`, templates, the STL, `auto`, range-`for`, **and `<cstring>`.  If you are in doubt about whether you can use something or not, post to Ed discussion instead of potentially losing points! **.
- **Makefile** with `all`, `test`, `clean`.
- **AI-citation** comment block on any AI-assisted lines.

## Deliverables

| File | Contents |
|---|---|
| `mystr.h`, `mystr.cpp` | Part A |
| `intarray.h`, `intarray.cpp` | Part B, both growth versions |
| `main.cpp` | Part C driver + your tests |
| `Makefile` | `all`, `test`, `clean` |
| `WRITEUP.md` | growth policy; what `realloc` did vs. manual copy (with the addresses you observed); the `realloc`-failure leak explained; the stale-pointer-after-growth explanation; the two-level `char**` ownership; **the pasted valgrind summary**; one specific bug you had to fix in AI-suggested code (or a statement that you wrote it unaided) |

## Rubric (100 pts → 1%)

| Criterion | Pts |
|---|---|
| Part A: `my_strlen`, `my_strdup`, `my_strcmp`, `my_strcpy_safe` correct (incl. the `+1` for the terminator and unsigned comparison), no `<cstring>` | 18 |
| Part B: `IntArray` correct — doubling growth, both growth versions present, correct copy-and-release, `size`/`cap` bookkeeping, `pop_back`, `clear` vs `destroy` | 28 |
| **Every allocation NULL-checked**, failure paths leave a valid structure, no `realloc`-overwrite leak | 8 |
| **Valgrind-clean**: 0 errors, 0 bytes lost, no invalid read/write, no double free | 20 |
| Ownership hygiene: `ia_destroy` idempotent and nulls out, every `my_strdup` freed exactly once, no stale pointers used after growth | 9 |
| **C-subset discipline** (no `std::string`/`vector`/`cout`/classes/references/`new`/`<cstring>`) | 7 |
| Compiles clean + `Makefile` + `WRITEUP.md` with the required explanations and valgrind output + AI citation | 10 |

> **Automatic deduction:** using `std::vector`, `std::string`, or any library container for Part A or B is **−30**, however well it works. Writing the thing yourself *is* the assignment.

## AI note

Memory bugs are where AI-written C is most often **plausible but wrong**, because the wrong version compiles clean and usually runs. Specifically, on this lab, expect:

- It will "helpfully" rewrite your growable array as `std::vector<int>` and your strings as `std::string`. **Both are banned here.** Say so in the prompt and check the output anyway.
- `a->data = realloc(a->data, n)` — the classic leak-on-failure, which it writes because that is what most tutorial code on the internet does.
- `malloc(my_strlen(s))` in `my_strdup`, forgetting the `+ 1`. It runs fine most of the time and corrupts the heap the rest of the time.
- Freeing the `char**` array without freeing the individual strings, or double-freeing after a `strdup` that was also freed by a helper.
- Forgetting the `(char *)` / `(int *)` cast on `malloc`, which is legal C and a **compile error in C++**. If Codex hands you C, this is the first thing that breaks.

The workflow that works: write it yourself, then ask Codex *"find the memory errors in this file"* rather than *"write this file."* Run **valgrind on anything it produces** before you believe it. Name in your WRITEUP one specific bug you had to correct in AI-suggested code — or state plainly that you wrote it unaided. Cite AI-generated lines.
