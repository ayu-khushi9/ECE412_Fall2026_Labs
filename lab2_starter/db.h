/*
 * ================================================================
 * Filename:    db.h
 * Description: ECE 412 Lab 2 -- Record ADT with an OPAQUE type.
 *              DO NOT MODIFY THIS FILE. The autograder compiles its
 *              tests against these exact prototypes.
 *
 *              The whole point of this header is what it does NOT
 *              contain: the layout of `struct DB`. That definition
 *              lives ONLY in db.cpp. Callers hold a `DB *` and pass
 *              it around; they cannot see, size, allocate on the
 *              stack, or modify the internals. That is information
 *              hiding enforced by the compiler -- three lectures
 *              before we meet `private:`.
 * ================================================================
 */

#ifndef DB_H
#define DB_H

#include <cstddef>   /* size_t */

/* A record. The DB owns the `name` buffer; clients must not free it. */
typedef struct {
    int     id;      /* unique, >= 0                        */
    char   *name;    /* malloc'd copy owned by the DB       */
    double  gpa;     /* 0.0 .. 4.0                          */
} Record;

/* Incomplete ("opaque") type: clients may hold a DB *, but they cannot
   see what is inside, cannot declare `DB db;` on the stack, and cannot
   write db->count. Only db.cpp knows the layout. */
typedef struct DB DB;

DB     *DB_create(size_t initial_capacity);
                /* returns NULL on allocation failure. If initial_capacity
                   is 0, uses 4. */

void    DB_destroy(DB *db);
                /* frees every record's name, the record array, and the DB
                   itself. DB_destroy(NULL) is a no-op. */

int     DB_add(DB *db, int id, const char *name, double gpa);
                /* deep-copies name. Returns 1 on success, 0 on allocation
                   failure or duplicate id. Grows the array when full. */

const Record *DB_find(const DB *db, int id);
                /* returns a pointer to the stored record, or NULL if absent.
                   The pointer is const: clients read, they do not mutate,
                   and they never free it. */

int     DB_remove(DB *db, int id);
                /* frees that record's name, removes it, keeps the array
                   contiguous. Returns 1 if removed, 0 if not present. */

int     DB_update_gpa(DB *db, int id, double gpa);
                /* 1 if updated, 0 if id absent */

size_t  DB_size(const DB *db);
size_t  DB_capacity(const DB *db);

void    DB_print(const DB *db);
                /* one line per record, printf, in insertion order.
                   Format: "id=<n> name=<s> gpa=<g>\n" */

int     DB_save(const DB *db, const char *path);
int     DB_load(DB *db, const char *path);
                /* text format, one record per line: id,name,gpa
                   Returns 1 on success, 0 on I/O or parse failure.
                   On a parse failure DB_load leaves the DB unchanged
                   from before the call (or with the records it had
                   already successfully added -- see your README). */

#endif /* DB_H */
