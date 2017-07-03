CC = g++
FLAGS = -Wall -std=c++14 -Ofast

main: main.o simple_avoiders.o
	$(CC) -o main main.o simple_avoiders.o

tests: tests.o simple_avoiders.o
	$(CC) -o tests tests.o simple_avoiders.o

clean:
	@-rm main.o simple_avoiders.o main ||: 


main.o: main.cpp simple/simple_avoiders.hpp
	$(CC) $(FLAGS) -c main.cpp

tests.o: tests.cpp simple/simple_avoiders.hpp
	$(CC) $(FLAGS) -c tests.cpp

simple_avoiders.o: simple/simple_avoiders.cpp simple/simple_avoiders.hpp
	$(CC) $(FLAGS) -c simple/simple_avoiders.cpp


