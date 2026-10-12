# ECE 412 -- Lab 3: Linked List in the C Subset

| | |
|---|---|
| **Assigned** | Tue Sep 22 |
| **Due** | **Mon Sep 28, 11:59 pm** (Gradescope) |
| **Weight** | 1% |
| **Builds on** | L8 (linked structures in the C subset) -- with L9 (from structs to classes) running alongside as the contrast |
| **Language** | **C-compatible subset of C++11** -- the last procedural lab |
| **Work** | Individual |

# Lab 3 -- <Ayushi Priyadarshani>, <ap62929>

## Removal strategy

`List_remove` uses the `Node **pp` idiom. It also keeps a trailing `Node *prev`, which is used only to fix `tail`.

`pp` always holds the address of *the pointer that points at the current node*. It starts as `&l->head`. After each step it becomes `&(*pp)->next`, the address of the `next` field inside the node before the current one.

Below is the list just before the middle node (102) is unlinked:

```
    l->head
       |
       v
   +--------+     +--------+     +--------+
   | 101    |     | 102    |     |   7    |
   | next --|---->| next --|---->| next 0 |
   +--------+     +--------+     +--------+
    ^     ^           ^               ^
    |     |           |               |
  prev    pp       *pp == victim   l->tail

  pp   == &(node 101).next   -- the ADDRESS of 101's next field
  *pp  == node 102           -- the node being removed
  prev == node 101
```

Unlinking takes one statement: `*pp = victim->next;`. That writes 7's address into 101's `next` field.

- **Empty list:** the loop never runs because `*pp` is NULL right away, so the function returns 0. No special case is needed. An absent sku ends the same way.
- **Head:** `pp == &l->head`, so `*pp = victim->next` writes into `l->head` itself.
- **Middle:** `pp == &prev->next`, so the same statement writes into the predecessor's `next` field. The head and middle cases are the same code.
- **Tail:** this is the one case that still needs its own line, `if (l->tail == victim) l->tail = prev;`. If the victim was also the only node, `prev` is still NULL, so `tail` correctly becomes NULL too.

## Why `next` must be saved before the `free`

After `free(cur)`, the memory `cur` points to belongs to the allocator again, so `cur->next` is a read of freed memory. That is undefined behavior, not a valid pointer. The program usually still prints the right answer because nothing has reused that block yet, so the old bytes are still there. Valgrind reports this as an **Invalid read of size 8** inside a block that was already freed. For that reason, `free_chain` copies `cur->next` into a local `next` before it frees the name or the node.

## What a destructor would buy me

In `main.cpp`, `make_three()` makes a list and inserts three items. If any insert fails, it has to call `List_destroy(l)` before it returns NULL. If I forgot that call on that early return, the `List` handle and every node and name already inserted would leak. Valgrind would report the handle as definitely lost and the chain as indirectly lost. Every test also has to end with `List_destroy`, and the compiler never reminds me. With next week's destructor, the list would be a local object. Any `return`, early or not, would free it automatically, so that `List_destroy` call and the bug that comes from forgetting it would go away. One thing would stay manual: the `char *` handed out by `List_remove_front` is still the caller's to `free`. A destructor only cleans up what the object still owns.

## valgrind

```
==498== HEAP SUMMARY:
==498==     in use at exit: 0 bytes in 0 blocks
==498==   total heap usage: 2,090 allocs, 2,090 frees, 45,695 bytes allocated
==498==
==498== All heap blocks were freed -- no leaks are possible
==498==
==498== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## AI citation

AI assistance was used for the implementations in `list.cpp` and `main.cpp`; every AI-generated block is marked with an `/* AI-generated: ... */` citation comment directly above the code it describes, per the syllabus requirement.

> **Why this one is still procedural.** Classes arrive in **L9**, the day this is assigned. We are keeping Lab 3 in the C subset on purpose. **The next lab rebuilds this exact structure as a class with a destructor**, and the whole point is that you feel the difference: this week *you* must remember to call `List_destroy`, and every path out of every function is a path you can leak on. Next week the compiler remembers for you. Do not skip ahead -- the contrast is the lesson.

## Goal

Pointer work at full strength. A singly linked list is the first structure whose *shape is made of pointers*: there is no index arithmetic to fall back on, only "the node that points to the node I want." You will insert at both ends, search, remove from anywhere, traverse, and release the whole thing -- and you will draw the box-and-arrow diagram for each operation before you write it, because that is what the midterm asks for on paper.

## Objectives

By the time you submit you should be able to do all of these without looking anything up:

- allocate a node, deep-copy a string into it, and link it into a chain
- keep a `tail` pointer and a `count` correct through every transition, including the empty-to-one and one-to-empty transitions that everybody gets wrong
- remove a node from the head, the middle, or the end of a chain, and say out loud which pointer you had to have kept in order to do it
- free a chain in an order that never reads memory you have already released
- hide a private struct behind an opaque handle, and give clients iteration without handing them a `Node *`
- read a valgrind report, including the difference between *definitely lost* and *indirectly lost*

## Background: the shape

A list is three pointers and a promise. `head` points at the first node or is `NULL`. Each node points at the next one, and the last points at `NULL`. `tail` points at the last node so that appending does not have to walk. `count` records the length so that asking for the length does not have to walk either.

```
      +--------+                                        List
      | head  ---------+                                (the handle)
      | tail  --------------------------------+
      | count  |  3     |                     |
      +--------+        |                     |
                        v                     v
                 +-----------+   +-----------+   +-----------+
                 | sku  101  |   | sku  102  |   | sku    7  |
                 | name -----|-->| name -----|-->| name -----|--> "cam"
                 | qty    4  |   | qty    9  |   | qty    0  |
                 | next   ---|-->| next   ---|-->| next  0   |
                 +-----------+   +-----------+   +-----------+
