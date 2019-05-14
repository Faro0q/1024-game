#include <limits.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <ctype.h>
#include <string.h>

#define MERGE_MASK INT_MAX - 1
#define GRID_SIZE 4
#define TILE_COUNT GRID_SIZE *GRID_SIZE
#define INITAL_POPULATED_TILES 2
#define STARTING_VALUES_SIZE 2
using namespace std;

// direction flags
static const int INVALID = 0, LEFT = 1, UP = 2, RIGHT = 3, DOWN = 4;
static const int EMPTY = 0;
static const int MERGED_FLAG = 0x01;
static const int STARTING_VALUES[STARTING_VALUES_SIZE] = {2, 4};

//--------------------------------------------------------------------
// Display Instructions
void displayInstructions() {
  cout << "Welcome to 1024. \n"
       << "  \n"
       << "For each move enter a direction as a letter key, as follows: \n"
       << "    W    \n"
       << "  A S D  \n"
       << "where A=left,W=up, D=right and S=down. \n"
       << "  \n"
       << "After a move, when two identical valued tiles come together they    "
          "\n"
       << "join to become a new single tile with the value of the sum of the   "
          "\n"
       << "two originals. This value gets added to the score.  On each move    "
          "\n"
       << "one new randomly chosen value of 2 or 4 is placed in a random open  "
          "\n"
       << "square.  User input of x exits the game.                            "
          "\n"
       << "\n"
       << "Game ends when you reach 1024."
       << "\n"
       << "\n";

}  // end displayInstructions()

/*
 * Generates the next number to populate an empty tile with a starting value
 */
// *** Arvan: Use camelCase for variable and function names.
static int next_number() {
  int selected_index = rand() % STARTING_VALUES_SIZE;
  return STARTING_VALUES[selected_index];
}
/*
 * Generates index of next empty tile in board
 */
// *** Arvan: Use camelCase for variable and function names.
static int empty_index(unsigned int *board, size_t board_size) {
  int index;
  do {
    index = rand() % board_size;
  } while (board[index] != 0);
  return index;
}
/*
 * Populates an empty tile with a starting value
 */
// *** Arvan: Use camelCase for variable and function names.
static void populate_random_empty_tile(unsigned int *board, size_t board_size) {
  int pieceToPlace = next_number();
  int index = empty_index(board, board_size);
  board[index] = pieceToPlace;
}

/*
 * Gets the direction from WASD character choice
 */
// *** Arvan: Use camelCase for variable and function names.
static u_int8_t get_direction(char letter) {
  switch (letter) {
    case 'A':
      return LEFT;
    case 'W':
      return UP;
    case 'D':
      return RIGHT;
    case 'S':
      return DOWN;
    default:
      return INVALID;
  }
}

static void display_board(unsigned int *board, size_t board_size) {
  cout << "\n";
  for (int row = 0; row < GRID_SIZE; row++) {
    cout << "   ";
    for (int col = 0; col < GRID_SIZE; col++) {
      int current =
          row * GRID_SIZE + col;  // 1-d index corresponding to row & col
      cout << setw(
          6);  // set output field size to 6 (Requires #include <iomanip> )
      // display '.' if board value is 0
      if (board[current] == 0) {
        cout << '.';
      } else {
        cout << board[current];
      }
    }
    cout << "\n\n";
  }
}

static void prepare_start_game(unsigned int *board, size_t board_size,
                               int populated_tiles) {
  for (int i = 0; i < populated_tiles; i++) {
    populate_random_empty_tile(board, board_size);
  }
}

/**
 * this function takes all merged values and resets to original value
 */
static void recalibrate(unsigned int *board, size_t board_size) {
  for (int i = 0; i < board_size; i++) {
    board[i] &= MERGE_MASK;
  }
}

int one_dimension_index(int i, int j, int length_row) {
  return i * length_row + j;
}

void rotate90Clockwise(unsigned int *board) 
{ 
    for (int i = 0; i < GRID_SIZE / 2; i++) { 
        for (int j = i; j < GRID_SIZE - i - 1; j++) { 
          int temp = board[one_dimension_index(i,j,GRID_SIZE)]; 
            board[one_dimension_index(i,j,GRID_SIZE)] = board[one_dimension_index(GRID_SIZE - 1 - j,i,GRID_SIZE)]; 
            board[one_dimension_index(GRID_SIZE - 1 - j,i, GRID_SIZE)] = board[one_dimension_index(GRID_SIZE - 1 - i,GRID_SIZE - 1 - j, GRID_SIZE)]; 
            board[one_dimension_index(GRID_SIZE - 1 - i,GRID_SIZE - 1 - j, GRID_SIZE)] = board[one_dimension_index(j,GRID_SIZE - 1 - i, GRID_SIZE)]; 
            board[one_dimension_index(j,GRID_SIZE - 1 - i, GRID_SIZE)] = temp; 
        } 
    } 
} 

