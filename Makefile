CC = gcc
CFLAGS = -Wall
DEPS = game.h
OBJ = game_cli.o game.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

game_cli_client.o: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm *.o
