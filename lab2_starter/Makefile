# ECE 412 Lab 2 -- Structs, Opaque Types & a Record ADT
# Required targets: all, test, clean (the autograder uses them).
#
# This lab is graded on SEPARATE COMPILATION: each .cpp becomes a .o,
# and the final link stitches them together. Editing main.cpp must
# not rebuild db.cpp. That is why the rules below name .o files.

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Werror -g

OBJS := mystr.o db.o main.o
BIN  := db

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(BIN)

# Per-object rules. Each .o depends only on its own .cpp plus the
# headers it #includes -- editing main.cpp does NOT rebuild db.o.
mystr.o: mystr.cpp mystr.h
	$(CXX) $(CXXFLAGS) -c mystr.cpp -o mystr.o

db.o: db.cpp db.h mystr.h
	$(CXX) $(CXXFLAGS) -c db.cpp -o db.o

main.o: main.cpp db.h mystr.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

test: $(BIN)
	./$(BIN)

# The graded check. Paste the summary block into your WRITEUP.md.
valgrind: $(BIN)
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN)

clean:
	rm -f $(BIN) $(OBJS)

.PHONY: all test valgrind clean