```

Two things in that picture are worth saying explicitly. The `name` field is a separate allocation, which means every node costs two `malloc`s and two `free`s. And `next` on the last node is `NULL`, not garbage -- which is only true if you set it.

The two transitions to draw before you write any code:

```
  inserting into an EMPTY list          head -> NULL,  tail -> NULL
    after:   head -> [new] -> NULL,     tail -> [new]
             ^^^^ both pointers change; forgetting tail is the bug

  removing the LAST remaining node      head -> [n] -> NULL, tail -> [n]
    after:   head -> NULL,              tail -> NULL
             ^^^^ if tail still points at [n], it points at freed memory
```

What is in the repository:

| File | What you do with it |
|---|---|
| `list.h` | **Do not modify.** The interface, with the contracts spelled out. |
| `list.cpp` | Your implementation. Every function is stubbed with a TODO. |
| `mystr.h` | **Do not modify.** |
| `mystr.cpp` | Bring your `my_strdup` from Lab 2 over. |
| `main.cpp` | Your tests. Three are written for you; the rest are yours. |
| `Makefile` | Complete, with per-object rules. Read it -- you will write the next one. |
| `tests/public_tests.cpp` | A provided suite. Do not edit it, do not submit it. |
| `README_TEMPLATE.md` | Copy over this file to write your report. |

`list.h` is marked do-not-modify for a concrete reason: the autograder compiles its own test programs against that interface. Change a signature and nothing links.

## The assignment

A list of inventory items, with the same opaque-ADT discipline as Lab 2: `Item` is public, `List` and `Node` are not.

### What goes where

`struct Node` and `struct List` are defined **only** in `list.cpp`:

```cpp
struct Node {
    Item  item;
    Node *next;
};

struct List {
    Node   *head;
    Node   *tail;   /* required: insert_back must be constant time */
    size_t  count;  /* required: List_size must be constant time   */
};
```

A client that includes `list.h` cannot see either one, cannot take `sizeof(List)`, and cannot name `Node`. The autograder checks this by trying to compile exactly those two things and requiring that both fail.

### The operations, and what is actually being graded

- **`List_insert_front`** -- allocate the node, deep-copy the name, link it in, fix `tail` **if the list was empty**. That last clause is the bug.
- **`List_insert_back`** -- constant time via `tail`: a fixed number of steps no matter how long the list is. Do not walk. On an empty list, `head` and `tail` both become the new node.
- **`List_find`** -- walk, compare, return `&node->item` or `NULL`. Takes `const List *` and returns `const Item *`; both matter.
- **`List_remove`** -- you must handle **four** cases and test all four:
  1. the list is empty, so return 0;
  2. the match is the **head**, so `head` moves, and `tail` must be fixed if the list is now empty;
  3. the match is in the **middle**, so the predecessor's `next` is rerouted;
  4. the match is the **tail**, so the predecessor becomes the new `tail`.

  You cannot reroute a predecessor you did not keep. Either walk with a `Node *prev` trailing pointer, or use the `Node **pp` "pointer to the pointer that points at me" idiom:
  ```cpp
  Node **pp = &l->head;
  while (*pp != NULL && (*pp)->item.sku != sku)
      pp = &(*pp)->next;
  ```
  The second version collapses the head and middle cases into one statement. **Whichever you use, explain it in your README** -- the `Node **` version is a favorite exam question, and if you use it you should be able to draw what `pp` points at.
- **`List_remove_front`** -- the item comes out, and **the caller now owns `out->name`**. The list must not free that string; the caller must. Document that in your README, and get it right: freeing it in both places is a double free, freeing it in neither is a leak.
- **`List_destroy` / `List_clear`** -- walk the list, and for each node: **save `next` first**, then free the name, then free the node. Freeing the node and *then* reading `n->next` is a use-after-free; valgrind reports it as an invalid read and your program will often still "work," which is the worst possible outcome. Write the loop as:
  ```cpp
  Node *cur = l->head;
  while (cur != NULL) {
      Node *next = cur->next;   /* SAVE FIRST */
      free(cur->item.name);
      free(cur);
      cur = next;
  }
  ```
  `List_clear` leaves the `List` itself alive and genuinely reusable: `head`, `tail` and `count` all back to empty. A stale `tail` here means the next `insert_back` writes through a freed pointer.
- **`List_size`** -- return the maintained `count`. The autograder times this function on a 200,000-node list; a version that walks does not finish fast enough to pass.
- **`List_for_each`** -- the client visits every item without ever seeing a `Node`. This is how you get iteration out of an opaque type in C, and it is worth understanding: the callback plus a `void *ctx` is the C ancestor of both C++ iterators and lambdas.

### `List_print`, exactly

The autograder compares this output, so match the format:

```
empty list:   [ ]
one item:     [ (101,"bolt",4) ]
three items:  [ (101,"bolt",4) -> (102,"nut",9) -> (7,"cam",0) ]
```

`"[ "`, then the items separated by `" -> "`, then `" ]"`, then one newline. Use `printf`. Runs of whitespace are compared loosely, so an extra space will not fail you, but the punctuation must be right.

### `main.cpp`

Your tests, covering at minimum: empty-list operations (find, remove, remove_front, print, size); a single-element list (remove it, then `insert_back` again -- **a stale `tail` here is the classic bug**); insert front and back interleaved; removal at head, middle, and tail; removal of an absent sku; a >= 1000-element list built and destroyed; and one `List_for_each` use that sums quantities through the `ctx` pointer.

`main.cpp` is a client. It must not contain the token `->next`, must not name `Node`, and must not `#include "list.cpp"`.

