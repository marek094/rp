CC = g++
FLAGS = -Wall -std=c++11 -Ofast 

main: main.o simple_avoiders.o
	$(CC) -o main main.o simple_avoiders.o

tests: tests.o simple_avoiders.o
	$(CC) -o tests tests.o simple_avoiders.o

clean:
	@-rm main.o simple_avoiders.o main ||: 


main.o: main.cpp simple_avoiders.hpp
	$(CC) $(FLAGS) -c main.cpp

tests.o: tests.cpp simple_avoiders.hpp
	$(CC) $(FLAGS) -c tests.cpp

simple_avoiders.o: simple_avoiders.cpp simple_avoiders.hpp
	$(CC) $(FLAGS) -c simple_avoiders.cpp


