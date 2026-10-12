# Lab 3 -- <Ayushi Priyadarshani>, <ap62929>

Copy this file over `README.md` and fill it in. Four sections are graded (5 pts); keep each one short and specific. Delete the bracketed instructions as you go.

## Removal strategy

[Say which one you used: a trailing `Node *prev`, or the `Node **pp` idiom. Then draw the state of a 3-node list at the moment just before you unlink the middle node, with an arrow showing what your pointer variable points AT. ASCII is fine and ASCII is expected -- something like the sketch below, but for your own code, and with your variable in it.]

```
        head
         |
         v
     +-------+     +-------+     +-------+
     | 101   |     | 102   |     |   7   |
     | next -|---->| next -|---->| next 0|
     +-------+     +-------+     +-------+
                       ^
                      ???   <-- put your prev or pp here, and say
                                exactly which pointer it holds the
                                address of
```

[Then: which of the four cases (empty / head / middle / tail) does your single loop handle without a special case, and which ones still need one? If you used `Node **`, say what `*pp = victim->next` writes to in the head case versus the middle case.]

## Why `next` must be saved before the `free`

[Two or three sentences. Say what `cur->next` is after `free(cur)`, why the program usually still prints the right answer anyway, and what valgrind calls it.]

## What a destructor would buy me

[One short paragraph, required. You wrote `List_destroy` and then had to call it on every exit path from `main`. Name one specific place in your own code -- a function and an early return, or a failed-allocation path -- where forgetting the call would have leaked. Then say what you expect next week's destructor to change about that specific place. Your own words.]

## valgrind

[Paste the summary from `valgrind --leak-check=full --show-leak-kinds=all ./inventory`. The HEAP SUMMARY and ERROR SUMMARY lines are enough.]

```
```

## AI citation

[Required, even if the answer is "none". For any line or block you got from an assistant: which lines, which tool, what you asked, and what you changed after checking it. If you used one only to review code you had already written, say that -- it counts and it is a good use of it.]
