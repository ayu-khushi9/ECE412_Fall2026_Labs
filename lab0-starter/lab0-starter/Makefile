CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Werror -g
TARGET   := stats
SRC      := stats.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	@echo "--- sample.txt (expect Count: 5 / Min: 62 / Max: 95 / Mean: 79.40) ---"
	./$(TARGET) < sample.txt
	@echo "--- empty input (expect Count: 0 and nothing else) ---"
	./$(TARGET) < /dev/null

clean:
	rm -f $(TARGET)

.PHONY: all test clean
