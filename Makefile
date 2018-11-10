CC = gcc
CFLAGS = -Wall
DEPS = game.h
OBJ = game_cli.o game.o

all: game-server game-client game_cli_client

game-server: game-server.o game.o
	$(CC) -o $@ $^

game-client: game-client.o game.o
	$(CC) -o $@ $^

game_cli_client: game_cli.o game.o
	$(CC) -o $@ $^

game-server.o: game-server.c socket.h game.o

game-client.o: game-client.c socket.h game.o

game_cli.o: game_cli.c game.o

game.o: game.h game.c

clean:
	rm *.o
