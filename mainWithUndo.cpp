//  prog5_1024.cpp
//     Text and graphical version of the game 1024, a variant of 2048 available online at:
//        http://gabrielecirulli.github.io/2048/
//     Object of game is to combine number tiles with the same value, accumulating
//     points as the game progresses, to try and create the tile with the value 1024.
//     Each move consists of sliding pieces to the left, up, right or down.
//
// Instructions:
//    To run the graphical version, first select the "Build and Run" option at the top of the window. You
//    can ignore the following error messages that will appear:
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to initialize inotify, joystick connections and disconnections won't be notified
//    To see the graphical output then select the "Viewer" option at the top of the window.
//
// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output.
//
//
/*  Running the program (for the textual output) looks like:

*/

#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
                             // in front of every cin, cout, endl, setw, and string
#include <iomanip>           // used for setting output field size using setw
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen()
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)
#include <cstdlib>

const int WindowXSize = 400;
const int WindowYSize = 500;
const int MaxBoardSize = 12; // Max number of squares per side
const int MaxTileStartValue = 1024;

using namespace std;
//---------------------------------------------------------------------------------------
class Square
{
  public:
    // Default Constructor
    Square()
    {
        size = 0;
        xPosition = 0;
        yPosition = 0;
        color = sf::Color::Black;
        isVisible = false;
        isCaptured = false;
        text = "";
    }

    // Fully-qualified constructor, used to set all fields
    Square(int theSize, int theXPosition, int theYPosition,
           const sf::Color &theColor, bool theVisibility, std::string theText)
    {
        // Set the class fields
        size = theSize;
        xPosition = theXPosition;
        yPosition = theYPosition;
        color = theColor;
        isVisible = theVisibility;
        isCaptured = false; // By default squares have not been captured
        text = theText;
        // Use the values to set the display characteristics of theSquare
        theSquare.setSize(sf::Vector2f(theSize, theSize));
        theSquare.setPosition(theXPosition, theYPosition); // Set the position of the square
        theSquare.setFillColor(theColor);
    }

    // Get (accessor) functions
    sf::RectangleShape getTheSquare() { return theSquare; }
    int getSize() { return size; }
    int getXPosition() { return xPosition; }
    int getYPosition() { return yPosition; }
    sf::Color &getColor() { return color; }
    bool getIsVisible() { return isVisible; }
    bool getIsCaptured() { return isCaptured; }
    std::string getText() { return text; }

    // Set (mutator) functions
    void setSize(int theSize)
    {
        size = theSize;
        theSquare.setSize(sf::Vector2f(theSize, theSize));
    }
    void setXPosition(int theXPosition)
    {
        xPosition = theXPosition;
        theSquare.setPosition(theXPosition, yPosition); // Set the position of the square
    }
    void setYPosition(int theYPosition)
    {
        yPosition = theYPosition;
        theSquare.setPosition(xPosition, theYPosition); // Set the position of the square
    }
    void setColor(sf::Color &theColor)
    {
        color = theColor;
        theSquare.setFillColor(theColor); // Also update the color on the square itself
    }
    void setColor(int R, int G, int B)
    {
        sf::Color theNewColor(R, G, B);
        color = theNewColor;
        theSquare.setFillColor(theNewColor);
    }
    void setVisibility(bool theVisibility) { isVisible = theVisibility; }
    void setIsCaptured(bool isCaptured) { this->isCaptured = isCaptured; }
    void setText(std::string theText) { text = theText; }

    // Utility functions
    void displayText(sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);

  private:
    int size;
    int xPosition;
    int yPosition;
    sf::Color color;
    bool isVisible;
    bool isCaptured; // Indicates whether or not it is part of the captured area
    std::string text;
    sf::RectangleShape theSquare;

}; //end class Square

