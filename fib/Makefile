CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= main
OBJS= main.o

main: main.o 
	$(CC) main.o -o main $(LFLAGS)

main.o: main.cpp fibonnaci.h
	$(CC) main.cpp -o main.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)