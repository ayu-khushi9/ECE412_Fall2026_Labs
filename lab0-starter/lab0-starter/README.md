# Lab 0 — Pipe Cleaner

**Name: Ayushi Priyadarshani**
**EID: ap62929**

Replace the placeholders below with your own answers, then delete this line.

---

## Which server did you use?

ap62929@mario.ece.utexas.edu server

## What inputs did you test?

I tested ./stats < sample.txt and got Count: 5 / Min: 62 / Max: 95 / Mean: 79.40, matching the expected output. I also tested ./stats < /dev/null (empty input), which printed Count: 0 and nothing else, as expected there was no crash and no division-by-zero on the mean.

## Did anything go wrong on the way?


I was confused on the fact that I had to clone the repo inside the server and how I was supposed to click ont eh arrow ont he bottom left corner to switch to the server.

---

## Checklist before you submit

- [ ] `make` runs with **zero warnings** on the ECE server
- [ ] `./stats < sample.txt` prints `Count: 5 / Min: 62 / Max: 95 / Mean: 79.40`
- [ ] `./stats < /dev/null` prints `Count: 0` and nothing else
- [ ] No global variables; `main` only orchestrates
- [ ] The file header block at the top of `stats.cpp` is filled in
- [ ] Any AI-assisted code carries a citation comment
- [ ] Submitted `stats.cpp`, `Makefile`, and `README.md` to **Gradescope**
- [ ] Opened the Gradescope submission and confirmed the autograder ran

That last box is the actual point of this lab.
