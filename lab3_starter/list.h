/* ------------------------------------------------------------------ *
 * list.h -- ECE 412 Lab 3: singly linked list of inventory items,
 *           written in the C-compatible subset of C++11.
 *
 * DO NOT MODIFY THIS FILE.  The autograder compiles its own test
 * programs against these exact declarations.  If you change a
 * signature, remove a function, or move `struct Node` in here, the
 * grading harness will not link against your code.
 * ------------------------------------------------------------------ */
#ifndef LIST_H
#define LIST_H

#include <cstddef>      /* size_t */

/* Public value type: what a client puts in and gets out. */
typedef struct {
    int    sku;      /* unique key, >= 0                  */
    char  *name;     /* owned by the list (deep copy)     */
    int    qty;
} Item;

/* Opaque. Clients hold a List *, and that is all they get.
   `struct Node` does not appear in this header at all. */
typedef struct List List;

/* ---- lifetime ---------------------------------------------------- */

/* Allocate an empty list.  Returns NULL on allocation failure. */
List  *List_create(void);

/* Free EVERY node and EVERY name, then the list itself.
   List_destroy(NULL) is a no-op. */
void   List_destroy(List *l);

/* ---- insertion --------------------------------------------------- */

/* Insert a new item at the front / back.  `name` is DEEP COPIED: the
   list must not retain the caller's pointer, and the caller may
   modify or free its own buffer immediately afterwards.
   Returns 1 on success, 0 on allocation failure.  On failure the
   list must be left exactly as it was -- no half-linked node, no
   leaked name, no bumped count. */
int    List_insert_front(List *l, int sku, const char *name, int qty);
int    List_insert_back (List *l, int sku, const char *name, int qty);

/* ---- lookup ------------------------------------------------------ */

/* Return a pointer to the stored item with this sku, or NULL if no
   such item.  The pointer is owned by the list and is valid only
   until that item is removed. */
const Item *List_find(const List *l, int sku);

/* ---- removal ----------------------------------------------------- */

/* Remove the item with this sku.  Returns 1 if an item was removed,
   0 if no item had that sku.  Frees both the node and its name. */
int    List_remove(List *l, int sku);

/* Remove the front item and hand it to the caller.
   Returns 1 on success, 0 if the list was empty (and then *out is
   not written).  On success *out receives the item and OWNERSHIP OF
   out->name TRANSFERS TO THE CALLER: the list must not free that
   string, and the caller must free(out->name) when done. */
int    List_remove_front(List *l, Item *out);

/* ---- observers --------------------------------------------------- */

size_t List_size (const List *l);   /* constant time: keep a count  */
int    List_empty(const List *l);   /* 1 if empty, 0 otherwise      */

/* Back to empty -- every node and every name freed -- but the List
   itself survives and is reusable.  Safe on an already empty list. */
void   List_clear(List *l);

/* Print the whole list on ONE line, followed by '\n':
 *     empty:      [ ]
 *     one item:   [ (101,"bolt",4) ]
 *     three:      [ (101,"bolt",4) -> (102,"nut",9) -> (7,"cam",0) ]
 * That is: "[ ", then the items separated by " -> ", then " ]".
 * The autograder compares this output, so match it exactly. */
void   List_print(const List *l);

/* Traverse with a callback -- no Node * escapes, and the client
   still gets to visit every element.  `fn` is called once per item,
   front to back, with the caller's `ctx` passed through untouched.
   A NULL `fn` is a no-op. */
void   List_for_each(const List *l, void (*fn)(const Item *it, void *ctx),
                     void *ctx);

#endif /* LIST_H */
