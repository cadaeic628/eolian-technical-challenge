CXX = g++


#compile the main file
compile: src/main.cpp
	$(CXX) -std=c++20 -O2 -o src/main.o src/main.cpp
	./src/main.o

