/* ------------------------------------------------------------------ *
 * tests/public_tests.cpp -- ECE 412 Lab 3 provided test suite.
 *
 *   make check     build and run it
 *   make vcheck    run it under valgrind
 *
 * These are the same KINDS of tests the autograder runs, written in
 * the same C subset you are restricted to.  They are not the whole
 * autograder: passing all of them is necessary, not sufficient.  In
 * particular they do not check the constant-time requirements, they
 * do not check your Makefile or your README, and the autograder runs
 * several sequences that are deliberately not in here.
 *
 * You do not submit this file, and you should not edit it.  Your own
 * tests still belong in main.cpp -- that is what the rubric grades.
 * ------------------------------------------------------------------ */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>     /* dup, dup2 -- used only to capture stdout */

#include "list.h"

/* ---- tiny harness ----------------------------------------------- */

static int g_checks = 0;
static int g_fails  = 0;
static const char *g_test = "";

static void ck(int cond, const char *what)
{
    g_checks++;
    if (!cond) {
        g_fails++;
        printf("  FAIL  [%s] %s\n", g_test, what);
    }
}

static void begin(const char *name)
{
    g_test = name;
    printf("%s\n", name);
}

/* ---- capture what List_print writes to stdout -------------------- */

/* Collapse runs of whitespace to one space and trim both ends, so a
 * stray extra space does not fail you.  The shape still must match. */
static void squeeze(char *s)
{
    int i = 0, j = 0, sp = 0;
    while (s[i] != '\0') {
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' ||
            s[i] == '\r') {
            sp = 1;
        } else {
            if (sp && j > 0) s[j++] = ' ';
            sp = 0;
            s[j++] = s[i];
        }
        i++;
    }
    s[j] = '\0';
}

/* Run List_print(l) with stdout redirected into buf. */
static void capture_print(const List *l, char *buf, size_t cap)
{
    const char *path = "._print_capture.tmp";
    int   saved;
    FILE *f;
    long  n;

    buf[0] = '\0';

    f = fopen(path, "w+");
    if (f == NULL) return;

    fflush(stdout);
    saved = dup(1);
    if (saved < 0) { fclose(f); return; }
    if (dup2(fileno(f), 1) < 0) { close(saved); fclose(f); return; }

    List_print(l);

    fflush(stdout);
    dup2(saved, 1);
    close(saved);

    fseek(f, 0, SEEK_END);
    n = ftell(f);
    if (n < 0) n = 0;
    if ((size_t)n > cap - 1) n = (long)cap - 1;
    fseek(f, 0, SEEK_SET);
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) { /* short read */ }
    buf[n] = '\0';
    fclose(f);
    remove(path);
    squeeze(buf);
}

static void ck_print(const List *l, const char *expect)
{
    char got[4096];
    capture_print(l, got, sizeof got);
    if (strcmp(got, expect) != 0) {
        g_checks++;
        g_fails++;
        printf("  FAIL  [%s] List_print\n", g_test);
        printf("          expected: %s\n", expect);
        printf("          got:      %s\n", got);
    } else {
        g_checks++;
    }
}

/* ---- helpers ---------------------------------------------------- */

static int qty_of(const List *l, int sku)
{
    const Item *it = List_find(l, sku);
    return (it == NULL) ? -1 : it->qty;
}

static void sum_qty(const Item *it, void *ctx)
{
    long *acc = (long *)ctx;
    *acc += it->qty;
}

static void count_calls(const Item *it, void *ctx)
{
    (void)it;
    int *n = (int *)ctx;
    (*n)++;
}

/* ================================================================== *
 * tests
 * ================================================================== */

static void t_empty(void)
{
    begin("t_empty -- every operation on a fresh list");

    List *l = List_create();
    ck(l != NULL, "List_create must not return NULL");
    if (l == NULL) return;

    ck(List_size(l) == 0,            "size 0");
    ck(List_empty(l) == 1,           "empty 1");
    ck(List_find(l, 0) == NULL,      "find 0 -> NULL");
    ck(List_find(l, 999) == NULL,    "find 999 -> NULL");
    ck(List_remove(l, 42) == 0,      "remove -> 0");

    Item out;
    memset(&out, 0, sizeof out);
    ck(List_remove_front(l, &out) == 0, "remove_front -> 0");

    int calls = 0;
    List_for_each(l, count_calls, &calls);
    ck(calls == 0,                   "for_each visits nothing");
    List_for_each(l, NULL, NULL);    /* must not crash */

    ck_print(l, "[ ]");

    List_clear(l);                   /* clear of an empty list is ok */
    ck(List_size(l) == 0,            "size still 0 after clear");

    List_destroy(l);
    List_destroy(NULL);              /* documented no-op */
}