//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text
// associated with this Square.
//
// Assuming we display output in sf::RenderWindow window(...), then call this function using:
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText(
    sf::RenderWindow *pWindow, // The window into which we draw everything
    sf::Font theFont,          // Font to be used in displaying text
    sf::Color theColor,        // Color of the font
    int textSize)              // Size of the text to be displayed
{
    // Create a sf::Text object to draw the text, using a sf::Text constructor
    sf::Text theText(text,      // text is a class data member
                     theFont,   // font from a font file, passed as a parameter
                     textSize); // this is the size of text to be displayed

    // Text color is the designated one, unless the background is Yellow, in which case the text
    // color gets changed to blue so we can see it, since we can't see white-on-yellow very well
    if (this->getColor() == sf::Color::Yellow)
    {
        theColor = sf::Color::Blue;
    }
    theText.setColor(theColor);

    // Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
    // For horizontal center, find the center of the square and subtract half the width of the text
    int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
    // For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
    int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
    // Use an additional offset to get it centered
    int offset = 5;
    theText.setPosition(theXPosition + offset, theYPosition - offset);

    // Finally draw the Text object in the RenderWindow
    pWindow->draw(theText);
}

//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont(sf::Font &theFont)
{
    // Create the global font object from the font file
    if (!theFont.loadFromFile("arial.ttf"))
    {
        std::cout << "Unable to load font. " << std::endl;
        exit(-1);
    }
}

// Linked list
struct Node
{
    int *data;
    int squaresPerSide;
    int score;
    struct Node *pNext;
};

Node *createNewNode(int *data, int squaresPerSide, int score)
{
    Node *newNode = new Node;
    int *createBoard = (int *) calloc(MaxBoardSize * MaxBoardSize, sizeof(int));
    for(int i = 0; i < MaxBoardSize * MaxBoardSize; i++) {
        createBoard[i] = data[i];
    }
    newNode->data = createBoard;

    newNode->squaresPerSide = squaresPerSide;
    newNode->score = score;
    newNode->pNext = NULL;
    return newNode;
}

Node *getTail(Node *pHead)
{
    Node *pCurrentNode = pHead;
    while (pCurrentNode->pNext != NULL)
    {
        pCurrentNode = pCurrentNode->pNext;
    }
    Node *pTail = pCurrentNode;
    return pTail;
}

void addNode(Node *pHead, Node *pNewNode)
{
    Node *pTail = getTail(pHead);
    pTail->pNext = pNewNode;
}

Node *undoMove(struct Node *pHead)
{
    Node *pTail = getTail(pHead);
    Node *pCurrentNode = pHead;
    if(pTail == pHead) {
        return NULL;
    }
    while (pCurrentNode->pNext != pTail)
    {
        pCurrentNode = pCurrentNode->pNext;
    }
    delete pTail;
    pCurrentNode->pNext = NULL;
    return pCurrentNode;
}


//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "Welcome to 1024. \n"
              << "  \n"
              << "For each move enter a direction as a letter key, as follows: \n"
              << "    W    \n"
              << "  A S D  \n"
              << "where A=left,W=up, D=right and S=down. \n"
              << "  \n"
              << "After a move, when two identical valued tiles come together they    \n"
              << "join to become a new single tile with the value of the sum of the   \n"
              << "two originals. This value gets added to the score.  On each move    \n"
              << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
              << "square.  User input of x exits the game.                            \n"
              << "  \n";
} //end displayInstructions()

//--------------------------------------------------------------------
// Place a randomly selected 2 or 4 into a random open square on
// the board.
void placeRandomPiece(int board[], int squaresPerSide)
{
    // Randomly choose a piece to be placed (2 or 4)
    int pieceToPlace = 2;
    if (rand() % 2 == 1)
    {
        pieceToPlace = 4;
    }

    // Find an unoccupied square that currently has a 0
    int index;
    do
    {
        index = rand() % (squaresPerSide * squaresPerSide);
    } while (board[index] != 0);

    // board at position index is blank, so place piece there
    board[index] = pieceToPlace;
} //end placeRandomPiece()

