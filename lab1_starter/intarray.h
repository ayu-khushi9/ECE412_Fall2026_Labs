/*
 * ================================================================
 * Filename:    intarray.h
 * Description: ECE 412 Lab 1, Part B -- IntArray, the growable array.
 *              DO NOT MODIFY THIS FILE. The autograder compiles its
 *              tests against these exact prototypes.
 * ================================================================
 */

#ifndef INTARRAY_H
#define INTARRAY_H

#include <stddef.h>   /* size_t */

typedef struct {
    int    *data;   /* owning pointer, from malloc/realloc; NULL when cap == 0 */
    size_t  size;   /* elements in use    */
    size_t  cap;    /* elements allocated */
} IntArray;

void   ia_init(IntArray *a);                  /* size = cap = 0, data = NULL   */
int    ia_push_back(IntArray *a, int v);      /* grows if full; 1 ok, 0 on OOM */
int    ia_get(const IntArray *a, size_t i);   /* precondition: i < a->size     */
int    ia_set(IntArray *a, size_t i, int v);  /* 1 ok, 0 if i out of range     */
int    ia_pop_back(IntArray *a, int *out);    /* 1 ok, 0 if empty              */
size_t ia_size(const IntArray *a);
size_t ia_capacity(const IntArray *a);
void   ia_clear(IntArray *a);                 /* size = 0, keeps the buffer    */
void   ia_destroy(IntArray *a);               /* free(data); data = NULL;
                                                 size = cap = 0                */

#endif /* INTARRAY_H */
