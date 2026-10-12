/* ------------------------------------------------------------------ *
 * main.cpp -- your test program for Lab 3.  Builds to ./inventory.
 *
 * This file is a CLIENT of the list.  It may use only what list.h
 * declares.  It may not name the private struct, may not contain the
 * token "->next", and may not #include "list.cpp".  If you find
 * yourself wanting any of those, the list is missing an operation.
 *
 * Three examples are written for you.  The rest of the list at the
 * bottom is yours -- the rubric grades whether you covered them.
 * ------------------------------------------------------------------ */
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "list.h"

/* ---- a two-line test harness, in the C subset ------------------- */

static int g_checks = 0;
static int g_fails  = 0;

static void check(int cond, const char *what)
{
    g_checks++;
    if (!cond) {
        g_fails++;
        printf("  FAIL: %s\n", what);
    }
}

/* ---- example 1: an empty list answers every question ------------ */

static void test_empty_list(void)
{
    printf("test_empty_list\n");

    List *l = List_create();
    check(l != NULL, "List_create returned NULL");
    if (l == NULL) return;

    check(List_size(l) == 0,        "fresh list size is 0");
    check(List_empty(l) == 1,       "fresh list is empty");
    check(List_find(l, 101) == NULL, "find on empty list is NULL");
    check(List_remove(l, 101) == 0, "remove on empty list returns 0");

    Item out;
    memset(&out, 0, sizeof out);
    check(List_remove_front(l, &out) == 0, "remove_front on empty is 0");

    printf("  expecting [ ] : ");
    List_print(l);

    List_destroy(l);
}

/* ---- example 2: the name is deep copied, not borrowed ----------- */

