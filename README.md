# 1024-game

Write a C++ program to play a text-based version of the game of 1024, which is the same idea as the game of 2048, which itself is a copy of the (better) game of threes.   When the program starts it defaults to a board size of 4, but later you can resize the board and restart.  The goal for a 4x4 board is to get a tile with the value 1024.  Each additional increment in board size doubles the goal value, so a 5x5 board would have a goal of 2048, a 6x6 board would have a goal of 4096, and so on.  Running this program should look like:
Welcome to 1024.

For each move enter a direction as a letter key, as follows:
    W
  A S D
where A=left,W=up, D=right and S=down.

After a move, when two identical valued tiles come together they
join to become a new single tile with the value of the sum of the
two originals. This value gets added to the score.  On each move
one new randomly chosen value of 2 or 4 is placed in a random open
square.  User input of x exits the game.

Game ends when you reach 1024.
