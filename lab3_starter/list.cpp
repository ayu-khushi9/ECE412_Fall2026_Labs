/* ------------------------------------------------------------------ *
 * list.cpp -- ECE 412 Lab 3.  The implementation, and the ONLY place
 *             `struct Node` and `struct List` are allowed to exist.
 *
 * C subset of C++11 only.  malloc/free, char *, no classes, no STL.
 * Every function below has a box-and-arrow diagram you should draw
 * before you write it.  The diagram is the work; the code is the
 * transcription.
 * ------------------------------------------------------------------ */
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "list.h"
#include "mystr.h"

/* ------------------------------------------------------------------ *
 * The private shape.  These two definitions never leave this file.
 * ------------------------------------------------------------------ */

struct Node {
    Item  item;
    Node *next;
};

struct List {
    Node   *head;
    Node   *tail;   /* required: List_insert_back must be constant time */
    size_t  count;  /* required: List_size must be constant time       */
};

/* ------------------------------------------------------------------ *
 * Recommended private helpers.  Uncomment and fill these in.
 *
 * node_new is a single place that builds one fully formed node --
 * name deep-copied, next set to NULL -- and returns NULL if either
 * malloc fails, having freed whatever it already took.  Writing it
 * first makes both inserts about three lines long, and satisfies
 * "a failed allocation must not corrupt the list" in one place
 * instead of two.
 *
 *   static Node *node_new(int sku, const char *name, int qty)
 *   {
 *       ...
 *   }
 *
 *   static void node_free(Node *n)    // frees the name AND the node
 *   {
 *       ...
 *   }
 * ------------------------------------------------------------------ */

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * node_new: builds one complete, unlinked node.  Allocates the node,
 * deep-copies `name` with my_strdup, sets next = NULL.  If either
 * allocation fails it frees whatever it already took and returns
 * NULL, so callers never see a half-built node and the list is never
 * touched on failure.  A NULL name is rejected the same way.
 * ------------------------------------------------------------------ */
static Node *node_new(int sku, const char *name, int qty)
{
    Node *n;

    if (name == NULL)
        return NULL;

    n = (Node *)malloc(sizeof(Node));
    if (n == NULL)
        return NULL;

    n->item.name = my_strdup(name);
    if (n->item.name == NULL) {
        free(n);                /* undo the first allocation */
        return NULL;
    }

    n->item.sku = sku;
    n->item.qty = qty;
    n->next     = NULL;
    return n;
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * node_free: releases one node -- the name first (its own malloc),
 * then the node.  NULL is a no-op.
 * ------------------------------------------------------------------ */
static void node_free(Node *n)
{
    if (n == NULL)
        return;
    free(n->item.name);
    free(n);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * free_chain: frees every node from `cur` to the end.  `next` is
 * SAVED before the node is freed, so we never read freed memory
 * (no use-after-free).  Shared by List_destroy and List_clear.
 * ------------------------------------------------------------------ */
static void free_chain(Node *cur)
{
    while (cur != NULL) {
        Node *next = cur->next;   /* SAVE FIRST */
        free(cur->item.name);
        free(cur);
        cur = next;
    }
}

/* ------------------------------------------------------------------ *
 * lifetime
 * ------------------------------------------------------------------ */

List *List_create(void)
{
    /* TODO: create and initialize a List */
    /* AI-GENERATED (Claude): malloc the handle, NULL-check it, and
     * start it empty -- head and tail NULL, count 0. */
    List *l = (List *)malloc(sizeof(List));
    if (l == NULL)
        return NULL;
    l->head  = NULL;
    l->tail  = NULL;
    l->count = 0;
    return l;
}

void List_destroy(List *l)
{
    /* TODO: List_destroy(NULL) is a no-op.  Otherwise free everything in the 
     * correct order.*/
    (void)l; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): NULL is a no-op.  Otherwise free every
     * node and name (saving next first, inside free_chain), and only
     * then free the List handle itself. */
    if (l == NULL)
        return;
    free_chain(l->head);
    free(l);
}

/* ------------------------------------------------------------------ *
 * insertion
 * ------------------------------------------------------------------ */

int List_insert_front(List *l, int sku, const char *name, int qty)
{
    /* TODO: allocate, deep-copy the name, link at the head. */
    (void)l; (void)sku; (void)name; (void)qty; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): build the node first (nothing in the
     * list changes if that fails), then point it at the old head and
     * make it the new head.  If the list was empty, the new node is
     * also the tail -- forgetting that is the classic bug.  O(1). */
    Node *n;

    if (l == NULL)
        return 0;

    n = node_new(sku, name, qty);
    if (n == NULL)
        return 0;               /* list untouched */

    n->next = l->head;
    l->head = n;
    if (l->tail == NULL)        /* was empty: empty -> one transition */
        l->tail = n;
    l->count++;
    return 1;
}

int List_insert_back(List *l, int sku, const char *name, int qty)
{
    /* TODO: constant time via l->tail.  Do NOT walk the list.
     * On an empty list, head and tail both become the new node. */
    (void)l; (void)sku; (void)name; (void)qty;
    /* AI-GENERATED (Claude): build the node first, then append it
     * through l->tail without walking (constant time).  On an empty
     * list head and tail both become the new node. */
    Node *n;

    if (l == NULL)
        return 0;

    n = node_new(sku, name, qty);
    if (n == NULL)
        return 0;               /* list untouched */

    if (l->tail == NULL) {      /* empty list */
        l->head = n;
        l->tail = n;
    } else {
        l->tail->next = n;
        l->tail = n;
    }
    l->count++;
    return 1;
}

/* ------------------------------------------------------------------ *
 * lookup
 * ------------------------------------------------------------------ */