static void test_deep_copy(void)
{
    printf("test_deep_copy\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    char buf[16];
    strcpy(buf, "bolt");
    check(List_insert_front(l, 101, buf, 4) == 1, "insert_front ok");

    strcpy(buf, "SCRIBBLE");          /* clobber the caller's buffer */

    const Item *it = List_find(l, 101);
    check(it != NULL, "find finds the item we just inserted");
    if (it != NULL) {
        check(strcmp(it->name, "bolt") == 0,
              "stored name survives the caller clobbering its buffer");
        check(it->name != buf, "stored name is not the caller's pointer");
    }

    List_destroy(l);
}

/* ---- example 3: the stale-tail sequence ------------------------- */

static void test_stale_tail(void)
{
    printf("test_stale_tail\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    check(List_insert_back(l, 101, "bolt", 4) == 1, "insert_back ok");
    check(List_remove(l, 101) == 1,  "removed the only element");
    check(List_empty(l) == 1,        "list is empty again");

    /* If remove left tail pointing at the freed node, this write goes
     * through a dangling pointer.  It will often appear to work. */
    check(List_insert_back(l, 102, "nut", 9) == 1, "insert_back again");
    check(List_size(l) == 1,          "size is 1");

    const Item *it = List_find(l, 102);
    check(it != NULL && it->qty == 9, "the new item is really there");

    printf("  expecting [ (102,\"nut\",9) ] : ");
    List_print(l);

    List_destroy(l);
}

/* ------------------------------------------------------------------ *
 * TODO -- write these.  The rubric expects all of them.
 *
 *   test_single_element      remove it, then insert_back again, then
 *                            remove_front, then insert_front
 *   test_insert_interleaved  front/back/front/back, check the order
 *                            with List_print and with List_find
 *   test_remove_head
 *   test_remove_middle
 *   test_remove_tail         and then insert_back again
 *   test_remove_absent       returns 0 and changes nothing
 *   test_remove_front_owns   after remove_front, destroy the list,
 *                            then read out.name -- it must still be
 *                            valid -- then free it yourself
 *   test_clear_and_reuse     clear a 5-element list, check empty,
 *                            then insert_back and insert_front again
 *   test_many                build >= 1000 elements, check size and
 *                            a few spot finds, then destroy
 *   test_for_each_sum        sum the quantities through the ctx
 *                            pointer and check the total
 *
 * Every one of them must call List_destroy (or List_clear) on every
 * path out.  That is this week's whole point: you are the destructor.
 * ------------------------------------------------------------------ */

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * Order-checking helpers.  The client cannot walk the list itself,
 * so it records the skus in visiting order through List_for_each and
 * a ctx struct, then compares against the expected order.
 * ------------------------------------------------------------------ */
#define MAX_ORDER 16

typedef struct {
    int    skus[MAX_ORDER];
    size_t n;
} OrderCtx;

static void record_sku(const Item *it, void *ctx)
{
    OrderCtx *oc = (OrderCtx *)ctx;
    if (oc->n < MAX_ORDER)
        oc->skus[oc->n] = it->sku;
    oc->n++;
}

/* returns 1 if the list holds exactly `n` items in the order `want` */
static int order_is(const List *l, const int *want, size_t n)
{
    OrderCtx oc;
    size_t i;

    memset(&oc, 0, sizeof oc);
    List_for_each(l, record_sku, &oc);
    if (oc.n != n || List_size(l) != n)
        return 0;
    for (i = 0; i < n && i < MAX_ORDER; i++) {
        if (oc.skus[i] != want[i])
            return 0;
    }
    return 1;
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * Builds a 3-item list 1 -> 2 -> 3 with insert_back.  Returns NULL
 * (having cleaned up) if anything fails, so callers can bail out.
 * ------------------------------------------------------------------ */
static List *make_three(void)
{
    List *l = List_create();
    if (l == NULL) return NULL;
    if (!List_insert_back(l, 1, "one",   10) ||
        !List_insert_back(l, 2, "two",   20) ||
        !List_insert_back(l, 3, "three", 30)) {
        List_destroy(l);
        return NULL;
    }
    return l;
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_single_element: one item through every empty<->one transition
 * -- remove it, insert_back, remove_front, insert_front -- checking
 * size, emptiness and that each new item is really reachable (a stale
 * tail would break the final insert_back).
 * ------------------------------------------------------------------ */
static void test_single_element(void)
{
    printf("test_single_element\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    check(List_insert_front(l, 5, "washer", 1) == 1, "insert_front one item");
    check(List_size(l) == 1,  "size is 1");
    check(List_empty(l) == 0, "not empty");
    check(List_remove(l, 5) == 1, "remove the single element");
    check(List_empty(l) == 1, "empty after removing single element");

    check(List_insert_back(l, 6, "gear", 2) == 1, "insert_back after remove");
    check(List_find(l, 6) != NULL, "gear is findable");

    Item out;
    memset(&out, 0, sizeof out);
    check(List_remove_front(l, &out) == 1, "remove_front the single element");
    check(out.sku == 6 && out.qty == 2, "remove_front gave back gear");
    check(out.name != NULL && strcmp(out.name, "gear") == 0,
          "remove_front gave back the name");
    free(out.name);                    /* caller owns it now */
    check(List_empty(l) == 1, "empty after remove_front");

    check(List_insert_front(l, 7, "cam", 0) == 1, "insert_front after remove_front");
    /* insert_back right after: exercises tail set by insert_front */
    check(List_insert_back(l, 8, "pin", 3) == 1, "insert_back after insert_front");
    {
        int want[] = { 7, 8 };
        check(order_is(l, want, 2), "order is 7, 8");
    }
    printf("  expecting [ (7,\"cam\",0) -> (8,\"pin\",3) ] : ");
    List_print(l);

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_insert_interleaved: front/back/front/back, then checks the
 * resulting order (via for_each and print) and every item via find.
 * ------------------------------------------------------------------ */
static void test_insert_interleaved(void)
{
    printf("test_insert_interleaved\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    check(List_insert_front(l, 102, "nut",   9) == 1, "front 102");
    check(List_insert_back (l, 7,   "cam",   0) == 1, "back 7");
    check(List_insert_front(l, 101, "bolt",  4) == 1, "front 101");
    check(List_insert_back (l, 55,  "shim",  2) == 1, "back 55");

    {
        int want[] = { 101, 102, 7, 55 };
        check(order_is(l, want, 4), "order is 101, 102, 7, 55");
    }
    check(List_size(l) == 4, "size is 4");

    const Item *it = List_find(l, 101);
    check(it != NULL && strcmp(it->name, "bolt") == 0 && it->qty == 4, "find 101");
    it = List_find(l, 102);
    check(it != NULL && strcmp(it->name, "nut") == 0 && it->qty == 9, "find 102");
    it = List_find(l, 7);
    check(it != NULL && strcmp(it->name, "cam") == 0 && it->qty == 0, "find 7");
    it = List_find(l, 55);
    check(it != NULL && strcmp(it->name, "shim") == 0 && it->qty == 2, "find 55");

    printf("  expecting [ (101,\"bolt\",4) -> (102,\"nut\",9) -> (7,\"cam\",0) -> (55,\"shim\",2) ] : ");
    List_print(l);

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_remove_head: remove case 2 on 1->2->3.  Head moves to 2; an
 * insert_back afterwards confirms tail is still right.
 * ------------------------------------------------------------------ */
static void test_remove_head(void)
{
    printf("test_remove_head\n");

    List *l = make_three();
    if (l == NULL) { check(0, "make_three failed"); return; }

    check(List_remove(l, 1) == 1, "remove head (1)");
    check(List_find(l, 1) == NULL, "1 is gone");
    {
        int want[] = { 2, 3 };
        check(order_is(l, want, 2), "order is 2, 3");
    }
    check(List_insert_back(l, 4, "four", 40) == 1, "insert_back after head removal");
    {
        int want[] = { 2, 3, 4 };
        check(order_is(l, want, 3), "order is 2, 3, 4");
    }

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_remove_middle: remove case 3 on 1->2->3.  The predecessor (1)
 * must be rerouted to 3.
 * ------------------------------------------------------------------ */
static void test_remove_middle(void)
{
    printf("test_remove_middle\n");

    List *l = make_three();
    if (l == NULL) { check(0, "make_three failed"); return; }

    check(List_remove(l, 2) == 1, "remove middle (2)");
    check(List_find(l, 2) == NULL, "2 is gone");
    {
        int want[] = { 1, 3 };
        check(order_is(l, want, 2), "order is 1, 3");
    }
    printf("  expecting [ (1,\"one\",10) -> (3,\"three\",30) ] : ");
    List_print(l);

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_remove_tail: remove case 4 on 1->2->3.  2 must become the new
 * tail, so the following insert_back must land after 2.
 * ------------------------------------------------------------------ */
static void test_remove_tail(void)
{
    printf("test_remove_tail\n");

    List *l = make_three();
    if (l == NULL) { check(0, "make_three failed"); return; }

    check(List_remove(l, 3) == 1, "remove tail (3)");
    check(List_find(l, 3) == NULL, "3 is gone");
    {
        int want[] = { 1, 2 };
        check(order_is(l, want, 2), "order is 1, 2");
    }
    check(List_insert_back(l, 9, "nine", 90) == 1, "insert_back after tail removal");
    {
        int want[] = { 1, 2, 9 };
        check(order_is(l, want, 3), "order is 1, 2, 9 (tail was fixed)");
    }
    printf("  expecting [ (1,\"one\",10) -> (2,\"two\",20) -> (9,\"nine\",90) ] : ");
    List_print(l);

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_remove_absent: removing a sku that is not there returns 0 and
 * leaves size and order untouched.
 * ------------------------------------------------------------------ */
static void test_remove_absent(void)
{
    printf("test_remove_absent\n");

    List *l = make_three();
    if (l == NULL) { check(0, "make_three failed"); return; }

    check(List_remove(l, 42) == 0, "remove absent sku returns 0");
    {
        int want[] = { 1, 2, 3 };
        check(order_is(l, want, 3), "list unchanged after absent remove");
    }
    check(List_size(l) == 3, "size still 3");

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_remove_front_owns: after remove_front the caller owns
 * out.name.  Destroy the list, then prove the name is still valid by
 * reading it, then free it ourselves.  Valgrind catches both a double
 * free and a leak here.
 * ------------------------------------------------------------------ */
static void test_remove_front_owns(void)
{
    printf("test_remove_front_owns\n");

    List *l = make_three();
    if (l == NULL) { check(0, "make_three failed"); return; }

    Item out;
    memset(&out, 0, sizeof out);
    check(List_remove_front(l, &out) == 1, "remove_front ok");
    check(List_size(l) == 2, "size is 2 after remove_front");

    List_destroy(l);                   /* list gone; out.name must survive */

    check(out.sku == 1 && out.qty == 10, "out holds the old front item");
    check(out.name != NULL && strcmp(out.name, "one") == 0,
          "out.name still valid after List_destroy");
    free(out.name);                    /* we own it: we free it */
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_clear_and_reuse: clear a 5-item list, confirm empty, then
 * reuse it with insert_back and insert_front (a stale tail after
 * clear would corrupt the insert_back).
 * ------------------------------------------------------------------ */
static void test_clear_and_reuse(void)
{
    printf("test_clear_and_reuse\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    int i;
    for (i = 0; i < 5; i++)
        check(List_insert_back(l, i, "part", i) == 1, "fill 5");
    check(List_size(l) == 5, "size is 5 before clear");

    List_clear(l);
    check(List_size(l) == 0, "size 0 after clear");
    check(List_empty(l) == 1, "empty after clear");
    check(List_find(l, 0) == NULL, "nothing findable after clear");
    printf("  expecting [ ] : ");
    List_print(l);

    List_clear(l);                     /* clearing an empty list is safe */
    check(List_empty(l) == 1, "clear on empty list is safe");

    check(List_insert_back(l, 20, "back", 2) == 1, "insert_back after clear");
    check(List_insert_front(l, 10, "front", 1) == 1, "insert_front after clear");
    {
        int want[] = { 10, 20 };
        check(order_is(l, want, 2), "order is 10, 20 after reuse");
    }

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_many: build 1000 items alternating front/back inserts, check
 * size and spot-find several, then destroy (valgrind checks it all
 * came back).
 * ------------------------------------------------------------------ */
static void test_many(void)
{
    printf("test_many\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    const int N = 1000;
    char name[32];
    int i, ok = 1;
    for (i = 0; i < N; i++) {
        snprintf(name, sizeof name, "item%d", i);
        int r = (i % 2 == 0) ? List_insert_back(l, i, name, i * 2)
                             : List_insert_front(l, i, name, i * 2);
        if (r != 1) ok = 0;
    }
    check(ok, "all 1000 inserts succeeded");
    check(List_size(l) == (size_t)N, "size is 1000");

    const Item *it = List_find(l, 0);
    check(it != NULL && strcmp(it->name, "item0") == 0 && it->qty == 0, "find 0");
    it = List_find(l, 499);
    check(it != NULL && strcmp(it->name, "item499") == 0 && it->qty == 998, "find 499");
    it = List_find(l, 998);
    check(it != NULL && strcmp(it->name, "item998") == 0 && it->qty == 1996, "find 998");
    it = List_find(l, 999);
    check(it != NULL && strcmp(it->name, "item999") == 0, "find 999");
    check(List_find(l, N) == NULL, "find past the end is NULL");

    List_destroy(l);
}

/* ---- AI-GENERATED (Claude, Anthropic) ------------------------------
 * test_for_each_sum: sum quantities through the ctx pointer.  Also
 * checks that a NULL callback and an empty list are both no-ops.
 * ------------------------------------------------------------------ */
static void sum_qty(const Item *it, void *ctx)
{
    int *total = (int *)ctx;
    *total += it->qty;
}

static void test_for_each_sum(void)
{
    printf("test_for_each_sum\n");

    List *l = List_create();
    if (l == NULL) { check(0, "List_create returned NULL"); return; }

    int total = 0;
    List_for_each(l, sum_qty, &total);
    check(total == 0, "sum over empty list is 0");

    check(List_insert_back(l, 101, "bolt", 4) == 1, "insert bolt");
    check(List_insert_back(l, 102, "nut",  9) == 1, "insert nut");
    check(List_insert_back(l, 7,   "cam",  0) == 1, "insert cam");

    total = 0;
    List_for_each(l, sum_qty, &total);
    check(total == 13, "sum of quantities is 13");

    List_for_each(l, NULL, &total);    /* NULL fn: no-op */
    check(total == 13, "NULL fn changes nothing");

    List_destroy(l);
}

int main(void)
{
    test_empty_list();
    test_deep_copy();
    test_stale_tail();

    /* TODO: call your tests here. */
    /* AI-GENERATED (Claude): run every required test. */
    test_single_element();
    test_insert_interleaved();
    test_remove_head();
    test_remove_middle();
    test_remove_tail();
    test_remove_absent();
    test_remove_front_owns();
    test_clear_and_reuse();
    test_many();
    test_for_each_sum();

    printf("\n%d checks, %d failures\n", g_checks, g_fails);
    return (g_fails == 0) ? 0 : 1;
}