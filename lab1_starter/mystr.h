/*
 * ================================================================
 * Filename:    mystr.h
 * Description: ECE 412 Lab 1, Part A -- C string utilities.
 *              DO NOT MODIFY THIS FILE. The autograder compiles its
 *              tests against these exact prototypes.
 * ================================================================
 */

#ifndef MYSTR_H
#define MYSTR_H

#include <stddef.h>   /* size_t */

/* Number of characters before the '\0'. my_strlen("") == 0. */
size_t my_strlen(const char *s);

/* malloc a new buffer of exactly the right size, copy s into it
 * INCLUDING the terminator, return it. Returns NULL if malloc fails.
 * The CALLER owns the result and must free it. */
char *my_strdup(const char *s);

/* <0 if a sorts before b, 0 if equal, >0 if after. Compare as
 * UNSIGNED CHAR values at the first position where they differ. */
int my_strcmp(const char *a, const char *b);

/* Copy src into dst, never writing more than dstsize bytes
 * (terminator included). Returns 1 on success, 0 if src did not fit,
 * in which case dst is left as a valid empty string (dstsize >= 1). */
int my_strcpy_safe(char *dst, size_t dstsize, const char *src);

#endif /* MYSTR_H */
