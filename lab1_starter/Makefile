# ECE 412 Lab 1 -- Pointers & Dynamic Memory
# Required targets: all, test, clean (the autograder uses them).

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Werror -g

# Which growth strategy your toolkit binary uses — same name as the #define
# in intarray.cpp, one knob, one name. 1 = realloc, 0 = malloc+copy+free.
#     make                          # realloc version
#     make GROW_WITH_REALLOC=0     # manual version
# (The autograder builds and tests BOTH versions regardless.)
GROW_WITH_REALLOC ?= 1

SRC  := mystr.cpp intarray.cpp main.cpp
HDRS := mystr.h intarray.h

all: toolkit

toolkit: $(SRC) $(HDRS)
	$(CXX) $(CXXFLAGS) -DGROW_WITH_REALLOC=$(GROW_WITH_REALLOC) $(SRC) -o toolkit

test: toolkit
	./toolkit

# The graded check. Paste the summary block into your WRITEUP.
valgrind: toolkit
	valgrind --leak-check=full --show-leak-kinds=all ./toolkit

clean:
	rm -f toolkit

.PHONY: all test valgrind clean
