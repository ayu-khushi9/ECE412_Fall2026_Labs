/*
 * ================================================================
 * Filename:    intarray.cpp
 * Description: ECE 412 Lab 1, Part B -- IntArray: a growable array
 *              built from malloc/realloc/free. This is what
 *              std::vector does under the hood.
 * Created:     9/13/2026
 * EID:         ap62929
 * Email:       ap62929@my.utexas.edu
 * Author:      Ayushi Priyadarshani
 * Provenance:  I certify that all code contained herein is mine
 *              alone except where otherwise noted.
 * ================================================================
 */

#include <stdlib.h>

#include "intarray.h"

/* Both growth strategies must be implemented and submitted.
 * Build with -DGROW_WITH_REALLOC=0 for the manual version and
 * -DGROW_WITH_REALLOC=1 for the realloc version (see Makefile).
 * The autograder builds and tests BOTH. */
#ifndef GROW_WITH_REALLOC
#define GROW_WITH_REALLOC 1
#endif

/* Grow a's buffer to newcap elements. Returns 1 ok, 0 on OOM.
 * On failure the array must be UNCHANGED and still valid. */
static int ia_grow(IntArray *a, size_t newcap) {
#if GROW_WITH_REALLOC
    /* TODO, Version 2: realloc. THE GRADED TRAP lives here:
     *   a->data = (int *)realloc(a->data, ...);
     * leaks the old block if realloc fails. Assign to a temporary,
     * check it, THEN commit. Your WRITEUP explains why. */
    int *temp = (int *)realloc(a->data, newcap * sizeof(int));
    if (temp == NULL) {
        return 0; // Out of memory, array is unchanged
    }
    a->data = temp;
    a->cap = newcap;
    return 1;
#else
    /* TODO, Version 1: malloc a bigger buffer, copy a->size
     * elements over, free the old buffer, commit pointer and cap.
     * On malloc failure: return 0 with the old array intact. */
    int *new_data = (int *)malloc(newcap * sizeof(int));
    if (new_data == NULL) {
        return 0; // Out of memory, array is unchanged
    }
    for (size_t i = 0; i < a->size; i++) {
        new_data[i] = a->data[i];
    }
    free(a->data);
    a->data = new_data;
    a->cap = newcap;
    return 1;   
#endif
}

void ia_init(IntArray *a) {
    /* TODO */
    a->data = NULL;
    a->size = 0;
    a->cap = 0;
}

int ia_push_back(IntArray *a, int v) {
    /* TODO: first push allocates capacity 4 (via ia_grow);
     * when size == cap, grow to 2 * cap; then store v.
     * (The ia_grow(a, 0) below only silences the unused-function
     * warning while ia_grow is a stub -- replace it with real calls.) */
    if (a->cap == 0) {
        if (!ia_grow(a, 4)) return 0;
    } else if (a->size == a->cap) {
        if (!ia_grow(a, a->cap * 2)) return 0;
    }
    a->data[a->size++] = v;
    return 1;
}

int ia_get(const IntArray *a, size_t i) {
    /* Precondition: i < a->size. TODO */
    return a->data[i];
}

int ia_set(IntArray *a, size_t i, int v) {
    /* TODO: 1 ok, 0 if i out of range. */
    if (i >= a->size) return 0;
    a->data[i] = v;
    return 1;
}

int ia_pop_back(IntArray *a, int *out) {
    /* TODO: 1 ok (value via *out), 0 if empty. */
    if (a->size == 0) return 0;
    *out = a->data[--a->size];
    return 1;
}

size_t ia_size(const IntArray *a) {
    /* TODO */
    return a->size;
}

size_t ia_capacity(const IntArray *a) {
    /* TODO */
    return a->cap;
}

void ia_clear(IntArray *a) {
    /* TODO: size = 0, KEEP the buffer. (How does this differ
     * from ia_destroy? One sentence in your WRITEUP.) */
    a->size = 0;
}

void ia_destroy(IntArray *a) {
    /* TODO: free the buffer, then null/zero every field so a
     * second ia_destroy is harmless. That nulling is not
     * politeness -- it is the whole defense against double free. */
    free(a->data);
    a->data = NULL;
    a->size = 0;
    a->cap  = 0;
}
