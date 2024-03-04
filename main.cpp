#include <iostream>
#include <random>
#include "imageProcessing.h"

using namespace std;

//Space class, contains row/col coordinates and whether it is occupied or not
//If whoOccupies is 0, neither player has marked it. If it is 1, the player
//has marked it. If it is 2, the computer has marked it.
class Space {
public:
    int rowCoordinate,
        colCoordinate,
        whoOccupies = 0;

    //constructor
    Space(int row, int col) {
        rowCoordinate = row;
        colCoordinate = col;
    }

    Space(int row, int col, int occupied) {
        rowCoordinate = row;
        colCoordinate = col;
        whoOccupies = occupied;
    }

    //empty constructor
    Space() {}
};

//Board class, contains a 2d array of spaces to represent a 4-in-a-Row board,
//as well as some functions for getting or setting who owns a space and determining a 4 in a row
class Board {
public:
    Space board[6][7];

    //Constructor, initialize each space
    Board() {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 7; j++) {
                board[i][j] = Space(i, j);
            }
        }
    }

    /*
    Function: isSpaceOccupied
    Purpose: determine if a space in the board array is occupied
    Arguments: int - the row coordinate of the space
                col - the column coordinate of the space
    Returns:  bool - true if the space's whoOccupies value is not 0
                false if it is 0
    */
    bool isSpaceOccupied(int row, int col) {
        if (row < 0 || row > 5 || col < 0 || col > 6) {
            cout << "Invalid space" << endl;
            return false;
        } else if (board[row][col].whoOccupies != 0)
            return true;
        else
            return false;
    }

    /*
    Function: isSpaceAvailable
    Purpose: determine if a space in the board array can be accessed
    Arguments: int - the row coordinate of the space
                col - the column coordinate of the space
    Returns:  bool - true if space is on the bottom row or the space below it is occupied, false otherwise
    */
    bool isSpaceAvailable(int row, int col) {

        if (row < 0 || row > 5 || col < 0 || col > 6 || isSpaceOccupied(row, col)) {
            return false;
        } else if (row == 0 || isSpaceOccupied((row - 1), col) && !isSpaceOccupied(row, col))
            return true;
        
        cout << "Error" << endl;
        return false;
    }

    /*
    Function: playerOccupies
    Purpose: set a space to be occupied by the player
    Arguments: int - the row coordinate of the space
                col - the column coordinate of the space
    Returns:   N/A
    Side Effects: the space above the space being marked becomes available
    */
    void playerOccupies(int row, int col) {
        if (row < 0 || row > 5 || col < 0 || col > 6) {
            cout << "Invalid space" << endl;
        } else if (board[row][col].whoOccupies == 0) {
            board[row][col].whoOccupies = 1;
        }
    }

    /*
    Function: cpuOccupies
    Purpose: set a space to be occupied by the computer
    Arguments: int - the row coordinate of the space
                col - the column coordinate of the space
    Returns:   N/A
    Side Effects: the space above the space being marked becomes available
    */
    void cpuOccupies (int row, int col) {
        if (row < 0 || row > 5 || col < 0 || col > 6) {
            cout << "Invalid space" << endl;
        } else if (board[row][col].whoOccupies == 0) {
            board[row][col].whoOccupies = 2;
            cout << "Robot marks row " << row << " col " << col << endl;
        }

        //mark the board
    }

    /*
    Function: is4InARow
    Purpose: determine if there is a 4 in a row from the most recent space added
    Arguments:  row - the row number of the source space
                col - the col number of the source space
                who - the player that is being checked for 4 in a row
    Returns:   bool - true if 4 in a row is found, false if not
    */
    bool is4InARow (int row, int col, int who) {
        //Check that the origin space is owned by who
        if (board[row][col].whoOccupies != who)
            return false;

        //Try to find 3 spaces connected to origin
        if (verticalCount(row, col, who)
            || horizontalCount(row, col, who)
            || diagonalNegativeSlopeCount(row, col, who)
            || diagonalPositiveSlopeCount(row, col, who))
            return true;
        else
            return false;
    }

    /*
    Function: verticalCount
    Purpose: count the number of spaces below an origin space that are owned by the same player as origin
    Arguments:  row - the row of origin space
                col - the col of origin space
                who - player that owns origin
    Returns:   bool - true if 3 or more subsequent spaces are found, false if not
    */
    bool verticalCount (int row, int col, int who) {
        int count = 0;

        //count below. if 3 found, return immediately
        for (int i = row - 1; i > -1; i--) {
            if (board[i][col].whoOccupies == who)
                count++;
            else   
                break;
            
            if (count == 3)
                return true;
        }

        return false;
    }

    /*
    Function: horizontalCount
    Purpose: count the number of spaces besides an origin space that 
                are owned by the same player as origin
    Arguments:  row - the row of origin space
                col - the col of origin space
                who - player that owns origin
    Returns:   bool - true if 3 or more subsequent spaces are found, false if not
    */
    bool horizontalCount (int row, int col, int who) {
        int count = 0;
        
        //Count left of origin. 
        //If 3 found, return immediately
        for (int i = col - 1; i > -1; i--) {
            if (board[row][i].whoOccupies == who)
                count++;
            else 
                break;
            
            if (count == 3)
                return true;
        }

        //Count right of origin.
        //If 3 found, return immediately
        for (int i = col + 1; i < 7; i++) {
            if (board[row][i].whoOccupies == who)
                count++;
            else
                break;
            
            if (count == 3)
                return true;
        }

        return false;
    }

    /*
    Function: diagonalNegativeSlopeCount
    Purpose: count the number of spaces from an origin space that are
            owned by the same player as origin
    Arguments:  row - the row of origin space
                col - the col of origin space
                who - player that owns origin
    Returns:   bool - true if 3 or more subsequent spaces are found, false if not
    */
    bool diagonalNegativeSlopeCount (int row, int col, int who) {
        int count = 0;

        //Define lesser and greater
        //These are constraining values so that the
        //loop cannot go beyond the bounds of the game board
        //lesser is set to which of row and col is closer to their left/bottom border
        //greater is set to which of row and col is closer to their right/top border
        int lesser, greater;
        if ((6 - row) < col) {
            lesser = (5 - row);
            greater = (6 - col);
        } else if ((6 - row) == col) {
            lesser = (5 - row);
            greater = (6 - col);
        } else {
            lesser = col;
            greater = row;
        }

        //Count top-left of origin
        //If count == 3, return immediately
        for (int i = 0; i < lesser; i++) {
            if (board[row + (i+1)][col - (i+1)].whoOccupies == who)
                count++;
            else 
                break;
            
            if (count == 3)
                return true;
        }

        //Count bottom-right of origin
        for (int i = 0; i < greater; i++) {
            if (board[row - (i+1)][col + (i+1)].whoOccupies == who)
                count++;
            else 
                break;
            

            if (count == 3)
                return true;
        }

        return false;
    }

    /*
    Function: diagonalPositiveSlopeCount
    Purpose: count the number of spaces from an origin space that are
            owned by the same player as origin
    Arguments:  row - the row of origin space
                col - the col of origin space
                who - player that owns origin
    Returns:   bool - true if 3 or more subsequent spaces are found, false if not
    */
    bool diagonalPositiveSlopeCount (int row, int col, int who) {
        int count = 0;

        //Define lesser and greater
        //These are constraining values so that the
        //loop cannot go beyond the bounds of the game board
        //lesser is set to which of row and col is closer to their left/bottom border
        //greater is set to which of row and col is closer to their right/top border
        int lesser, greater;
        if (row < col) {
            lesser = row;
            greater = (5 - row);
        } else if (row == col) {
            lesser = col;
            greater = (5 - row);
        } else {
            lesser = col;
            greater = row;
        }

        //Count bottom-left of origin
        //If count > 3, return immediately
        for (int i = 1; i < lesser + 1; i++) {
            if (board[row - i][col - i].whoOccupies == who)
                count++;
            else
                break;
            
            if (count == 3)
                return true;
        }

        //Count top-right of origin
        for (int i = 1; i < greater + 1; i++) {
            if (board[row + i][col + i].whoOccupies == who)
                count++;
            else
                break;
            

            if (count == 3)
                return true;
        }

        return false;
    }

    /*
    Function: decideRobotMove
    Purpose: Calls cpuOccupies with two other class functions as arguments. Exists to make selecting a robot move cleaner in the code
    Arguments:  int - the column of the most recent player move
    Returns:  Space - the space being marked on the board
    Side Effects:   the space that is chosen will be marked by the robot
    */
    Space decideRobotMove(int mostRecentPlayerMoveCol) {
        int chosenCol= chooseColumn(mostRecentPlayerMoveCol);

        //if chosenCol is a full column, increment through columns 
        //until an available one is found
        while (isColumnFull(chosenCol)) {
            chosenCol = (chosenCol + 1) % 7;
        }

        int chosenRow = availableRowInCol(chosenCol);
        cpuOccupies(chosenRow, chosenCol);
        
        return Space(chosenRow, chosenCol, 2);
    }

    /*
    Function: availableRowInCol
    Purpose: determine the first available row in a column for marking
    Arguments:  int - the column being checked
    Returns:    int - the row that is available
    */
    int availableRowInCol(int col) {
        for (int i = 0; i < 6; i++) {
            if (isSpaceAvailable(i, col))
                return i;
        }

        return -1;
    }

    /*
    Function: chooseColumn
    Purpose: randomly selects a column value from 0-6
    Arguments:  int - the base column the random value will be weighted against
    Returns:    int - the chosen column
    */
    int chooseColumn(int baseCol) {
        //generate a random number between 0 and 20
        int value = rand() % 20;
        int push = 0;

        //Choose column
        /*
        7/21 - same col as baseCol, return immediately
        10/21 - either of the adjacent columns to baseCol
        2/21 - either of the columns two columns away from baseCol
        2/21 - either of the columns three columns away from baseCol
        */
        if (value < 7)
            return baseCol;
        else if (value < 17)
            push = 1;
        else if (value < 19)
            push = 2;
        else
            push = 3;
        
        //If baseCol wasn't chosen,
        //do a 50/50 on whether to choose the col
        //to (push) columns left or right of baseCol
        //In case baseCol -/+ push exceeds bounds of board,
        //uses modulus so it chooses a column on the other side.
        if (rand() % 2 == 0)
            return (((baseCol - push) % 7) + 7) % 7;
        else
            return (baseCol + push) % 7;
    }

    /*
    Function: isColumnFull
    Purpose: determine if a column in the game board is full
    Arguments:  int - the column to check
    Returns:   bool - true if the column is full, false if not
    */
    bool isColumnFull(int col) {
        //Check if the top spot in the column is occupied
        //should never be occupied if not all the spaces below it aren't
        if (board[5][col].whoOccupies != 0)
            return true;
        else
            return false;
    }

    /*
    Function: isTieState
    Purpose: determine if all spots in board are occupied
    Arguments:  N/A
    Returns:    bool - true if tie state reached, false if not
    */
    bool isTieState() {
        //check if the top spot in each column is full,
        //they should only be full if all spaces below them are also full.
        for (int i = 0; i < 7; i++) {
            if (!isColumnFull(i))
                return false;
        }

        return true;
    }
};