## Requirements

- `List_size` is **constant time** -- maintain `count`, do not walk.
- `List_insert_back` is **constant time** -- maintain `tail`.
- **Names are deep-copied on insert and freed on removal and on destroy.** Missing the `free` on the removal path is the most common leak in this lab.
- **`Node` never escapes.**
- Every `malloc` NULL-checked; a failed allocation must not corrupt or half-link the list.
- Read-only functions take `const List *`. `List_find` returns `const Item *`.
- Header guards; multi-file `Makefile` with per-object rules.
- **In your `README.md`, one short paragraph titled "What a destructor would buy me."** You have written `List_destroy` and you have had to call it on every exit path from `main`. Name one place in your own code where forgetting it would have leaked, and say what you expect next week's destructor to change. (You are previewing the RAII argument; say it in your own words.)

## Constraints

- **Compiler:** `g++ -std=c++11 -Wall -Wextra -Werror -g` per file, then link -- zero warnings.
- **valgrind-clean (graded, 15 pts):** `valgrind --leak-check=full --show-leak-kinds=all ./inventory` gives 0 errors and 0 bytes definitely or indirectly lost. Paste the summary in your README. A leaked chain of nodes shows up as *indirectly lost* -- learn to read that line, you will see it again.
- **Makefile** with `all`, `test`, `clean`.
- **AI-citation** comment block on any AI-assisted lines.
- **C subset only.** The banned list, in full:

| Banned | Use instead |
|---|---|
| `class`, member functions, constructors, destructors | a `struct` of data, plus free functions |
| `new`, `delete` | `malloc`, `free` |
| `std::string` | `char *` and your `my_strdup` |
| `std::vector`, `std::list`, anything else from the STL | the structure you are building |
| `std::cout`, `<iostream>` | `printf` |
| references (`int &x`) | pointers |
| templates, `auto`, range-`for`, lambdas, `nullptr` | write the type out; use `NULL` |
| `static_cast<>` and friends | a C cast |
| POSIX `strdup` | your own `my_strdup` |

The autograder scans your code for every item in that table. It blanks out comments and string literals before scanning, so the word "class" in a comment is fine. **A banned construct does not just cost you the 5 style points: it scales your functional score, because these constructs remove the thing the lab is teaching.** `std::string` deletes the entire deep-copy lesson, and a destructor is next week's assignment.

## Common mistakes

In rough order of how often they show up:

1. `insert_front` on an empty list does not set `tail`. Nothing breaks until the next `insert_back`, which links onto a node that `head` does not know about.
2. `remove` of the last element leaves `tail` pointing at the freed node. The next `insert_back` writes through a dangling pointer, and it usually appears to work.
3. `free(node)` without `free(node->item.name)` on the removal path. Destroy is usually right; removal is usually wrong.
4. `free(cur); cur = cur->next;` -- reading a freed node. Prints the right answer on most runs. This is why valgrind is graded rather than program output.
5. `List_clear` resets `head` and `count` but not `tail`.
6. `List_remove_front` frees the name it just handed to the caller, or the caller frees a name the list still owns. Decide who owns it, write it down, and be consistent.
7. `my_strdup` allocates `strlen(s)` bytes instead of `strlen(s) + 1`. Valgrind calls this an invalid write of size 1.
8. `List_size` walks the list. Correct, and still wrong.

