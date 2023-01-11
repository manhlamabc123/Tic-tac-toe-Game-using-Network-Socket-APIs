#include "../helper/helper.h"

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
    char date[BUFFER_SIZE];
    Board board;
    Account first_player;
    Account second_player;
    int number_of_moves;
    Move moves[BUFFER_SIZE];
    int status;
} Game;

void make_move(int *board, const int sq, const int side);
void print_board(const int *board, Account current_user);
void initialise_board(int *board);
int get_side(Game, Account);
void play_with_bot(int, Account);
void find_player(int, Account*);
void play_with_player(int, Account, Game);