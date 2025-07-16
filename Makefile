CXX = g++


#compile the main file
compile: src/main.cpp
	touch /tmp/frames.fifo
	$(CXX) -std=c++20 -O2 -Wno-narrowing -o src/main.o src/main.cpp
	./src/main.o < /tmp/frames.fifo

