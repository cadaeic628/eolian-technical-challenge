CC = g++

#compile the main file
compile: src/main.cpp
	$(CC) -o src/main.o src/main.cpp
	./src/main.o