//Main function
int main() {
    Board game = Board();
    int turn = 0;
    srand(time(0));
    
    //Calibrate HSV of player mark color and the size of frame
    int *hsvPtr = calibratePlayerColor();

    //end immediately if hsvPtr = -1
    if (*hsvPtr == -1)
        return 0;
    
    //wait for user to remove calibration mark and take a picture of the blank board
    cout << "Please remove the calibration mark. Press ESC when calibration mark has been removed to take an image of the empty board." <<endl;
    cout << "Press ESC to continue." << endl;
    Mat previousGameState = getImage(hsvPtr);


    //play the game for 21 turns
    //When turn reaches 22, all spaces should have been marked, so is a tie state
    while (turn < 21) {
        //increment turn
        turn++;
        cout << "Turn #" << turn << endl;
        
        //player move
        //Get next game state
        cout << "Put your move on the board. Press ESC to continue." << endl;
        Mat newGameState = getImage(hsvPtr);

        //Get coordinates of next move
        Moments newMove = findMoments(previousGameState, newGameState);

        //Calculate relative position of the move
        int col = trunc(float (int (newMove.m10 / newMove.m00)) / *(hsvPtr + 6) * 7);
        int row = trunc(float (*(hsvPtr + 7) - (int (newMove.m01 / newMove.m00))) / *(hsvPtr + 7) * 6);

        //checking if space is available
        //if not, try again
        if (game.isSpaceAvailable(row, col))
            game.playerOccupies(row, col);
        else {
            cout << "Error. Invalid option. Please try again." << endl;
            turn--;
            continue;
        }

        //check for 4-in-a-row, only check on turn 4 or higher to reduce runtime
        if (turn > 3 && game.is4InARow(row, col, 1)) {
            cout << "Congratulations, player! You won!" << endl;
            break;
        }

        //computer move
        //Call function to decide robot's next move
        Space robotMove = game.decideRobotMove(col);

        //check for 4-in-a-row, only check on turn 4 or higher to reduce runtime
        if (turn > 3 && game.is4InARow(robotMove.rowCoordinate, robotMove.colCoordinate, 2)) {
            cout << "Sorry, CPU player won! Better luck next time!" << endl;
            break;
        }

        //set the previous game state to the current one
        previousGameState = newGameState;
    }
    
    if (turn >= 21)
        cout << "Tie state reached. Ending game.";

    return 0;
    
}