//--------------------------------------------------------------------
// Prompt for and get board size, dynamically allocate space for the
// board, initialize the board and set the max tile value that
// corresponds to the board size.
void getSizeAndSetBoard(
    int board[],         // Playing board
    int previousBoard[], // Copy of board
    int &squaresPerSide, // size of the board, entered by user
    int &maxTileValue)
{
    // First initialize the array of int values used to represent the Ascii board
    // While board has already been allocated within main to be of max size, here
    //   we initialize the part we will be using to be all 0's
    for (int row = 0; row < squaresPerSide; row++)
    {
        for (int col = 0; col < squaresPerSide; col++)
        {
            board[row * squaresPerSide + col] = 0;
        }
    }

    // Calculate and display game ending value
    maxTileValue = MaxTileStartValue; // Reset the value, in case we resize the board to be smaller
    for (int i = 4; i < squaresPerSide; i++)
    {
        maxTileValue = maxTileValue * 2; // double for each additional board dimension > 4
    }
    std::cout << "Game ends when you reach " << maxTileValue << "." << std::endl;

    // Set two random pieces to start game
    placeRandomPiece(board, squaresPerSide);
    placeRandomPiece(board, squaresPerSide);
} //end getSizeAndSetBoard()

//--------------------------------------------------------------------
// Display the text-based Board
void displayAsciiBoard(int board[], int squaresPerSide, int score)
{
    std::cout << "\n"
              << "Score: " << score << std::endl;
    for (int row = 0; row < squaresPerSide; row++)
    {
        std::cout << "   ";
        for (int col = 0; col < squaresPerSide; col++)
        {
            int current = row * squaresPerSide + col; // 1-d index corresponding to row & col
            std::cout << setw(6);                     // set output field size to 6 (Requires #include <iomanip> )
            // display '.' if board value is 0
            if (board[current] == 0)
            {
                std::cout << '.';
            }
            else
            {
                std::cout << board[current];
            }
        }
        std::cout << "\n\n";
    }
} //end displayBoard()
void displayList(int moveCount)
{
    cout << "     " << "List: ";
    for (int i = 1; i < moveCount; i++)
    {
        std::cout << i << "->";
    }
    std::cout << moveCount;
    std::cout << endl;
}

//--------------------------------------------------------------------
// Make a copy of the board.  This is used after an attempted move
// to see if the board actually changed.
void copyBoard(
       int previousBoard[], // destination for board copy
       int board[],         // board from which copy will be made
       int squaresPerSide)       // size of the board
{
    for( int row=0; row<squaresPerSide; row++) {
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            previousBoard[ current] = board[ current];
        }
    }
}//end copyBoard()
//--------------------------------------------------------------------
// See if board changed this turn. If not, no additional piece
// is randomly added and move number does not increment in main().
// Returns true if boards are different, false otherwise.
bool boardChangedThisTurn(int previousBoard[], int board[], int squaresPerSide)
{
    // Compare element by element.  If one is found that is different
    // then return true, as board was changed.
    for (int row = 0; row < squaresPerSide; row++)
    {
        for (int col = 0; col < squaresPerSide; col++)
        {
            int current = row * squaresPerSide + col; // 1-d index corresponding to row & col
            if (previousBoard[current] != board[current])
            {
                return true;
            }
        }
    }

    return false; // No board difference was found
} //end boardChangedThisTurn(...)

// While the 4 functions below (slideLeft(), slideRight(), slideUp(), slideDown() ) could
// be all combined into a single function, that single function would be difficult to
// understand, so these 4 functions are left separate.

