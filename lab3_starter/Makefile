# ------------------------------------------------------------------- #
# ECE 412 Lab 3 -- multi-file build with per-object rules.
#
# Targets you will use:
#   make            build ./inventory
#   make test       build and run your own tests (main.cpp)
#   make check      build and run the provided suite (tests/)
#   make valgrind   run ./inventory under valgrind, as we grade it
#   make vcheck     run ./public_tests under valgrind
#   make clean      remove everything built
# ------------------------------------------------------------------- #

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -g
VG       = valgrind --leak-check=full --show-leak-kinds=all \
           --error-exitcode=1

TARGET   = inventory
OBJS     = main.o list.o mystr.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# One rule per object, with its real header dependencies, so that
# touching list.h rebuilds exactly what depends on list.h.
main.o: main.cpp list.h
	$(CXX) $(CXXFLAGS) -c main.cpp

list.o: list.cpp list.h mystr.h
	$(CXX) $(CXXFLAGS) -c list.cpp

mystr.o: mystr.cpp mystr.h
	$(CXX) $(CXXFLAGS) -c mystr.cpp

# The provided suite lives in tests/ and links against your objects.
public_tests: tests/public_tests.cpp list.o mystr.o list.h
	$(CXX) $(CXXFLAGS) -I. -o public_tests tests/public_tests.cpp \
	    list.o mystr.o

test: $(TARGET)
	./$(TARGET)

check: public_tests
	./public_tests

valgrind: $(TARGET)
	$(VG) ./$(TARGET)

vcheck: public_tests
	$(VG) ./public_tests

clean:
	rm -f $(OBJS) $(TARGET) public_tests

.PHONY: all test check valgrind vcheck clean
