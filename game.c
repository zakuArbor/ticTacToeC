#include "game.h"

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
* Move player
*
* @param num_moves: a reference to the number of moves that has been played
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
* @param move: slot where the player wishes to move
*/
void write_move(int *num_moves, int *moves, int *player_num, int move) {
	moves[move] = *player_num;
	*player_num = *player_num ^ 1;
	*num_moves += 1;
}

/*
* Find if the player can win with the current board configuration
*
* @param: moves: a reference to a list of moves that has been made so far
* @param player_num: an integer that represents the user that is making the move
*/
int move_to_win(int *moves, int player_num) {
	int win_spot = -1; //default: -1 to indicate no winning spot for the player
	int i;

	for (i = 0; i < 9; i++)	{
		if (moves[i] == -1) {
			moves[i] = player_num;
			if (isWin(moves) != -1) {
				moves[i] = -1; //reset tile
				win_spot = i;
				break;
			}
			else {
				moves[i] = -1;
			}
		}
	}

	return(win_spot);
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
	int i, win_slot;
	int enemy_num = *player_num ^ 1;

	if ((win_slot = move_to_win(moves, *player_num)) != -1 || (win_slot = move_to_win(moves, enemy_num)) != -1) {
		//First clause checks if the AI can win
		//Second clause checks if the AI can stop player from winning
		//Only one of the two clauses will be evaluated to be true due to short circuit if the boolean expression evaluates to true
		moves[win_slot] = *player_num;
	}
	else { //pick the first empty slot
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