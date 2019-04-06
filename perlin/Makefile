CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= Server_main run_perlin
OBJS= Server_main.o digraph.o serialport.o dijkstra.o

all: Server_main

Server_main: Server_main.o digraph.o serialport.o dijkstra.o
	$(CC) Server_main.o digraph.o serialport.o dijkstra.o -o Server_main $(LFLAGS)

Server_main.o: Server_main.cpp digraph.cpp digraph.h wdigraph.h astar.h server_util.h fibonnaci.h pointData.h
	$(CC) Server_main.cpp -o Server_main.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

serialport.o: serialport.cpp serialport.h
	$(CC) serialport.cpp -o serialport.o $(CFLAGS)

perlin: perlin.cpp perlin.h
	$(CC) perlin.cpp -o run_perlin -Wall -std=c++11

dijkstra.o: dijkstra.cpp dijkstra.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)