const Item *List_find(const List *l, int sku)
{
    /* TODO: return  requested item or NULL.
     * Note the const: this function may not modify anything. */
    (void)l; (void)sku;
    /* AI-GENERATED (Claude): walk front to back with a const node
     * pointer; return the address of the first matching item, or
     * NULL if none (or if l is NULL).  Modifies nothing. */
    const Node *cur;

    if (l == NULL)
        return NULL;

    for (cur = l->head; cur != NULL; cur = cur->next) {
        if (cur->item.sku == sku)
            return &cur->item;
    }
    return NULL;
}

/* ------------------------------------------------------------------ *
 * removal
 * ------------------------------------------------------------------ */

int List_remove(List *l, int sku)
{
    /* TODO: four cases, and you must test all four:
     *   1. empty list                  
     *   2. match is the head           
     *   3. match is in the middle      
     *   4. match is the tail          
     */
    (void)l; (void)sku; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): the Node ** idiom.  pp always points at
     * "the pointer that points at the current node" -- first
     * &l->head, then &prev->next.  Unlinking is then one statement,
     * *pp = victim->next, which covers the head case and the middle
     * case identically.  prev trails along only so the tail can be
     * repaired: if the victim was the tail, prev becomes the new tail
     * (NULL when the list just became empty).  Name and node are both
     * freed, and count drops by one.
     *   case 1 (empty):  *pp is NULL immediately -> return 0
     *   case 2 (head):   pp == &l->head, head moves; tail fixed if now empty
     *   case 3 (middle): pp == &prev->next, prev is rerouted
     *   case 4 (tail):   as case 3, plus tail = prev */
    Node **pp;
    Node  *prev;
    Node  *victim;

    if (l == NULL)
        return 0;

    pp   = &l->head;
    prev = NULL;
    while (*pp != NULL && (*pp)->item.sku != sku) {
        prev = *pp;
        pp   = &(*pp)->next;
    }

    if (*pp == NULL)            /* empty list, or sku absent */
        return 0;

    victim = *pp;
    *pp = victim->next;         /* unlink: head or predecessor's next */
    if (l->tail == victim)      /* removed the last node */
        l->tail = prev;         /* NULL if the list is now empty */

    node_free(victim);          /* name AND node */
    l->count--;
    return 1;
}

int List_remove_front(List *l, Item *out)
{
    /* TODO: implement, not forgetting to check if it is already empty */
    (void)l; (void)out; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): if empty, return 0 without touching
     * *out.  Otherwise copy the front item (including its name
     * pointer) into *out, unlink the node, fix tail if the list just
     * became empty, and free ONLY the node -- not the name.
     * Ownership of out->name passes to the caller, who must free it. */
    Node *n;

    if (l == NULL || l->head == NULL)
        return 0;

    n = l->head;
    if (out != NULL) {
        *out = n->item;         /* caller now owns out->name */
    } else {
        free(n->item.name);     /* no one to hand it to: don't leak it */
    }

    l->head = n->next;
    if (l->head == NULL)        /* one -> empty transition */
        l->tail = NULL;

    free(n);                    /* the node only, NOT the name */
    l->count--;
    return 1;
}

/* ------------------------------------------------------------------ *
 * observers
 * ------------------------------------------------------------------ */

size_t List_size(const List *l)
{
    /* TODO: return the maintained count.*/
    (void)l; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): constant time -- just return the count
     * every insert/remove keeps up to date.  NULL list has size 0. */
    if (l == NULL)
        return 0;
    return l->count;
}

int List_empty(const List *l)
{
    /* TODO */
    (void)l;
    /* AI-GENERATED (Claude): 1 if there are no items (or l is NULL),
     * 0 otherwise.  Constant time via count. */
    if (l == NULL)
        return 1;
    return (l->count == 0) ? 1 : 0;
}

void List_clear(List *l)
{
    /* TODO: same freeing loop as List_destroy, but the List survives
     * and must be left genuinely reusable. */ 
    (void)l; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): free every node and name (save-next-
     * first loop in free_chain), keep the handle, and reset ALL
     * three fields -- head, tail and count -- so the next insert_back
     * does not write through a stale tail. */
    if (l == NULL)
        return;
    free_chain(l->head);
    l->head  = NULL;
    l->tail  = NULL;
    l->count = 0;
}

void List_print(const List *l)
{
    /* TODO: exactly one line, exactly this shape:
     *     [ ]
     *     [ (101,"bolt",4) -> (102,"nut",9) ]
     * printf only.  No std::cout -- it is banned this week. */
    (void)l; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): prints "[", then " " before the first
     * item and " -> " before each later one, then " ]\n".  An empty
     * list therefore prints exactly "[ ]". */
    const Node *cur;
    int first = 1;

    printf("[");
    if (l != NULL) {
        for (cur = l->head; cur != NULL; cur = cur->next) {
            printf("%s(%d,\"%s\",%d)", first ? " " : " -> ",
                   cur->item.sku, cur->item.name, cur->item.qty);
            first = 0;
        }
    }
    printf(" ]\n");
}

void List_for_each(const List *l, void (*fn)(const Item *it, void *ctx),
                   void *ctx)
{
    /* TODO: call fn once per item, front to back, passing ctx
     * through untouched.  NULL fn is a no-op.  No Node * escapes --
     * this is how you get iteration out of an opaque type in C. */
    (void)l; (void)fn; (void)ctx; /* avoid unused parameter warning */
    /* AI-GENERATED (Claude): walk front to back calling fn on each
     * item with the caller's ctx.  Only const Item * is handed out;
     * the node never escapes.  NULL list or NULL fn is a no-op. */
    const Node *cur;

    if (l == NULL || fn == NULL)
        return;
    for (cur = l->head; cur != NULL; cur = cur->next)
        fn(&cur->item, ctx);
}