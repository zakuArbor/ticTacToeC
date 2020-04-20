CC = gcc
CFLAGS = -Wall
DEPS = game.h
OBJ = game_cli.o game.o

all: socket socketless

socket: game-server game-client

socketless: game-socketless

game-server: game-server.o game.o
	$(CC) -o $@ $^ -fsanitize=address

game-client: game-client.o game.o
	$(CC) -o $@ $^ -fsanitize=address

game-socketless: game-socketless.o game.o
	$(CC) -o $@ $^ -fsanitize=address

game-server.o: game-server.c socket.h game.o

game-client.o: game-client.c socket.h game.o 

game-socketless.o: game-socketless.c game.o

game.o: game.h game.c

clean:
	rm *.o game-server game-client game-socketless-client