static int move_pieces_left(unsigned int *board, size_t board_size) {
  int score = 0;
  for (int row = 0; row < GRID_SIZE; row++) {
    int starting_row_index = row * GRID_SIZE;
    int ending_row_index = starting_row_index + GRID_SIZE;
    // loop through row
    int empty_row_index = -1;
    for (int i = starting_row_index; i < ending_row_index; i++) {
      int tile_value = board[i];
      // if slot empty in tile
      if (tile_value == EMPTY) {
        // keep index if previous value is not empty (bounds check)
        if (i == starting_row_index) {
          empty_row_index = i;
          continue;
        }
        int prev_value = board[i - 1];
        // they're both empty keep original empty row index
        if (prev_value != tile_value) {
          empty_row_index = i;
        }
      } else {  // tile not empty
        if (i == starting_row_index) {
          continue;
        }
        // we have an empty value, merge with previous value
        int value_index_to_check = i - 1;
        if(empty_row_index > 0 && empty_row_index < i) {
          value_index_to_check = empty_row_index-1;
        }
        int prev_value = board[value_index_to_check];
        // if match - add to previous value
        if (prev_value == tile_value) {
          int updated_value = prev_value * 2;
          board[value_index_to_check] = updated_value + 1;
          score += updated_value;
          board[i] = EMPTY;
          empty_row_index = i;
        } else {
          if (empty_row_index == -1) {
            continue;
          }
          board[empty_row_index] = tile_value;
          board[i] = EMPTY;
          i = empty_row_index;
        }
      }
    }
  }
  recalibrate(board, board_size);
  return score;
}

static int move_pieces_right(unsigned int *board, size_t board_size) {
  int score = 0;
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  score += move_pieces_left(board, board_size);
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  return score;
}

static int move_pieces_top(unsigned int *board, size_t board_size) {
  int score = 0;
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  score += move_pieces_left(board, board_size);
  rotate90Clockwise(board);
  return score;
}

static int move_pieces_bottom(unsigned int *board, size_t board_size) {
  int score = 0;
  rotate90Clockwise(board);
  score += move_pieces_left(board, board_size);
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  rotate90Clockwise(board);
  return score;
}
//--------------------------------------------------------------------
// Make a copy of the board.  This is used after an attempted move
// to see if the board actually changed.
void copyBoard(
       unsigned int previousBoard[], // destination for board copy
       unsigned int board[],         // board from which copy will be made
       int squaresPerSide)       // size of the board
{
    for( int row=0; row<squaresPerSide; row++) {
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            previousBoard[ current] = board[ current];
        }
    }
}//end copyBoard()

// 1 is valid still / -1 is invalid
int test_board(unsigned int *board) {
  for(int i = 0; i < TILE_COUNT; i++) {
    if(board[i] == 0) {
      return 1;
    }
  }
  unsigned int newBoard[TILE_COUNT];
  copyBoard(newBoard,board,GRID_SIZE);
  move_pieces_left(newBoard,TILE_COUNT);
  move_pieces_bottom(newBoard,TILE_COUNT);
  for(int i = 0; i < TILE_COUNT; i++){
    if(board[i] != newBoard[i]) {
      return 1;
    }
  }

  for(int i = 0; i < TILE_COUNT; i++) {
    if(board[i] = 1024) {
      return 100;
    }
  }
  return -1;
}

int main(void) {
  // srand(time(NULL));
  unsigned int game_board[TILE_COUNT] = {0};
  int move_counter = 1, score = 0;

  prepare_start_game(game_board, TILE_COUNT, INITAL_POPULATED_TILES);
  displayInstructions();
  printf("Score: %d\n\n", score);

  display_board(game_board, TILE_COUNT);

  char character_input;

  cout << endl << move_counter << ". Your move: ";
  cin >> character_input;
  char normalized_input = toupper(character_input);
  while (true) {
    if(normalized_input == 'P') {
      int replace_index = 0,
          replaced_value = 0;
      cin >> replace_index
          >> replaced_value;
      game_board[replace_index] = replaced_value;
      printf("Score: %d\n", score);
      display_board(game_board, TILE_COUNT);
    // character_input = reask for value
    cout << move_counter << ". Your move: ";

    cin >> character_input;
    normalized_input = toupper(character_input);
    continue;
    }
    if(normalized_input == 'X') {
      cout << endl << "Thanks for playing. Exiting program... \n\n";
      break;
    }
    u_int8_t direction = get_direction(normalized_input);
    if (direction == INVALID) {
      cout << "Invalid input, please retry.";
    } else {
      switch (direction) {
        case LEFT:
          score += move_pieces_left(game_board, TILE_COUNT);
          break;
        case RIGHT:
          score += move_pieces_right(game_board, TILE_COUNT);
          break;
        case DOWN:
          score += move_pieces_bottom(game_board, TILE_COUNT);
          break;
        case UP:
          score += move_pieces_top(game_board, TILE_COUNT);
          break;
      }
      move_counter++;
      if(test_board(game_board) == -1) {
            cout << "\n"
              << "No more available moves.  Game is over.\n"
              << "\n";
      }
    }
    // check if full board
    //     if(new_number_position(game_board,TILE_COUNT) == -1) {
    //       printf("Board is full, 1024 not reached");
    //     }
    populate_random_empty_tile(game_board, TILE_COUNT);
    printf("Score: %d\n", score);
    display_board(game_board, TILE_COUNT);
    // character_input = reask for value
    cout << move_counter << ". Your move: ";

    cin >> character_input;
    normalized_input = toupper(character_input);
  }
  return 0;
}
