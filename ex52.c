#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define BOARD_SIZE 20
#define DONE 1
#define NOTDONE 2
#define HORIZONTAL 3
#define VERTICAL 4
#define ERROR -1

typedef struct Game
{
    char board[BOARD_SIZE][BOARD_SIZE];
    int middle_x;
    int middle_y;
    int layout;
}Game;

//Function declarations.
void writeError();
void handle(int num);
void printBoard();
void updateBoard();
void handleAlarm();
void initializeGame();

//Global variables.
Game game;
int doneFlag=NOTDONE;

/***
 * Implements a Tetris Game.
 * The function waits for signals from ex51.c ,when a signal arrives the "handle" function is called.
 * Also an alarm is set and a function to handle "handleAlarm".
 * @return
 */
int main() {
    initializeGame();
    signal(SIGALRM,handleAlarm);
    signal(SIGUSR2,handle);
    updateBoard();
    printBoard();
    alarm(1);
    while(doneFlag == NOTDONE) {
        pause();
    }
}

/***
 * Initializes the struct 'Game' variables.
 */
void initializeGame() {
    game.layout=VERTICAL;
    game.middle_x=1;
    game.middle_y=BOARD_SIZE/2;
}

/***
 * The function handles a signal. It reads a char from the Pipe and handles it.
 * 'a' is to move left, 'd' is to move right, 's' is to move down, 'w' is to change the layout of the shape
 * and 'q' is to end the program so we change the 'doneFlag'.
 * Also we update the board and print it using the correct functions.
 * @param num
 */
void handle(int num){
    char ch;
    read(STDIN_FILENO,&ch,sizeof(ch));
    switch(ch){
        case 'a':{
            if((game.layout == HORIZONTAL && game.middle_y-1 > 1) || (game.layout == VERTICAL && game.middle_y > 1))
                game.middle_y=game.middle_y-1;
            break;
        }
        case 'd':{
            if((game.layout == HORIZONTAL && game.middle_y+1 < BOARD_SIZE-2)||(game.layout == VERTICAL && game.middle_y < BOARD_SIZE-2))
                game.middle_y=game.middle_y+1;
            break;
        }
        case 'w':{
            if(game.layout == HORIZONTAL && game.middle_x < BOARD_SIZE-2)
                game.layout=VERTICAL;
            else if(game.layout == VERTICAL && game.middle_y > 1 && game.middle_y < BOARD_SIZE-2)
                game.layout=HORIZONTAL;
            break;
        }
        case 's':{
            if(game.layout == HORIZONTAL && game.middle_x < BOARD_SIZE-2)
                game.middle_x=game.middle_x+1;
            else if(game.layout == VERTICAL && game.middle_x+1 < BOARD_SIZE-2)
                game.middle_x=game.middle_x+1;
            else initializeGame();
            break;
        }
        case 'q':{
            doneFlag=DONE;
            return;
        }
    }
    updateBoard();
    printBoard();
    signal(SIGUSR2,handle);
}

/***
 * Updates the board content according to the struct variables.
 */
void updateBoard() {
    int i,j;
    for(i=0;i<BOARD_SIZE;i++){
        for(j=0;j<BOARD_SIZE;j++){
            game.board[i][j]=' ';
            if(i == BOARD_SIZE-1 || j == 0 || j == BOARD_SIZE-1)
                game.board[i][j]='*';
            if(game.layout == VERTICAL && j == game.middle_y && (i == game.middle_x || i == game.middle_x-1 || i == game.middle_x+1 ))
                game.board[i][j]='-';
            if(game.layout == HORIZONTAL && i == game.middle_x && (j == game.middle_y || j == game.middle_y-1 || j == game.middle_y+1 ))
                game.board[i][j]='-';
        }
    }
}

/***
 * Goes over the board and prints each char.
 */
void printBoard(){
    if(system("clear") == ERROR)
        writeError();
    int i,j;
    for(i=0;i<BOARD_SIZE;i++){
        for(j=0;j<BOARD_SIZE;j++){
            printf("%c",game.board[i][j]);
        }
        printf("\n");
    }
}

/***
 * Handles the alarm signal by lowering the shape down by one.
 * Also sets a new alarm.
 */
void handleAlarm(){
    if(game.layout == HORIZONTAL && game.middle_x < BOARD_SIZE-2)
        game.middle_x=game.middle_x+1;
    else if(game.layout == VERTICAL && game.middle_x+1 < BOARD_SIZE-2)
        game.middle_x=game.middle_x+1;
    else initializeGame();
    updateBoard();
    printBoard();
    signal(SIGALRM,handleAlarm);
    alarm(1);
}

/***
 * Writes an Error message to STDERR.
 */
void writeError() {
    char *error = "Error in system call";
    write(STDERR_FILENO, error, sizeof(error));
}
