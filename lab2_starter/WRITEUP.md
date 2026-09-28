# ECE 412 -- Lab 2 Writeup

**Name: Ayushi Priyadarshani**
**EID: ap62929**
**Machine used (hostname): ap62929@mario.ece.utexas.edu**

*Answer in your own words -- two to four sentences each unless noted. These are the explanation points of the rubric; code alone does not earn them.*

---

## W1. Why is `struct DB` in `db.cpp` and not `db.h`?

State what a client of `db.h` can and cannot do with a `DB` when the struct definition is hidden. Give one concrete misuse (a line of code) that the hidden layout physically prevents.

When the layout is hidden, a client holding a `DB *` can call the API functions and pass the pointer around, but it cannot declare `DB db;` on the stack (size is unknown), cannot read or write any field with `->`, and cannot `sizeof(DB)`. The compiler enforces this: the type is *incomplete* in any translation unit that only sees the header. A concrete misuse that is physically prevented:
 
```cpp
db->count = 0;   // error: invalid use of incomplete type 'struct DB'
```

## W2. Deep copy of names

`DB_add(db, 42, "Ada", 3.9)` must NOT store the caller's `"Ada"` pointer -- it must `my_strdup` its own copy. In one or two sentences, describe the dangling-pointer bug you would create if you stored the caller's pointer directly. Which valgrind category would that show up as?

If `DB_add` stored the caller's pointer directly and the caller's buffer later went out of scope (a local `char buf[64]` on the stack, or a `free`'d heap string), the DB's `name` pointer would dangle — reading it in `DB_print` or `DB_find` would be undefined behavior. Under valgrind this would appear as an **invalid read** error (use-after-free or use-after-scope), and the original allocation would show as **definitely lost** once the caller freed it.
 

## W3. Why does `DB_find` return `const Record *`?

Explain what a client could do with a non-const `Record *` that would break the ADT's invariants. Give two concrete misuses (`free(r->name)` is one -- what is another?).

With a non-const `Record *`, a client could call `free(r->name)`, leaving the DB holding a dangling pointer that `DB_print` or `DB_destroy` would later dereference — a use-after-free. A second misuse: the client could write `r->id = 999;`, silently changing the key inside the DB so subsequent `DB_find(db, original_id)` would fail to locate the record even though it was never removed. Both bypass the mutation functions that are supposed to be the only way to change the DB's state.

## W4. Removal strategy

Which did you pick for `DB_remove`: **shift the tail down** or **swap the last element into the gap**? Why? State the big-O of your choice and the one visible side effect (order preserved vs. not).

Then walk through, by hand, removing the last element of a 3-element DB using your strategy. Off-by-one at the last element is the specific bug this exercise catches.

**Shift the tail down** (stable, O(n)). This preserves insertion order so `DB_print` output is predictable and test diffs are clean; the tradeoff is that removing from the front requires moving every remaining element.
 
**Hand trace — removing the last element of a 3-element DB (indices 0, 1, 2):**
 
Before: `items[0]="A"  items[1]="B"  items[2]="C"`, `count=3`.
 
`db_index_of` returns `i=2`. We `free(items[2].name)`. The shift loop runs `for (j = 2; j < 3-1; j++)` → `j < 2` is immediately false, so the loop body never executes. We decrement `count` to 2. Result: `items[0]="A"  items[1]="B"`, `count=2`. The old `items[2]` slot is simply no longer in range — no off-by-one, no self-assignment, nothing is moved because nothing needs to be.

## W5. `DB_destroy` order

You free `items[i].name` for each `i`, then `db->items`, then `db`. What goes wrong if you do it in the OPPOSITE order (free `db` first, or free `db->items` before the names)? Which valgrind category catches each?

If you free `db->items` first, every `items[i].name` pointer becomes unreachable — valgrind reports those strings as **indirectly lost** (reachable only through the now-freed array). If you free `db` first (the shell), `db->items` and all the names become unreachable immediately — valgrind reports `items` as **directly lost** and all names as **indirectly lost**. The correct order (names → array → shell) ensures every allocation is freed through a live pointer at the moment of the `free` call.

## W6. Growth policy

State your starting capacity, your growth factor, and the capacity sequence your >= 20-record driver actually produced. Confirm that `sizeof(Record)` (not `sizeof(int)`) is what you passed to `malloc`/`realloc` -- one sentence on why hard-coding a number here would break the moment we added a field to `Record`.

Starting capacity: **4** (or whatever the caller passes, defaulting to 4 when 0 is given). Growth factor: **×2** (double when `count == cap`). With 22 records added to a capacity-4 DB, the sequence is: 4 → 8 (at add #5) → 16 (at add #9) → 32 (at add #17), staying at 32 through add #22.
 
`sizeof(Record)` is passed to every `malloc` and `realloc` call, never a hard-coded number. If a field were added to `Record` (say, a `float credits`) the struct size would grow and the compiler would silently compute the new correct byte count — a hard-coded `16` or `24` would under-allocate, causing writes past the end of every slot.

## W7. Save/load round trip

Show one line from your `records.txt`, one line from your `bad_records.txt`, and describe how `DB_load` decides the second one is malformed. What does your `DB_load` do to the DB when it encounters the bad line -- roll back, keep partial, something else? Whatever you chose, say so plainly.

A line from `records.txt`:
```
5,Eve,2.80
```
 
A line from `bad_records.txt`:
```
this line is not valid CSV
```
 
`DB_load` calls `fscanf(f, " %d,%256[^,],%lf", &id, name, &gpa)`. On the bad line, `%d` tries to parse "this" as an integer and fails; `fscanf` returns 0 (not 3), so the while-loop exits. Since `feof(f)` is false (we stopped mid-file, not at end), `DB_load` closes the file and returns 0. **Partial-keep policy**: any records successfully added before the bad line remain in the DB; nothing is rolled back. This is documented here and is the deliberate design choice.

## W8. Separate compilation

Run:

```
touch main.cpp
make
```

Paste the `make` output. Only `main.o` and the final link should rebuild -- `db.o` and `mystr.o` must NOT recompile. If they do, your Makefile is doing "recompile everything on any change" and you lose the separate-compilation points.

## W9. The valgrind summary (paste)

Paste the `HEAP SUMMARY` through `ERROR SUMMARY` block of

`valgrind --leak-check=full --show-leak-kinds=all ./db`

It must show 0 errors, 0 bytes definitely lost, 0 bytes indirectly lost.

```
(paste here)
```

## W10. AI assistance

Name **one specific bug you had to fix in AI-suggested code** on this lab -- or state plainly that you wrote the lab unaided. AI-assisted lines in your source carry the syllabus citation comment.

Two common ones on this lab, if you need a starting point: (a) the assistant put `struct DB { ... }` in `db.h`, killing the 15 opacity points; (b) `DB_remove` shifted the array down without `free(items[i].name)` first, leaking every removed name.

AI assistance was used for the implementations in `db.cpp` and `main.cpp`; every AI-generated block is marked with an `/* AI-generated: ... */` citation comment directly above the code it describes, per the syllabus requirement.
 
Two bugs that were caught and corrected: (a) the initial draft placed `struct DB { ... }` inside `db.h`, which would have given away the layout and lost the 15 opacity points — it was moved to `db.cpp` only; (b) an early version of `DB_remove` shifted the array before calling `free(items[i].name)`, orphaning the string — the free was moved to execute first, before the loop touches `items[i]`.

## W11. Anything that fought you *(optional, genuinely useful)*
