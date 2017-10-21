# ticTacToeC
A C network Tic Tac Toe 

## PLANS
1. Create Board [ ###### COMPLETED]
  
  + void drawBoard(struct player *player1, struct player *player2, int num_pieces, int *pieces)
2. Make Move [###### COMPLETED]

  + void move(struct player *p, int *pieces) 
3. isWin [###### COMPLETED]
  
  + check if the player wins
 
3. tie [###### COMPLETED]
  + check if there is a tie after all pieces have been placed

3. Reset Game [###### COMPLETED]
  + Reset game after the game finishes

4. Add AI
  
  + use a general breadth search AI
  
5. Add socket
  
  + Add client and server
    * Try using forks
    * Think about using threads if applicable
  
6. GUI
  
  + Use a GUI library such as SDL or Allegro


## ALGORITHMS

1. isWin

  + Possible Combinations of winning: 
  	* Win by Horizontal: [0, 1, 2], [3, 4, 5], [6, 7, 8]
    * Win by Vertical: [0, 3, 6], [1, 4, 7], [2, 5, 8]
    * Win by Diagonal: [0, 4, 7], [2, 4, 6]

  + Problem: 
  	* Space Complexity: If we have a combination of possible winning combinations, it requires space
  	* Can get very messy with nested if statements (Example: <a href = "https://github.com/zakuArbor/FreeCodeCampAssignments/blob/master/ticTacToe/script.js">My previous tic tac toe</a>)

  + Solution:
  	* There is a pattern for winning
  	* Win by Horizontal: 

  	For each row it's row*3, row*3 + 1, row*3 + 2, row E {0, 1, 2}
  	
  	* Win by Vertical: 

  	For each column it's col, col+3, col+6, col E {0, 1, 2}

  	* Decreases the amount of writing needed
