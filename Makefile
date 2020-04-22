CC = gcc
CFLAGS = -Wall
DEPS = game.h
OBJ = game_cli.o game.o
SOCKET_MACRO = -DSOCKET_V

all: sockets socketless

sockets: game-server game-client

socketless: game-socketless

game-server: game-server.c game.c socket.c
	$(CC) -o $@ $(SOCKET_MACRO) $^ -fsanitize=address

game-client: game-client.c game.c socket.c 
	$(CC) -o $@ $(SOCKET_MACRO) $^ -fsanitize=address

game-socketless: game-socketless.c game.c
	$(CC) -o $@ $^ -fsanitize=address

%.o: %.c
	gcc ${CFLAGS} -c $<

clean:
	rm *.o game-server game-client game-socketless
