/*
 * ================================================================
 * Filename:    mystr.cpp
 * Description: ECE 412 Lab 1, Part A -- C string utilities,
 *              implemented from scratch. No <cstring> anywhere.
 * Created:     9/13/2026
 * EID:         ap62929
 * Email:       ap62929@my.utexas.edu
 * Author:      Ayushi Priyadarshani
 * Provenance:  I certify that all code contained herein is mine
 *              alone except where otherwise noted.
 * ================================================================
 */

#include <stdlib.h>    /* malloc, free -- NOT <cstring>! */

#include "mystr.h"

size_t my_strlen(const char *s) {
    /* TODO: walk to the '\0'. my_strlen("") must be 0. */
    int count = 0;
    for(int i = 0; s[i] != '\0'; i++){
        count++;
    }
    return count;
}

char *my_strdup(const char *s) {
    /* TODO: malloc exactly the right number of bytes (how many?),
     * copy s including the terminator, return the new buffer.
     * Return NULL if malloc fails. Remember: in C++, malloc's
     * void* must be cast: (char *)malloc(...). */
    size_t len = my_strlen(s);
    char *new_s = (char *)malloc((len + 1)*sizeof(char));
    if (new_s == NULL) {
        return NULL;
    }
    for (size_t i = 0; i <= len; i++) {
        new_s[i] = s[i];
    }
    return new_s;
}

int my_strcmp(const char *a, const char *b) {
    /* TODO: the strcmp contract. Compare as UNSIGNED CHAR --
     * plain char is signed on mario, and "\xFF" vs "a" will
     * come out with the wrong sign if you compare as char. */
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
    /* TODO: copy src into dst, never writing more than dstsize
     * bytes INCLUDING the terminator. On success return 1.
     * If src does not fit, leave dst a valid empty string and
     * return 0. Think about dstsize == 0 before you write. */
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
