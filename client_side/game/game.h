#include "../helper/helper.h"

void make_move(int *board, const int sq, const int side);
void print_board(const int *board, Account current_user);
void initialise_board(int *board);
int get_side(Game, Account);
void play_with_bot(int, Account);
void find_player(int, Account*);
void play_with_player(int, Account, Game);