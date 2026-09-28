/*
 * ================================================================
 * Filename:    db.cpp
 * Description: ECE 412 Lab 2 -- the Record ADT. The struct DB
 *              definition below lives ONLY in this file. Nothing
 *              in main.cpp may know it exists. That is the whole
 *              lesson: encapsulation without a `class`, enforced
 *              by the compiler because the header declined to
 *              describe the layout.
 * Created:     9/20/2026
 * EID:         ap62929
 * Email:       ap62929@utexas.edu
 * Author:      Ayushi Priyadarshani
 * Provenance:  I certify that all code contained herein is mine
 *              alone except where otherwise noted.
 * ================================================================
 */


#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "mystr.h"

/* The layout of DB. This struct is graded: it MUST NOT appear in db.h,
 * and main.cpp MUST NOT know it exists. Delete this definition from
 * db.h if a well-meaning refactor ever puts a copy there -- if main.cpp
 * still compiles after that deletion, you did it right. */
struct DB {
    Record *items;    /* malloc'd array of Record, grows by doubling */
    size_t  count;
    size_t  cap;
};

/* ---------- helpers ---------- */

/* Grow db->items to newcap slots. Returns 1 ok, 0 on OOM.
 * On failure the DB must be UNCHANGED and still valid.
 * (The __attribute__((unused)) is only here so the starter compiles
 * clean before you wire this into DB_add -- delete it once you do.) */
static int db_grow(DB *db, size_t newcap) {
    /* TODO: use realloc OR malloc-copy-free. Follow Lab 1's rule --
     * do NOT do `db->items = (Record *)realloc(db->items, ...)`, that
     * leaks the old block on failure. Assign to a temporary first,
     * check it, THEN commit db->items and db->cap. */

    /* AI-generated: Assigns realloc result to a temporary so that if
     * it fails, db->items and db->cap are untouched and the DB stays
     * valid. Only commits once we know the allocation succeeded. */
    Record *tmp = (Record *)realloc(db->items, newcap * sizeof(Record));
    if (tmp == NULL) return 0;
    db->items = tmp;
    db->cap   = newcap;
    return 1;
}

/* Return the index of the record with this id, or db->count if absent.
 * (__attribute__((unused)) only for the starter, same as above.) */
static size_t db_index_of(const DB *db, int id) {
    /* TODO: linear scan. Return db->count when not found (a common
     * "not found" sentinel for zero-based arrays). */

    /* AI-generated: Walks the items array left to right comparing ids.
     * Returns db->count (one past the last valid index) as the sentinel
     * so callers can check `i == db->count` uniformly. */
    for (size_t i = 0; i < db->count; i++) {
        if (db->items[i].id == id) return i;
    }
    return db->count;
}

/* ---------- lifecycle ---------- */

DB *DB_create(size_t initial_capacity) {
    /* TODO: malloc a DB, malloc db->items with initial_capacity slots
     * (or 4 if the caller passed 0), initialize count = 0. If EITHER
     * malloc fails, clean up the one that succeeded and return NULL.
     * That partial-cleanup case is the leak valgrind will find. */

    /* AI-generated: Allocates the DB shell first, then the items array.
     * If the items malloc fails the DB shell is freed before returning
     * NULL -- this is the partial-cleanup valgrind would catch otherwise. */
    if (initial_capacity == 0) initial_capacity = 4;
    DB *db = (DB *)malloc(sizeof(DB));
    if (db == NULL) return NULL;
    db->items = (Record *)malloc(initial_capacity * sizeof(Record));
    if (db->items == NULL) {
        free(db);
        return NULL;
    }
    db->count = 0;
    db->cap   = initial_capacity;
    return db;
}

void DB_destroy(DB *db) {
    /* TODO: DB_destroy(NULL) is a no-op. Otherwise:
     *   1) free each items[i].name        (every owned string)
     *   2) free(db->items)                 (the record array)
     *   3) free(db)                        (the DB itself)
     * That order is not stylistic -- freeing items first orphans
     * every name, and valgrind reports those as `indirectly lost`. */

    /* AI-generated: Guards against NULL, then frees in the only safe
     * order: each name first (freeing items before names would orphan
     * them and show as indirectly lost in valgrind), then the array,
     * then the shell itself. */
    if (db == NULL) return;
    for (size_t i = 0; i < db->count; i++) {
        free(db->items[i].name);
    }
    free(db->items);
    free(db);
}

/* ---------- mutation ---------- */

int DB_add(DB *db, int id, const char *name, double gpa) {
    /* TODO:
     *   - reject duplicate id (return 0)
     *   - grow if count == cap
     *   - my_strdup(name); if that fails, return 0 with DB unchanged
     *   - append and increment count
     */

    /* AI-generated: Checks for a duplicate id first, then grows the
     * array if it is full. The name is deep-copied via my_strdup so
     * the DB owns the string independent of the caller's buffer; if
     * strdup fails the DB is left unchanged and 0 is returned. */
    if (db_index_of(db, id) != db->count) return 0;   /* duplicate */
    if (db->count == db->cap) {
        if (!db_grow(db, db->cap * 2)) return 0;
    }
    char *copy = my_strdup(name);
    if (copy == NULL) return 0;
    db->items[db->count].id   = id;
    db->items[db->count].name = copy;
    db->items[db->count].gpa  = gpa;
    db->count++;
    return 1;
}

