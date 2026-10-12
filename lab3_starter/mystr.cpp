/* ------------------------------------------------------------------ *
 * mystr.cpp -- my_strdup, from Lab 2.
 * ------------------------------------------------------------------ */
#include <cstdlib>
#include <cstring>

#include "mystr.h"

size_t my_strlen(const char *s) {
    /* TODO (from Lab 1). */
    int count = 0;
    for(int i = 0; s[i] != '\0'; i++){
        count++;
    }
    return count;
}

char *my_strdup(const char *s)
{
    /* TODO: bring your Lab 2 implementation over.
     *
     *   1. if s is NULL, return NULL
     *   2. measure it (strlen)
     *   3. malloc strlen + 1 -- the +1 is the bug everyone writes once
     *   4. NULL-check the malloc
     *   5. copy, including the terminator
     */
    size_t len = my_strlen(s);
    char *new_s = (char *)malloc((len + 1)*sizeof(char));
    if (new_s == NULL) {
        return NULL;
    }
    for (size_t i = 0; i <= len; i++) {
        new_s[i] = s[i];
    }
    return new_s;

    return NULL;
}
