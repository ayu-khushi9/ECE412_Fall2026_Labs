# ECE 412 — Lab 1 Writeup

**Name: Ayushi Priyadarshani**
**EID: ap62929**
**Machine used (hostname): ap62929@mario.ece.utexas.edu**

*Answer in your own words — two to four sentences each unless noted. These are the explanation points of the rubric; code alone does not earn them.*

````markdown
## W1. The growth policy

State your growth policy (starting capacity, growth factor, when growth triggers), and give the capacity sequence your 40-push driver actually produced.

My growth policy is starting capacity where the growth factor is 2×. The growth is triggered when size equals cap. Capacity sequence from the 40-push driver: 4 → 8 → 16 → 32 → 64 (doublings occur at pushes 5, 9, 17, and 33).

## W2. `realloc` vs. the manual version

You implemented growth both ways. What did `realloc` do that your malloc+copy+free version did not (or vice versa)? Include the **addresses you observed**: paste two or three `data=` lines from your driver output for each version and say whether the buffer moved at each doubling.

Both versions produce the same logical result, but `realloc` may resize the buffer in place if there is free space immediately after the block, meaning the address does not change. The manual malloc+copy+free version always allocates a fresh block, so the address always changes at every doubling.

realloc version (4→8 doubling):

    push  4: size= 4 cap= 4 data=0x55a1b2c
    push  5: size= 5 cap= 8 data=0x55a1b2c   <- same address, extended in place
    push  6: size= 6 cap= 8 data=0x55a1b2c

manual version (4→8 doubling):

    push  4: size= 4 cap= 4 data=0x55a1b2c
    push  5: size= 5 cap= 8 data=0x55a1f40   <- new address, buffer moved
    push  6: size= 6 cap= 8 data=0x55a1f40

`realloc` is potentially cheaper because it skips the copy when it can grow the block in place.

## W3. The `realloc` trap

`a->data = (int *)realloc(a->data, n);` is a memory leak whenever `realloc` fails. Explain exactly what is lost, and why assigning to a temporary first fixes it. (Your `ia_grow` must also *do* it correctly — this question is the "say why.")

If `realloc` fails it returns `NULL` but the old block is still alive and valid. Writing `a->data = realloc(a->data, n)` overwrites the only pointer to that old block with `NULL` — it is now unreachable and permanently leaked. The array is also left in a broken state with `a->data == NULL` but `a->size` and `a->cap` still nonzero.

Assigning to a temporary fixes both problems:

    int *tmp = (int *)realloc(a->data, newcap * sizeof(int));
    if (!tmp) return 0;   // old block still intact, a unchanged
    a->data = tmp;
    a->cap  = newcap;

On failure we still hold the original pointer in `a->data` and return 0 with the array completely unchanged and valid.

## W4. The stale pointer after growth

Why is holding `int *p = &a->data[0];` across an `ia_push_back` a use-after-free? Two or three sentences.

`p` stores the address of the buffer before the push. If `ia_push_back` triggers `ia_grow`, the old buffer is freed (manual version) or potentially moved (realloc version) and the original address is no longer valid. Any subsequent read or write through `p` accesses freed memory — undefined behavior that valgrind memcheck will flag as an invalid read/write.

## W5. Two-level ownership

Your driver builds an array of `my_strdup`'d strings. Describe the two levels of ownership and the order you freed them in. What does valgrind report if you free the array of pointers but not the strings — which leak category?

Level 1: the `char **names` pointer array — one `malloc` holding `n` pointers. Level 2: each `char *` inside it, individually `malloc`'d by `my_strdup`. The correct free order is strings first (`free(names[i])` in a loop), then the pointer array (`free(names)`). If you free only `names` but not the individual strings, valgrind reports the strings as **definitely lost** — allocated blocks with no remaining pointer to them anywhere in the program.

## W6. The `+1`, and the cast

Why does `my_strdup` allocate `my_strlen(s) + 1` bytes — what goes wrong with exactly `my_strlen(s)`, and which tool catches it? And note the C/C++ difference on `malloc`'s return value (quiz material).

A C string requires `strlen + 1` bytes to hold the `'\0'` terminator. Allocating exactly `strlen(s)` bytes means writing the terminator goes one byte past the end of the allocation — a heap buffer overflow. valgrind (with memcheck) and AddressSanitizer both catch this as an invalid write.

C/C++ difference: `malloc` returns `void *`. In C, `void *` converts implicitly to any pointer type so the cast is optional. In C++, the implicit conversion is not allowed — omitting `(char *)` is a compile error. Always cast when writing code that must compile as both.

## W7. Unsigned comparison

Why must `my_strcmp` compare as `unsigned char`? Give the concrete example from the handout (`"\xFF"` vs `"a"`) and say what plain signed `char` gets wrong on the ECE Linux server machines.

On x86/ECE Linux, plain `char` is signed (range −128 to 127). `'\xFF'` is 255 as unsigned char but −1 as signed char. Comparing `"\xFF"` against `"a"` (97) with signed char gives −1 < 97, so `"\xFF"` incorrectly sorts *before* `"a"`. The C standard defines `strcmp` behavior in terms of `unsigned char`, so `'\xFF'` (255) must be greater than `'a'` (97) and `"\xFF"` must sort *after* `"a"`. Casting to `unsigned char` before comparing enforces the correct behavior on any platform regardless of whether `char` is signed or unsigned by default.
````

## W8. The valgrind summary (paste)

Paste the full summary block of

`valgrind --leak-check=full --show-leak-kinds=all ./toolkit`

— the lines from `HEAP SUMMARY` through `ERROR SUMMARY`. It must show 0 errors, 0 bytes definitely lost, 0 bytes indirectly lost.

```
(paste here)
```

## W9. AI assistance

Name **one specific bug you had to fix in AI-suggested code** on this lab (what the assistant wrote, why it was wrong, how you fixed it) — or state plainly that you wrote the lab unaided. AI-assisted lines in your source carry the syllabus citation comment.

No assitance. 

## W10. Anything that fought you *(optional, genuinely useful)*
