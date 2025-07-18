CXX=g++
CXXFLAGS=-std=c++20 -O2 -Wall -Wextra
TARGET=can_writer

$(TARGET): can_writer.cpp
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: clean

