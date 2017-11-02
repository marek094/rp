CC = g++
FLAGS = -Wall -std=c++14 -Ofast -march=native

main: main.o
	$(CC) -o main main.o

tests: tests.o
	$(CC) -o tests tests.o

clean:
	@-rm main.o main ||:

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

tests.o: tests.cpp
	$(CC) $(FLAGS) -c tests.cpp
