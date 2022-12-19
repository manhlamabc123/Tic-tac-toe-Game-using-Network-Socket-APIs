#include "../account/account.h"
#define BUFFER_SIZE 1024

enum STATUS
{
    PROCESS,
    WIN,
    LOSE,
    DRAW
};

enum BOARD
{
    NOUGHTS, // O
    CROSSES, // X
    BORDER,
    EMPTY
};

typedef struct _board
{
    int size;
    int board[BUFFER_SIZE];
} Board;

typedef struct _move
{
    Account account;
    int move;
} Move;

typedef struct _game
{
    int id;
    char date[BUFFER_SIZE];
    Board board;
    Account first_player;
    Account second_player;
    int number_of_moves;
    Move moves[BUFFER_SIZE];
    int status;
} Game;

void RunGameBot(int, char*);
void MakeMove(int *board, const int sq, const int side);
int GetComputerMove(int *board, const int side);
int GetWinningMove(int *board, const int side);
int GetNextBest(const int *board);
int GetHumanMove(const int *board, const int Side);
int HasEmpty(const int *board);
void PrintBoard(const int *board);
void InitialiseBoard(int *board);

void server_game_bot(int, Account*);
int GetSide(Game);