static void t_insert_front_order(void)
{
    begin("t_insert_front_order");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    ck(List_insert_front(l, 1, "a", 10) == 1, "insert a");
    ck(List_insert_front(l, 2, "b", 20) == 1, "insert b");
    ck(List_insert_front(l, 3, "c", 30) == 1, "insert c");

    ck(List_size(l) == 3,  "size 3");
    ck(List_empty(l) == 0, "not empty");
    ck_print(l, "[ (3,\"c\",30) -> (2,\"b\",20) -> (1,\"a\",10) ]");

    List_destroy(l);
}

static void t_insert_back_order(void)
{
    begin("t_insert_back_order");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    ck(List_insert_back(l, 1, "a", 10) == 1, "insert a");
    ck(List_insert_back(l, 2, "b", 20) == 1, "insert b");
    ck(List_insert_back(l, 3, "c", 30) == 1, "insert c");

    ck(List_size(l) == 3, "size 3");
    ck_print(l, "[ (1,\"a\",10) -> (2,\"b\",20) -> (3,\"c\",30) ]");

    List_destroy(l);
}

/* insert_front on an EMPTY list must also set tail.  If it does not,
 * the following insert_back links onto a node nobody points at. */
static void t_front_then_back(void)
{
    begin("t_front_then_back -- tail after a front insert on empty");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    ck(List_insert_front(l, 1, "a", 10) == 1, "front onto empty");
    ck(List_insert_back(l, 2, "b", 20) == 1,  "back after that");
    ck(List_size(l) == 2, "size 2");
    ck_print(l, "[ (1,\"a\",10) -> (2,\"b\",20) ]");

    ck(qty_of(l, 1) == 10, "item 1 intact");
    ck(qty_of(l, 2) == 20, "item 2 intact");

    List_destroy(l);
}

static void t_interleaved(void)
{
    begin("t_interleaved -- front/back/front/back");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back (l, 2, "b", 2);
    List_insert_front(l, 1, "a", 1);
    List_insert_back (l, 3, "c", 3);
    List_insert_front(l, 0, "z", 0);

    ck(List_size(l) == 4, "size 4");
    ck_print(l,
        "[ (0,\"z\",0) -> (1,\"a\",1) -> (2,\"b\",2) -> (3,\"c\",3) ]");

    List_destroy(l);
}

static void t_deep_copy(void)
{
    begin("t_deep_copy -- the list owns its own copy of the name");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    char buf[32];
    strcpy(buf, "bolt");
    List_insert_front(l, 101, buf, 4);
    strcpy(buf, "SCRIBBLED");

    const Item *it = List_find(l, 101);
    ck(it != NULL, "find after insert");
    if (it != NULL) {
        ck(strcmp(it->name, "bolt") == 0, "name unaffected by clobber");
        ck(it->name != buf, "name is not the caller's pointer");
    }

    /* a second insert of the same string must get its own copy */
    strcpy(buf, "nut");
    List_insert_back(l, 102, buf, 9);
    const Item *a = List_find(l, 101);
    const Item *b = List_find(l, 102);
    ck(a != NULL && b != NULL, "both present");
    if (a != NULL && b != NULL) ck(a->name != b->name, "separate copies");

    List_destroy(l);
}

static void t_remove_head(void)
{
    begin("t_remove_head");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 1, "a", 1);
    List_insert_back(l, 2, "b", 2);
    List_insert_back(l, 3, "c", 3);

    ck(List_remove(l, 1) == 1,       "removed the head");
    ck(List_size(l) == 2,            "size 2");
    ck(List_find(l, 1) == NULL,      "head is gone");
    ck_print(l, "[ (2,\"b\",2) -> (3,\"c\",3) ]");

    /* the new head must still be reachable and the tail still valid */
    ck(List_insert_back(l, 4, "d", 4) == 1, "insert_back still works");
    ck_print(l, "[ (2,\"b\",2) -> (3,\"c\",3) -> (4,\"d\",4) ]");

    List_destroy(l);
}

