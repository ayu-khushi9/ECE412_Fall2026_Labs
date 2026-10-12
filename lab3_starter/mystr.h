/* ------------------------------------------------------------------ *
 * mystr.h -- your string helper from Lab 2, reused here.
 *            C subset only: char *, malloc, free.
 * ------------------------------------------------------------------ */
#ifndef MYSTR_H
#define MYSTR_H

/* Return a freshly malloc'd copy of s, which the caller must free.
 * Returns NULL if s is NULL or if the allocation fails.
 * Do not call the POSIX strdup -- write this yourself, as in Lab 2. */
char *my_strdup(const char *s);

#endif /* MYSTR_H */
