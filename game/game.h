#include "../helper/helper.h"
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

void play_with_bot(int, Account);
void make_move(int *board, const int sq, const int side);
int get_bot_move(int *board, const int side);
int GetWinningMove(int *board, const int side);
int GetNextBest(const int *board);
int get_player_move(const int *board, const int side, int* real_move);
int has_empty(const int *board);
void print_board(const int *board, Account current_user);
void initialise_board(int *board);

void server_game_bot(int, Account*);
int get_side(Game);