static void t_remove_middle(void)
{
    begin("t_remove_middle");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 1, "a", 1);
    List_insert_back(l, 2, "b", 2);
    List_insert_back(l, 3, "c", 3);

    ck(List_remove(l, 2) == 1,   "removed the middle");
    ck(List_size(l) == 2,        "size 2");
    ck(List_find(l, 2) == NULL,  "middle is gone");
    ck(qty_of(l, 1) == 1,        "predecessor intact");
    ck(qty_of(l, 3) == 3,        "successor still reachable");
    ck_print(l, "[ (1,\"a\",1) -> (3,\"c\",3) ]");

    List_destroy(l);
}

static void t_remove_tail(void)
{
    begin("t_remove_tail -- and then insert_back again");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 1, "a", 1);
    List_insert_back(l, 2, "b", 2);
    List_insert_back(l, 3, "c", 3);

    ck(List_remove(l, 3) == 1,   "removed the tail");
    ck(List_size(l) == 2,        "size 2");
    ck(List_find(l, 3) == NULL,  "tail is gone");

    /* This is the write-through-a-freed-pointer test. */
    ck(List_insert_back(l, 4, "d", 4) == 1, "insert_back after that");
    ck(List_size(l) == 3,        "size 3");
    ck_print(l, "[ (1,\"a\",1) -> (2,\"b\",2) -> (4,\"d\",4) ]");

    List_destroy(l);
}

static void t_remove_only_then_back(void)
{
    begin("t_remove_only_then_back -- the single-element list");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 7, "solo", 7);
    ck(List_remove(l, 7) == 1,  "removed the only element");
    ck(List_size(l) == 0,       "size 0");
    ck(List_empty(l) == 1,      "empty");
    ck_print(l, "[ ]");

    ck(List_insert_back(l, 8, "again", 8) == 1, "insert_back on empty");
    ck(List_size(l) == 1, "size 1");
    ck_print(l, "[ (8,\"again\",8) ]");

    /* and the same round trip through the front */
    ck(List_remove(l, 8) == 1, "removed it again");
    ck(List_insert_front(l, 9, "third", 9) == 1, "insert_front on empty");
    ck_print(l, "[ (9,\"third\",9) ]");

    List_destroy(l);
}

static void t_remove_absent(void)
{
    begin("t_remove_absent -- changes nothing");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 1, "a", 1);
    List_insert_back(l, 2, "b", 2);

    ck(List_remove(l, 99) == 0,  "absent sku -> 0");
    ck(List_size(l) == 2,        "size unchanged");
    ck_print(l, "[ (1,\"a\",1) -> (2,\"b\",2) ]");

    ck(List_insert_back(l, 3, "c", 3) == 1, "tail unharmed");
    ck(List_size(l) == 3, "size 3");

    List_destroy(l);
}

static void t_remove_all_orders(void)
{
    begin("t_remove_all_orders -- drain forwards, then backwards");

    int i;

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    for (i = 0; i < 5; i++) List_insert_back(l, i, "x", i);
    for (i = 0; i < 5; i++) ck(List_remove(l, i) == 1, "drain forwards");
    ck(List_size(l) == 0 && List_empty(l) == 1, "empty after drain");

    for (i = 0; i < 5; i++) List_insert_back(l, i, "x", i);
    for (i = 4; i >= 0; i--)
        ck(List_remove(l, i) == 1, "drain backwards");
    ck(List_size(l) == 0 && List_empty(l) == 1, "empty again");

    ck(List_insert_back(l, 42, "last", 42) == 1, "still usable");
    ck_print(l, "[ (42,\"last\",42) ]");

    List_destroy(l);
}

static void t_remove_front_ownership(void)
{
    begin("t_remove_front_ownership -- the caller owns out->name");

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    List_insert_back(l, 1, "alpha", 11);
    List_insert_back(l, 2, "beta",  22);

    Item out;
    memset(&out, 0, sizeof out);

    ck(List_remove_front(l, &out) == 1, "remove_front -> 1");
    ck(out.sku == 1 && out.qty == 11,   "got the front item");
    ck(out.name != NULL && strcmp(out.name, "alpha") == 0,
                                        "got the name");
    ck(List_size(l) == 1,               "size 1");

    /* The name must outlive the list.  If the list freed it, valgrind
     * reports an invalid read here, or the free below double-frees. */
    List_destroy(l);
    ck(out.name != NULL && strcmp(out.name, "alpha") == 0,
       "out->name still valid after List_destroy");
    free(out.name);
}