## Testing

```
make test       # your tests (main.cpp) -> ./inventory
make check      # the provided suite  -> ./public_tests
make valgrind   # ./inventory under valgrind, the way we grade it
make vcheck     # the provided suite under valgrind
```

The provided suite in `tests/` runs the same kinds of checks the autograder runs, in the same C subset you are restricted to. Passing it is necessary, not sufficient: it does not check the constant-time requirements, it does not check your Makefile or README, and the autograder runs several sequences that are not in it. Your own `main.cpp` tests are graded separately and are worth more than the provided suite is.

A test that passes without valgrind and fails under it has still failed.

## Deliverables

Submit these files to Gradescope. Submit the files themselves -- not a folder of your whole home directory or a folder with the files inside:wq.

| File | Contents |
|---|---|
| `list.h`, `list.cpp` | the ADT; `struct Node` and `struct List` defined only in the `.cpp` |
| `mystr.h`, `mystr.cpp` | your `my_strdup` from Lab 2 |
| `main.cpp` | tests covering every case above |
| `Makefile` | `all`, `test`, `clean` |
| `README.md` | your removal strategy (`prev` vs `Node **`) with a box-and-arrow sketch in ASCII; why `next` must be saved before `free`; the **"What a destructor would buy me"** paragraph; the pasted valgrind summary |

Do not submit `tests/`, object files, or `./inventory`.

## Rubric (100 pts -> 1%)

| Criterion | Pts |
|---|---|
| `List_insert_front` / `List_insert_back` correct, both constant time, `tail` maintained through the empty-list transitions | 20 |
| `List_find`, `List_size` (constant time), `List_empty`, `List_print`, `List_for_each` correct | 10 |
| `List_remove` correct in **all four** cases (empty / head / middle / tail) + `List_remove_front` with documented ownership of the returned name | 25 |
| `List_destroy` / `List_clear` free every node **and** every name, saving `next` before freeing | 12 |
| **Valgrind-clean** (0 errors, 0 definitely/indirectly lost) | 15 |
| Opaque ADT: `Node` and `List` invisible in the header; no `->next` in `main.cpp`; `const` on observers | 8 |
| Compiles clean + multi-file `Makefile` + C-subset discipline | 5 |
| `README.md` (incl. the "What a destructor would buy me" paragraph) + AI citation | 5 |

95 of those points are awarded by the autograder, which you may run as often as you like before the deadline. The last 5 are read by a human.

## Checklist before you submit

- [ ] `make` produces zero warnings with `-Wall -Wextra -Werror`
- [ ] `make check` passes every provided test
- [ ] `make valgrind` reports 0 errors and 0 bytes lost, and so does `make vcheck`
- [ ] `main.cpp` covers all four removal cases, and the single-element list, by name
- [ ] `main.cpp` contains no `->next`, no `Node`, and no `#include "list.cpp"`
- [ ] `list.h` is unmodified
- [ ] nothing from the banned table appears in your code
- [ ] `List_size` and `List_insert_back` do not walk the list
- [ ] every `malloc` is NULL-checked, and a failure leaves the list intact
- [ ] `README.md` has the removal-strategy sketch, the save-`next`-first explanation, the "What a destructor would buy me" paragraph, the valgrind summary, and your AI citation

## AI note

Asked for a linked list "in C++," an assistant writes `class LinkedList` with `std::string name` and `new Node` -- **all three are banned this week**, and the `std::string` in particular would delete the entire deep-copy lesson. Constrain the prompt (*"C subset, `malloc`/`free`, `char *`, opaque struct, no classes, no STL"*) and audit the output; it drifts back within a couple of turns.

Where AI is genuinely, subtly wrong on linked lists:

- **It forgets `tail`.** Almost every generated `remove` updates `head` correctly and leaves `tail` dangling at a freed node. Your `insert_back` after `remove`-the-last-element then writes through a freed pointer. Test that sequence explicitly.
- **It frees before it reads.** `free(cur); cur = cur->next;` appears constantly. It usually still prints the right answer, because the freed memory has not been reused yet -- which is exactly why you must run valgrind rather than trust the output.
- **It leaks the payload.** `free(node)` without `free(node->item.name)`.
- **It reaches for C++14/17** (`std::optional` returns, structured bindings) and for `virtual` if you let it near a "generic" list.

Best use of an assistant on this lab: draw your box-and-arrow diagram, write the code, then ask it *"which of the four removal cases does this get wrong, and where does `tail` become stale?"* Verify its claims by hand on a 3-node list before you change anything. Cite AI-generated lines.
