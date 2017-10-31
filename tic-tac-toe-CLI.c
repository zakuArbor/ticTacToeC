#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

typedef struct 
{
	char name[10];
	char piece; //either x or o
	int isHuman; //true(1) for human player, false(0) for computer player
} player;

extern int initGame(player **players, int *moves, int *num_moves, int *player_num);
extern int reset();
extern int isWin(int *moves);
extern int win(player **players, int *moves, int *num_moves, int *player_num);
extern int tied(player **players, int *moves, int *num_moves, int *player_num);
extern int menu();
extern void resetGame(player **players, int *moves, int *num_moves, int *player_num);
extern void drawBoard(player **players, int num_moves, int *moves);
extern void makeMove(player **players, int *num_moves, int *moves, int *player_num);
extern void ai_move(player **players, int *moves, int *player_num, int *num_moves);
extern void player_move(player **players, int *moves, int* player_num, int *num_moves);
extern void set_players(player **players);

int main (int argc, char **argv) {
	player **players;
	int num_moves;
	int player_num;
	int quit = 0;
	int moves[9]; //-1 for empty slot, 0 for player1 and 1 for player2

	printf("Hello World\n");

	if ((players = malloc(sizeof(player*) * 2)) == NULL) {
		perror("malloc");
		return(1);
	}
	
	if (initGame(players, moves, &num_moves, &player_num) != 0) {
		return(1);
	}

	set_players(players);

	while (!quit) {
		drawBoard(players, num_moves, moves);
		if (isWin(moves) != -1) {
			quit = win(players, moves, &num_moves, &player_num);
		}
		else if (num_moves == 9) {
			quit = tied(players, moves, &num_moves, &player_num);		
		}
		else {
			player_move(players, moves, &player_num, &num_moves);
		}
	}
	return(0);
}

/*
* Initializes the players and the board
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
*/
int initGame(player **players, int *moves, int *num_moves, int *player_num) {
	if (!(players[0] = malloc(sizeof(player)))) {
		perror("malloc");
		return(1);
	}

	if (!(players[1] = malloc(sizeof(player)))) {
		perror("malloc");
		return(1);
	}

	players[0]->piece = 'x';
	players[1]->piece = 'o';
	
	resetGame(players, moves, num_moves, player_num);

	return(0);
}

/*
* Reset game if player wishes to replay the game after winning
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
* @return: 0 if the player wishes to quit else 1 if the player wishes to continue playing
*/
int win(player **players, int *moves, int *num_moves, int *player_num) {
	printf("Player %d won!\n", isWin(moves) + 1);
	if (reset()) {
		resetGame(players, moves, num_moves, player_num);
	}			
	else {
		return(1);
	}
	return(0);
}

/*
* Reset game if player wishes to replay the game after the game tied
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
* @return: 0 if the player wishes to quit else 1 if the player wishes to continue playing
*/

int tied(player **players, int *moves, int *num_moves, int *player_num) { 
	printf("Game Tied\n");
	if (reset()) {
		resetGame(players, moves, num_moves, player_num);
	}			
	else {
		return(1);
	}
	return(0);
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
* Resets the game parameters and the board
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
*/
void resetGame(player **players, int *moves, int *num_moves, int *player_num) {
	int i;

	*num_moves = 0;
	*player_num = 0;

	for (i = 0; i < 9; i++) {
		moves[i] = -1;
	}
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

/*
* Returns -1 if no player wins else returns the index of the player who wins
* 
* @param moves: the pieces in the board
*/
int isWin(int *moves) {
	int row, col;

	// Diagonal Row Win
	if (moves[0] != -1 && moves[0] == moves[4] && moves[0] == moves[8]) return (moves[0]);
	if (moves[2] != -1 && moves[2] == moves[4] && moves[2] == moves[6]) return (moves[2]);

	// Horizontal Row Win
	for (row = 0; row < 3; row++) 
		if (moves[row*3] != -1 && moves[row*3] == moves[row*3+1] && moves[row*3] == moves[row*3+2]) return(moves[row*3]); 
	

	// Vertical Row Win
	for (col = 0; col < 3; col++) 
		if (moves[col] != -1 && moves[col] == moves[col+3] && moves[col] == moves[col+6]) return(moves[col]); 
	
	return -1;
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
		moves[move] = *player_num;
		*player_num = *player_num ^ 1;
		*num_moves += 1;
	}
}

/*
* AI makes move based on:
*	1. Preventing the enemy from winning
*	2. Playing a move to win
*	3. The first empty spot
*
* @param players: an array of 2 player struct
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
* @param num_moves: a reference to the number of moves that has been played
*/

void ai_move(player **players, int *moves, int *player_num, int *num_moves) {
	int i;
	int enemy_num = *player_num ^ 1;

	//stop opponent from winning or choose a move for ai to win
	for (i = 0; i < 9; i++)	{
		if (moves[i] == -1) {
			moves[i] = enemy_num;
			if (isWin(moves) != -1) {
				moves[i] = *player_num;
				break;
			}
			else {
				moves[i] = -1;
			}
		}
	}

	if (moves[i] != *player_num) {
		//pick the first spot to win
		for (i = 0; i < 9; i++) {
			if (moves[i] == -1) {
				moves[i] = *player_num;
				break;
			}
		}
	}
	*player_num ^= 1; //switch player
	*num_moves += 1;
}

/*
* A controller that directs either the ai or player to make a turn
*
* @param players: an array of 2 player struct
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
* @param num_moves: a reference to the number of moves that has been played
*/
void player_move(player **players, int *moves, int *player_num, int *num_moves) {
	if (players[*player_num]->isHuman) 
		makeMove(players, num_moves, moves, player_num);
	else
		ai_move(players, moves, player_num, num_moves);
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