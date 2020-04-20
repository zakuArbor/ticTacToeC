#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "game.h"

extern int isWin(int *moves);
extern int isReset(player **players, int *moves, int *num_moves, int *player_num);
extern int menu();
extern void resetGame(player **players, int *moves, int *num_moves, int *player_num);
extern void drawBoard(player **players, int num_moves, int *moves);
extern void set_players(player **players);
extern int reset();

int main (int argc, char **argv) {
	player **players;
	int num_moves;
	int player_num;
	int quit = 0;
	int moves[9]; //-1 for empty slot, 0 for player1 and 1 for player2

	printf("Hello World\n");
	
	if (!(players = initGame(moves, &num_moves, &player_num))) {
		goto terminate;
	}

	set_players(players);

	while (!quit) {
		drawBoard(players, num_moves, moves);
		if (isWin(moves) != -1) {
			printf("Player %d won!\n", isWin(moves) + 1);
			quit = isReset(players, moves, &num_moves, &player_num);
		}
		else if (num_moves == 9) {
			printf("Game Tied\n");
			quit = isReset(players, moves, &num_moves, &player_num);		
		}
		else {
			player_move(players, moves, &player_num, &num_moves);
		}
	}
terminate:
	free_players(players);
	return(0);
}

/*
* Set players to their respective role based on game option
* @param players: an array of 2 player struct
*/
void set_players(player **players) {
	switch(menu()) {
		case 0:
			players[0]->isHuman = true;
			players[1]->isHuman = true;
			break;
		case 1:
			players[0]->isHuman = true;
			players[1]->isHuman = false;	
			break;
		case 2:
			players[0]->isHuman = false;
			players[1]->isHuman = true;
			break;
	}
}

/*
* Allow player to make a move
*
* @param players: an array of 2 player struct
* @param num_moves: a reference to the number of moves that has been played
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
*/
void makeMove(player **players, int *num_moves, int *moves, int *player_num) {
	int move;
	char s[2];

	printf("Player %d Turn: ", *player_num + 1);
	scanf("%s", s);
	move = strtol(s, NULL, 10);

	if (move >= 0 && move < 9 && moves[move] == -1) {
		write_move(num_moves, moves, player_num, move);
	}
}



/*
* Reset game if player wishes to replay the game
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
* @return: 1 if the player wishes to quit else return 0 if the player wishes to continue playing
*/
int isReset(player **players, int *moves, int *num_moves, int *player_num) {
	if (reset()) {
		resetGame(players, moves, num_moves, player_num);
		return(0);
	}			
	return(1);
}

/*
* Display menu and return:
* 	0: If multiplayer
*	1: If single player and player goes first
*	2: If single player and ai goes first
*/
int menu() {
	int option;
	char s[2];
	char c = 'i'; //some default value that is not 'y' nor 'n'

	printf("Menu\n");
	printf("1. Single Player\n");
	printf("2. Multi Player\n");
	printf("Please input either 1 or 2: ");
	scanf("%s", s);
	option = strtol(s, NULL, 10);

	while (strcmp(s, "1") && strcmp(s, "2")) {
		printf("Invalid option! Please input either 1 or 2: ");
		scanf("%s", s);	
	}
	option = strtol(s, NULL, 10);

	if (option == 1) {
		while (tolower(c) != 'y' && tolower(c) != 'n') {
			printf("Do you wish to go first?[Y/n]: ");
			scanf(" %c", &c); //extra space in format specifier allows input to ignore leading whitespace chars (including newline)
		}
		if (c == 'y') {
			return(1); 
		}
		return(2);
	}
	return(0);
}

/*
* Return 1 iff user wants to reset game, else return 0
*/
int reset() {
	char s[1];
	printf("Would you like to reset the game? Enter 1 to reset the game: ");
	scanf("%s", s);
	if (strtol(s, NULL, 10) == 1) {
		return(1);
	}
	return(0);
}

/*
* Draws the game board
*
* @param players: an array of 2 player struct
* @param num_moves: number of moves that has been played
* @param moves: a reference to a list of player's moves
*/
void drawBoard(player **players, int num_moves, int *moves) {
	int row, col;
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 5; col++) {
			if (col % 2 == 0) { //a slot
				if (num_moves > 0 && moves[(row*3) + (col/2)] != -1) { //if there is a piece placed
					printf("%c", players[moves[(row*3) + (col/2)]]->piece);
				}
				else {
					printf(" ");
				}
			} 
			else { //a border
					printf("|");
			}
		}
		if (row < 2) printf("\n-----\n");
	}
	printf("\n\n");
}
