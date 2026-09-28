/*
 * ================================================================
 * Filename:    mystr.cpp
 * Description: ECE 412 Lab 2 -- C string utilities. This is the
 *              same file you wrote for Lab 1. Bring your Lab 1
 *              implementation over, or reimplement here.
 *
 *              Only my_strdup is strictly required by db.cpp for
 *              this lab. The others are here so your `main.cpp`
 *              can keep using the same toolkit you built last week.
 * Created:     TODO
 * EID:         TODO
 * Email:       TODO
 * Author:      TODO
 * Provenance:  I certify that all code contained herein is mine
 *              alone except where otherwise noted.
 * ================================================================
 */

#include <stdlib.h>    /* malloc, free -- NOT <cstring>! */

#include "mystr.h"

size_t my_strlen(const char *s) {
    /* TODO (from Lab 1). */
    int count = 0;
    for(int i = 0; s[i] != '\0'; i++){
        count++;
    }
    return count;
}

char *my_strdup(const char *s) {
    /* TODO (from Lab 1). db.cpp depends on this: DB_add's deep copy
     * of the caller's `name` is a my_strdup, and the trap of storing
     * the caller's `const char *` directly is the same dangling-pointer
     * bug we discussed last week. */
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

int my_strcmp(const char *a, const char *b) {
    /* TODO (from Lab 1). */
    for(int i = 0; a[i] != '\0' && b[i] != '\0'; i++){
        if((unsigned char)a[i] < (unsigned char)b[i]){
            return -1;
        }
        else if((unsigned char)a[i] > (unsigned char)b[i]){
            return 1;
        }
    }
    return 0;
}

int my_strcpy_safe(char *dst, size_t dstsize, const char *src) {
    /* TODO (from Lab 1). */
    if (dstsize == 0) {
        return 0;
    }
    size_t src_len = my_strlen(src);
    if (src_len >= dstsize) {
        dst[0] = '\0';
        return 0;
    }
    for (size_t i = 0; i <= src_len; i++) {
        dst[i] = src[i];
    }
    return 1;
    
}