//--------------------------------------------------------------------
// Slide all tiles left, combining matching values, updating the score
void slideLeft(int board[], int squaresPerSide, int &score)
{
    // handle each row separately
    for (int row = 0; row < squaresPerSide; row++)
    {
        // set index limit for this row to be index of left-most tile on this row
        int limit = row * squaresPerSide;

        // Start from the second column and process each element from left to right
        for (int col = 1; col < squaresPerSide; col++)
        {

            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;

            // slide current piece over as far left as possible
            while (current > limit && board[current - 1] == 0)
            {
                board[current - 1] = board[current];
                board[current] = 0;
                current--;
            }

            // Combine it with left neighbor if values are the same and non zero.
            // The additional check for (current > limit) ensures a tile can be combined
            // at most once on a move, since limit is moved right every time a combination is made.
            // This ensures a row of:  2 2 4 4   ends up correctly as:  4 8 0 0   and not:  8 4 0 0
            if ((current > limit) && (board[current - 1] == board[current]) && (board[current] != 0))
            {
                board[current - 1] = board[current - 1] + board[current];
                board[current] = 0;
                limit = current;             // Reset row index limit, to prevent combining a piece more than once
                score += board[current - 1]; // Update score
            }

        } //end for( int col...
    }     //end for( int row...

} //end slideLeft()

//--------------------------------------------------------------------
// Slide all tiles right, combining matching values, updating the score
void slideRight(int board[], int squaresPerSide, int &score)
{
    // handle each row separately
    for (int row = 0; row < squaresPerSide; row++)
    {
        // set index limit for this row to be index of right-most tile on this row
        int limit = row * squaresPerSide + squaresPerSide - 1;

        // Start from the second-to-last column and process each element from right to left
        for (int col = squaresPerSide - 1; col >= 0; col--)
        {

            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;

            // slide current piece over as far right as possible
            while (current < limit && board[current + 1] == 0)
            {
                board[current + 1] = board[current];
                board[current] = 0;
                current++;
            }

            // Combine it with right neighbor if values are the same and non zero.
            // The additional check for (current < limit) ensures a tile can be combined
            // at most once on a move, since limit is moved left every time a combination is made.
            // This ensures a row of:  4 4 2 2   ends up correctly as:  0 0 8 4   and not:  0 0 4 8
            if ((current < limit) && (board[current + 1] == board[current]) && (board[current] != 0))
            {
                board[current + 1] = board[current + 1] + board[current];
                board[current] = 0;
                limit = current;             // Reset row index limit, to prevent combining a piece more than once
                score += board[current + 1]; // Update score
            }

        } //end for( int col...
    }     //end for( int row...

} //end slideRight()

//--------------------------------------------------------------------
// Slide all tiles up, combining matching values, updating the score
void slideUp(int board[], int squaresPerSide, int &score)
{
    // handle each column separately
    for (int col = 0; col < squaresPerSide; col++)
    {
        // set index limit for this column to be index of top-most tile on this row
        int limit = col;

        // Start from the second row and process each element from top to bottom
        for (int row = 1; row < squaresPerSide; row++)
        {

            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;

            // slide current piece up as far as possible
            while ((current > limit) && (board[current - squaresPerSide] == 0))
            {
                board[current - squaresPerSide] = board[current];
                board[current] = 0;
                current = current - squaresPerSide;
            }

            // Combine it with upper neighbor if values are the same and non zero.
            // The additional check for (current > limit) ensures a tile can be combined
            // at most once on a move, since limit is moved down every time a combination is made.
            if ((current > limit) && (board[current - squaresPerSide] == board[current]) && (board[current] != 0))
            {
                board[current - squaresPerSide] = board[current - squaresPerSide] + board[current];
                board[current] = 0;
                limit = current;                          // Reset row index limit, to prevent combining a piece more than once
                score += board[current - squaresPerSide]; // Update score
            }

        } //end for( int col...
    }     //end for( int row...

} //end slideUp()