static void t_remove_front_until_empty(void)
{
    begin("t_remove_front_until_empty");

    int i;

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    for (i = 0; i < 4; i++) List_insert_back(l, i, "n", i);

    for (i = 0; i < 4; i++) {
        Item out;
        memset(&out, 0, sizeof out);
        ck(List_remove_front(l, &out) == 1, "remove_front -> 1");
        ck(out.sku == i, "front items come out in order");
        free(out.name);
    }

    Item out;
    memset(&out, 0, sizeof out);
    ck(List_remove_front(l, &out) == 0, "now empty -> 0");
    ck(List_empty(l) == 1, "empty");

    /* tail must have been reset when the list drained */
    ck(List_insert_back(l, 99, "z", 99) == 1, "insert_back after drain");
    ck_print(l, "[ (99,\"z\",99) ]");

    List_destroy(l);
}

static void t_clear_and_reuse(void)
{
    begin("t_clear_and_reuse");

    int i;

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    for (i = 0; i < 5; i++) List_insert_back(l, i, "item", i);
    List_clear(l);

    ck(List_size(l) == 0,  "size 0 after clear");
    ck(List_empty(l) == 1, "empty after clear");
    ck(List_find(l, 3) == NULL, "nothing found after clear");
    ck_print(l, "[ ]");

    List_clear(l);         /* clearing twice must be harmless */

    ck(List_insert_back(l, 10, "b", 10) == 1,  "insert_back after clear");
    ck(List_insert_front(l, 9, "a", 9) == 1,  "insert_front post-clear");
    ck(List_size(l) == 2, "size 2");
    ck_print(l, "[ (9,\"a\",9) -> (10,\"b\",10) ]");

    List_destroy(l);
}

static void t_for_each_sum(void)
{
    begin("t_for_each_sum -- ctx carries the accumulator");

    int i;

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    for (i = 1; i <= 10; i++) List_insert_back(l, i, "x", i);

    long total = 0;
    List_for_each(l, sum_qty, &total);
    ck(total == 55, "quantities sum to 55");

    int calls = 0;
    List_for_each(l, count_calls, &calls);
    ck(calls == 10, "callback runs once per item");

    List_destroy(l);
}

static void t_many(void)
{
    begin("t_many -- 1000 elements built, searched, destroyed");

    int i;
    char nm[32];

    List *l = List_create();
    if (l == NULL) { ck(0, "List_create NULL"); return; }

    for (i = 0; i < 1000; i++) {
        sprintf(nm, "part%d", i);
        if (List_insert_back(l, i, nm, i) != 1) {
            ck(0, "insert_back failed mid-build");
            List_destroy(l);
            return;
        }
    }

    ck(List_size(l) == 1000, "size 1000");
    ck(qty_of(l, 0)   == 0,   "first present");
    ck(qty_of(l, 500) == 500, "middle present");
    ck(qty_of(l, 999) == 999, "last present");
    ck(List_find(l, 1000) == NULL, "one past the end absent");

    const Item *it = List_find(l, 777);
    ck(it != NULL && strcmp(it->name, "part777") == 0, "name 777 right");

    /* remove every third one, then check the survivors */
    for (i = 0; i < 1000; i += 3) ck(List_remove(l, i) == 1, "remove 3k");
    ck(List_size(l) == 1000 - 334, "size after removing every third");
    ck(List_find(l, 999) == NULL,  "999 was a multiple of 3");
    ck(qty_of(l, 998) == 998,      "998 survives");

    long total = 0;
    List_for_each(l, sum_qty, &total);
    ck(total > 0, "for_each still walks the survivors");

    List_destroy(l);
}

/* ================================================================== */

int main(void)
{
    t_empty();
    t_insert_front_order();
    t_insert_back_order();
    t_front_then_back();
    t_interleaved();
    t_deep_copy();
    t_remove_head();
    t_remove_middle();
    t_remove_tail();
    t_remove_only_then_back();
    t_remove_absent();
    t_remove_all_orders();
    t_remove_front_ownership();
    t_remove_front_until_empty();
    t_clear_and_reuse();
    t_for_each_sum();
    t_many();

    printf("\n----------------------------------------\n");
    printf("%d checks, %d failures\n", g_checks, g_fails);
    if (g_fails == 0)
        printf("all provided tests passed "
               "(now run it under valgrind: make vcheck)\n");
    return (g_fails == 0) ? 0 : 1;
}
