/*
 * ================================================================
 * Filename:    main.cpp
 * Description: ECE 412 Lab 1, Part C -- driver. Exercises Parts A
 *              and B, including every failure path you can reach.
 *              This whole program must be valgrind-clean.
 * Created:     9/13/2026
 * EID:         ap62929
 * Email:       ap62929@my.utexas.edu
 * Author:      Ayushi Priyadarshani
 * Provenance:  I certify that all code contained herein is mine
 *              alone except where otherwise noted.
 * ================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "intarray.h"
#include "mystr.h"

static void sort_names(char **names, int n) {
    for (int i = 1; i < n; i++) {
        char *key = names[i];
        int j = i - 1;
        while (j >= 0 && my_strcmp(names[j], key) > 0) {
            names[j + 1] = names[j];
            j--;
        }
        names[j + 1] = key;
    }
}

int main(void) {
    /* ---- 1. Growth demo ------------------------------------------------
     * Push at least 40 values so the array doubles at least three
     * times (4 -> 8 -> 16 -> 32 -> 64). After EACH push print size, cap,
     * and the buffer address, so the doublings -- and any address change --
     * are visible:
     *
     *     printf("push %2d: size=%2zu cap=%2zu data=%p\n",
     *            v, ia_size(&a), ia_capacity(&a), (void *)a.data);
     *
     * Circle in your output where the address changed (or did not) and
     * connect it to your WRITEUP's realloc-vs-manual answer.
     */
    IntArray a;
    ia_init(&a);
    for (int v = 1; v <= 40; v++) {
        ia_push_back(&a, v);
        printf("push %2d: size=%2zu cap=%2zu data=%p\n",
               v, ia_size(&a), ia_capacity(&a), (void *)a.data);
    }
    ia_destroy(&a);

    /* ---- 2. Names: the two-level ownership -----------------------------
     * Build an array of my_strdup'd names (a char** you malloc, or a
     * fixed char *names[N]), sort it with my_strcmp (your own insertion
     * sort is fine), print the sorted names -- then free EVERY string AND
     * (if malloc'd) the array of pointers. Two levels, two kinds of free.
     */
    const char *raw[] = {"Zara", "Alice", "Mona", "Bob", "Charlie"};
    int n = 5;
    char **names = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) names[i] = my_strdup(raw[i]);

    sort_names(names, n);
    printf("\nSorted names:\n");
    for (int i = 0; i < n; i++) printf("  %s\n", names[i]);

    for (int i = 0; i < n; i++) free(names[i]);
    free(names);

    /* ---- 3. Failure paths ----------------------------------------------
     * Each printed so the grader can see it happened:
     *   - ia_pop_back on an empty array          (expect return 0)
     *   - ia_get / ia_set out of range           (expect ia_set -> 0)
     *   - ia_destroy called twice                (expect: no crash)
     *   - my_strcpy_safe into a too-small buffer (expect return 0, dst "")
     */
    IntArray empty;
    ia_init(&empty);

    int out;
    printf("\nia_pop_back on empty:     %d (expect 0)\n", ia_pop_back(&empty, &out));

    ia_push_back(&empty, 42);
    printf("ia_set out-of-range:      %d (expect 0)\n", ia_set(&empty, 99, 7));

    ia_destroy(&empty);
    ia_destroy(&empty);
    printf("double ia_destroy:        ok (no crash)\n");

    char small[3];
    int r = my_strcpy_safe(small, sizeof(small), "toolong");
    printf("my_strcpy_safe too-small: ret=%d dst=\"%s\" (expect 0, \"\")\n", r, small);

    return 0;
}