//--------------------------------------------------------------------
// Slide all tiles down, combining matching values, updating the score
void slideDown(int board[], int squaresPerSide, int &score)
{
    // handle each column separately
    for (int col = 0; col < squaresPerSide; col++)
    {
        // set index limit for this column to be index of bottom-most tile on this row
        int limit = (squaresPerSide - 1) * squaresPerSide + col;

        // Start from the next to last row and process each element from bottom to top
        for (int row = squaresPerSide - 1; row >= 0; row--)
        {

            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;

            // slide current piece down as far as possible
            while (current < limit && board[current + squaresPerSide] == 0)
            {
                board[current + squaresPerSide] = board[current];
                board[current] = 0;
                current = current + squaresPerSide;
            }

            // Combine it with lower neighbor if values are the same and non zero.
            // The additional check for (current < limit) ensures a tile can be combined
            // at most once on a move, since limit is moved up every time a combination is made.
            if ((current < limit) && (board[current + squaresPerSide] == board[current]) && (board[current] != 0))
            {
                board[current + squaresPerSide] = board[current + squaresPerSide] + board[current];
                board[current] = 0;
                limit = current;                          // Reset row index limit, to prevent combining a piece more than once
                score += board[current + squaresPerSide]; // Update score
            }

        } //end for( int col...
    }     //end for( int row...

} //end slideDown()

//--------------------------------------------------------------------
// Return true if we're done, false if we are not done
//    Game is done if board is full and no more valid moves can be made
//    or if a tile with maxTileValue has been created.
bool gameIsOver(int board[],        // current board
                int squaresPerSide, // size of one side of board
                int maxTileValue)   // max tile value for this size board
{
    // See if the maxTileValue is found anywhere on the board.
    // If so, game is over.
    for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
    {
        if (board[i] == maxTileValue)
        {
            std::cout << "Congratulations!  You made it to " << maxTileValue << " !!!" << std::endl;
            return true; // game is over
        }
    }

    // See if there are any open squares.  If so return true since we aren't done
    for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
    {
        if (board[i] == 0)
        {
            return false; // there are open squares, so game is not over
        }
    }

    // All squares are full.
    // To check if board is done, make a copy of board, then slide left
    // and slide down.  If resulting board is same as original, we are done
    // with game since there are no moves to be made.
    int boardCopy[squaresPerSide * squaresPerSide];
    copyBoard(boardCopy, board, squaresPerSide);
    int tempScore = 0; // used as a placeHolder only for function calls below
    slideLeft(boardCopy, squaresPerSide, tempScore);
    slideDown(boardCopy, squaresPerSide, tempScore);

    // Compare each square of boards.  If any is different, a move is possible
    for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
    {
        if (boardCopy[i] != board[i])
        {
            return false; // Game is not over
        }
    }

    std::cout << "\n"
              << "No more available moves.  Game is over.\n"
              << "\n";
    return true; // Game is over since all squares are full and there are no moves
} //end gameIsOver()