int DB_remove(DB *db, int id) {
    /* TODO:
     *   - find the id; if absent return 0
     *   - FREE items[i].name FIRST (the most commonly missed free
     *     in this lab -- valgrind will catch it)
     *   - close the gap. Either shift the tail down (stable, O(n))
     *     or swap in the last element (unstable, O(1)). Say which
     *     you picked, and why, in your README.
     *   - decrement count
     */

    /* AI-generated: Finds the slot, frees its name before touching
     * anything else (skipping this free is the most common valgrind
     * hit on this lab), then shifts the tail one slot left to keep the
     * array contiguous. Strategy: shift-down (stable, O(n)) -- preserves
     * insertion order so DB_print output is predictable for testing. */
    size_t i = db_index_of(db, id);
    if (i == db->count) return 0;
    free(db->items[i].name);
    for (size_t j = i; j < db->count - 1; j++) {
        db->items[j] = db->items[j + 1];
    }
    db->count--;
    return 1;
}

int DB_update_gpa(DB *db, int id, double gpa) {
    /* TODO: find, then overwrite items[i].gpa. Return 0 if absent. */

    /* AI-generated: Looks up the record and overwrites its gpa field
     * in place. Returns 0 without modifying anything if the id is absent. */
    size_t i = db_index_of(db, id);
    if (i == db->count) return 0;
    db->items[i].gpa = gpa;
    return 1;
}

/* ---------- read-only ---------- */

const Record *DB_find(const DB *db, int id) {
    /* TODO: return a pointer to the stored record (NOT a copy) or
     * NULL if absent. The return type is `const Record *` on purpose
     * -- explain in your README why returning a non-const `Record *`
     * would break the ADT (client could free(r->name) or overwrite
     * r->id behind the DB's back). */

    /* AI-generated: Returns a direct pointer into the items array --
     * not a copy -- so the caller can read fields cheaply. The const
     * qualifier prevents the caller from mutating the record or freeing
     * the name behind the DB's back (see README). */
    size_t i = db_index_of(db, id);
    if (i == db->count) return NULL;
    return &db->items[i];
}

size_t DB_size(const DB *db) {
    /* TODO */

    /* AI-generated: Exposes the live record count without letting the
     * caller see the struct's internals. */
    return db->count;
}

size_t DB_capacity(const DB *db) {
    /* TODO */

    /* AI-generated: Exposes the allocated slot count so the driver can
     * watch the doubling events as records are added. */
    return db->cap;
}

void DB_print(const DB *db) {
    /* TODO: one line per record, in insertion order. Use exactly
     *   printf("id=%d name=%s gpa=%.2f\n", id, name, gpa);
     * so the autograder's diff can find your output. */

    /* AI-generated: Iterates in insertion order and prints the exact
     * format the autograder's diff expects -- no extra labels or spaces. */
    for (size_t i = 0; i < db->count; i++) {
        printf("id=%d name=%s gpa=%.2f\n",
               db->items[i].id, db->items[i].name, db->items[i].gpa);
    }
}

/* ---------- persistence ---------- */

int DB_save(const DB *db, const char *path) {
    /* TODO: fopen(path, "w"), one line per record:
     *   fprintf(f, "%d,%s,%.2f\n", id, name, gpa);
     * fclose. Return 0 on any I/O failure. Names in your test data
     * will not contain commas or newlines -- do not worry about
     * quoting for this lab. */

    /* AI-generated: Opens the file for writing and emits one CSV line
     * per record. Returns 0 on any I/O failure (including a failed
     * fprintf for cases like a full disk) and always fcloses before
     * returning to avoid leaking the FILE handle. */
    FILE *f = fopen(path, "w");
    if (f == NULL) return 0;
    for (size_t i = 0; i < db->count; i++) {
        if (fprintf(f, "%d,%s,%.2f\n",
                    db->items[i].id,
                    db->items[i].name,
                    db->items[i].gpa) < 0) {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

int DB_load(DB *db, const char *path) {
    /* TODO: fopen(path, "r"), read one line at a time, parse
     *   id,name,gpa
     * with fscanf or by hand. For each valid line call DB_add;
     * on a parse failure (missing field, malformed number, ...)
     * stop, fclose, return 0. Do NOT crash on a huge line -- pick
     * a reasonable maximum name length (256 is fine) and reject
     * anything longer.
     *
     * Whether the records added before the bad line stay in the
     * DB is your design choice -- document it in the README. */

    /* AI-generated: Reads the file one record at a time with fscanf.
     * The %256[^,] width-bounded scanset prevents buffer overflow on
     * long names. If fscanf stops matching before EOF the line is
     * malformed: close and return 0. Records successfully added before
     * a bad line stay in the DB (documented in README). */
    FILE *f = fopen(path, "r");
    if (f == NULL) return 0;
    int    id;
    char   name[257];
    double gpa;
    int    matched;
    while ((matched = fscanf(f, " %d,%256[^,],%lf", &id, name, &gpa)) == 3) {
        if (!DB_add(db, id, name, gpa)) {
            fclose(f);
            return 0;
        }
    }
    if (matched != EOF) {   /* stopped on a bad line, not clean EOF */
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}
