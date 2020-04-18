CC = gcc
CFLAGS = -Wall
DEPS = game.h
OBJ = game_cli.o game.o

all: game-server game-client game-socketless-client

game-server: game-server.o game.o
	$(CC) -o $@ $^

game-client: game-client.o game.o
	$(CC) -o $@ $^

game-socketless-client: game-socketless.o game.o
	$(CC) -o $@ $^ -fsanitize=address

game-server.o: game-server.c socket.h game.o

game-client.o: game-client.c socket.h game.o

game-socketless.o: game-socketless.c game.o

game.o: game.h game.c

clean:
	rm *.o game-server game-client game-socketless-client