//---------------------------------------------------------------------------------------
int main()
{
    int moveNumber = 1;                             // User move counter
    int score = 0;                                  // Cummulative score, which is sum of combined tiles
    int squaresPerSide = 4;                         // User will enter this value.  Set default to 4
    int board[MaxBoardSize * MaxBoardSize];         // space for largest possible board
    int previousBoard[MaxBoardSize * MaxBoardSize]; // space for copy of board, used to see
                                                    //    if a move changed the board.
    // Create the graphical board, an array of Square objects set to be the max size it will ever be.
    Square squaresArray[MaxBoardSize * MaxBoardSize];
    int maxTileValue = 1024; // 1024 for 4x4 board, 2048 for 5x5, 4096 for 6x6, etc.
    char userInput = ' ';    // Stores user input
    char aString[81];        // C-string to hold concatenated output of character literals
    int move = 1;            // user move counter

    // Create the graphics window
    sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 56: 1024");
    std::cout << std::endl;
    // Create and initialize the font, to be used in displaying text.
    sf::Font font;
    initializeFont(font);
    // Create the messages label at the bottom of the graphics screen, for displaying debugging information
    sf::Text messagesLabel("Welcome to 1024", font, 20);
    // Make a text object from the font
    messagesLabel.setColor(sf::Color(255, 255, 255));
    // Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
    messagesLabel.setPosition(0, WindowYSize - messagesLabel.getCharacterSize() - 5);

    displayInstructions();

    // Get the board size, create and initialize the board, and set the max tile value
    getSizeAndSetBoard(board, previousBoard, squaresPerSide, maxTileValue);


    int *boardCopy = new int[MaxBoardSize * MaxBoardSize];
    memset(boardCopy,0,MaxBoardSize * MaxBoardSize);
    copyBoard(boardCopy, board, squaresPerSide);

    Node *pHead = createNewNode(boardCopy, squaresPerSide, score);
    // Declare a pointer for the head of the list.  Add a node onto the list.
    //    The list may grow and shrink, but this first node should always be there.
    // ...

    // Run the program as long as the window is open.  This is known as the "Event loop".
    while (window.isOpen())
    {
        // Display both the graphical.
        displayAsciiBoard(board, squaresPerSide, score);
        displayList(move);

        // Make a copy of the board.  After we then attempt a move, the copy will be used to
        // verify that the board changed, which only then allows randomly placing an additional
        // piece on the board and updating the move number.
        copyBoard(previousBoard, board, squaresPerSide);

        // Prompt for and handle user input
        std::cout << move << ". Your move: ";
        std::cin >> userInput;
        int *previousUndoMove = NULL;
        Node *pPreviousMove = NULL;

        switch (userInput)
        {
        case 'x':
            std::cout << "Thanks for playing. Exiting program... \n\n";
            exit(0);
            break;
        case 'r':
            std::cout << "\n"
                      << "Resetting board \n"
                      << "\n";
            // Prompt for board size
            std::cout << "Enter the size board you want, between 4 and 12: ";
            std::cin >> squaresPerSide;
            getSizeAndSetBoard(board, previousBoard, squaresPerSide, maxTileValue);
            score = 0;
            move = 1;
            continue; // go back up to main loop and restart game
            break;
        case 'a':
            slideLeft(board, squaresPerSide, score); // Slide left
            break;

        case 's':
            slideDown(board, squaresPerSide, score); // Slide down
            break;

        case 'd':
            slideRight(board, squaresPerSide, score); // Slide right
            break;

        case 'w':
            slideUp(board, squaresPerSide, score); // Slide up
            break;

        case 'p':
            // Place a piece on the board
            int index; // 1-d array index location to place piece
            int value; // value to be placed
            std::cin >> index >> value;
            board[index] = value;
            continue; // Do not increment move number or place random piece
            break;
        case 'u':
            pPreviousMove = undoMove(pHead);
            if(pPreviousMove == NULL) {
                cout << "*** You cannot undo past the beginning of the game.  Please retry. ***";
                break;
            } else {
                move--;
            }
            score = pPreviousMove->score;
            squaresPerSide = pPreviousMove->squaresPerSide;
            previousUndoMove = pPreviousMove->data;
            for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
            {
                previousBoard[i] = previousUndoMove[i];
                board[i] = previousUndoMove[i];
            }
            break;
        default:
            std::cout << "Invalid input, please retry.";
            continue;
            break;
        } //end switch( userInput)

        // If the move resulted in pieces changing position, then it was a valid move
        // so place a new random piece (2 or 4) in a random open square and update move number.
        // Add the new board, moveNumber and score to a new list node at the front of the list.
        if (boardChangedThisTurn(previousBoard, board, squaresPerSide))
        {
            // Place a random piece on board
            delete [] boardCopy;
            boardCopy = new int[MaxBoardSize * MaxBoardSize]; 
            memset(boardCopy,0,MaxBoardSize * MaxBoardSize);
            copyBoard(boardCopy, board, squaresPerSide);
            addNode(pHead, createNewNode(boardCopy, squaresPerSide, score));
            placeRandomPiece(board, squaresPerSide);

            // Update move number after a valid move
            move++;
        }

        // See if we're done
        if (gameIsOver(board, squaresPerSide, maxTileValue))
        {
            // Display the final board
            displayAsciiBoard(board, squaresPerSide, score);
            displayList(move);
            break;
        }

        //system("clear");   // Clear the screen

        // Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    } //end while( window.isOpen())

    return 0;
} //end main()
