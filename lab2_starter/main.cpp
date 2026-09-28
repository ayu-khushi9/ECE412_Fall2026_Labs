/*
 * ================================================================
 * Filename:    main.cpp
 * Description: ECE 412 Lab 2 -- driver. Exercises the ADT through
 *              the six numbered activities from the handout. This
 *              whole program must be valgrind-clean, and must NEVER
 *              touch db->anything -- if you cannot spell out the
 *              struct's fields, that is the ADT working correctly.
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

int main(void) {
    /* ---- 1. Create + grow ----------------------------------------------
     * TODO: DB_create(4). Add >= 20 records (id, "name<i>", gpa) so the
     * array doubles at least twice (4 -> 8 -> 16 -> 32). After each add,
     * or every few adds, print DB_size and DB_capacity so the two
     * doublings are visible in the output.
     */

    /* AI-generated: Creates the DB with initial capacity 4 and adds 22
     * records to force three doublings (cap goes 4->8 at the 5th add,
     * 8->16 at the 9th, 16->32 at the 17th). Size and capacity are
     * printed after every add so each growth event is clearly visible. */
    DB *db = DB_create(4);
    if (db == NULL) {
        printf("DB_create failed\n");
        return 1;
    }

    const char *names[] = {
        "Alice",  "Bob",    "Carol",  "Dave",  "Eve",
        "Frank",  "Grace",  "Hank",   "Iris",  "Jack",
        "Karen",  "Leo",    "Mona",   "Ned",   "Olive",
        "Paul",   "Quinn",  "Rose",   "Sam",   "Tara",
        "Uma",    "Vera"
    };
    double gpas[] = {
        3.9, 2.5, 3.7, 3.1, 2.8,
        3.5, 4.0, 1.9, 3.3, 2.7,
        3.6, 2.2, 3.8, 3.0, 2.4,
        3.4, 1.7, 3.2, 2.9, 3.6,
        3.1, 2.6
    };

    int num_records = 22;
    for (int i = 0; i < num_records; i++) {
        DB_add(db, i + 1, names[i], gpas[i]);
        printf("After add id=%d: size=%zu cap=%zu\n",
               i + 1, DB_size(db), DB_capacity(db));
    }
    printf("\n");

    /* ---- 2. Find present + absent --------------------------------------
     * TODO: DB_find an id you added (print id/name/gpa from the returned
     * const Record *). DB_find an id you did NOT add (print "not found").
     * Note that you never write through the pointer -- it is const on
     * purpose; explain why in your README.
     */

    /* AI-generated: Calls DB_find on id=5 (present) and id=999 (absent).
     * We only READ through the returned const pointer -- never assign to
     * its fields -- which is enforced by the type system (see README). */
    const Record *r = DB_find(db, 5);
    if (r != NULL) {
        printf("Found:    id=%d name=%s gpa=%.2f\n", r->id, r->name, r->gpa);
    }
    r = DB_find(db, 999);
    printf("id=999:   %s\n\n", (r == NULL) ? "not found" : "found (ERROR)");

    /* ---- 3. Remove first, middle, last ---------------------------------
     * TODO: DB_remove the first id, DB_remove a middle id, DB_remove the
     * last id. After each removal call DB_print to show the DB. Removing
     * from the middle without leaving a hole is the interesting case --
     * your README states whether you shift the tail down or swap the
     * last element into the gap.
     */

    /* AI-generated: Removes the first inserted record (id=1), a middle
     * record (id=11), and the last inserted record (id=22). DB_print
     * after each removal shows the shift-down closing the gap cleanly --
     * no holes appear. See README for the shift-down vs swap-last
     * tradeoff discussion. */
    printf("--- Removing id=1 (first) ---\n");
    DB_remove(db, 1);
    DB_print(db);
    printf("\n");

    printf("--- Removing id=11 (middle) ---\n");
    DB_remove(db, 11);
    DB_print(db);
    printf("\n");

    printf("--- Removing id=22 (last) ---\n");
    DB_remove(db, 22);
    DB_print(db);
    printf("\n");

    /* ---- 4. Duplicate add fails ----------------------------------------
     * TODO: DB_add an id that is still present. Check that the return
     * value is 0 and DB_size did not change.
     */

    /* AI-generated: Tries to insert id=5, which is still in the DB.
     * DB_add must return 0 and the size must be unchanged -- both are
     * printed so the result is visible in the output. */
    size_t before = DB_size(db);
    int    ret    = DB_add(db, 5, "Duplicate", 0.0);
    printf("Duplicate add (id=5): returned %d, size %zu->%zu (must be same)\n\n",
           ret, before, DB_size(db));

    /* ---- 5. Save + destroy + create + load round trip ------------------
     * TODO:
     *   - DB_save(db, "records.txt")
     *   - DB_destroy(db); db = NULL;   // hygiene
     *   - db = DB_create(4)
     *   - DB_load(db, "records.txt")
     *   - DB_print(db) -- must match what you saved
     */

    /* AI-generated: Saves the current DB to records.txt, destroys it
     * (setting the pointer to NULL for hygiene), creates a fresh DB with
     * capacity 4, loads the file back, and prints it. The output must
     * match what was saved -- this is the round-trip check. */
    if (!DB_save(db, "records.txt")) {
        printf("DB_save failed\n");
        DB_destroy(db);
        return 1;
    }
    DB_destroy(db);
    db = NULL;

    db = DB_create(4);
    if (db == NULL) {
        printf("DB_create (reload) failed\n");
        return 1;
    }
    if (!DB_load(db, "records.txt")) {
        printf("DB_load failed\n");
        DB_destroy(db);
        return 1;
    }
    printf("--- After round-trip load from records.txt ---\n");
    DB_print(db);
    printf("\n");

    /* ---- 6. Malformed file fails cleanly -------------------------------
     * TODO: DB_load(db, "bad_records.txt"). Check the return value is 0.
     * The DB must still be usable after this call (no crash, no leak).
     * Then DB_destroy the final DB so the whole program is
     * valgrind-clean.
     */

    /* AI-generated: Writes a bad_records.txt with one valid line
     * followed by a deliberately malformed line, then feeds it to
     * DB_load. The return value must be 0 and the DB must remain usable.
     * The final DB_destroy makes the whole program valgrind-clean. */
    FILE *bad = fopen("bad_records.txt", "w");
    if (bad != NULL) {
        fprintf(bad, "100,TestPerson,3.5\n");
        fprintf(bad, "this line is not valid CSV\n");
        fclose(bad);
    }

    int load_ret = DB_load(db, "bad_records.txt");
    printf("DB_load(bad_records.txt) returned %d (expected 0)\n", load_ret);
    printf("DB still usable -- size=%zu\n", DB_size(db));

    DB_destroy(db);
    db = NULL;

    return 0;
}
