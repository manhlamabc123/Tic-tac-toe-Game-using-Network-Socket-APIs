#include "../account/account.h"

void make_move(int *board, const int sq, const int side);
int get_bot_move(int *board, const int side);
int get_winning_move(int *board, const int side);
int get_next_best(const int *board);
int get_player_move(const int *board, const int side);
int has_empty(const int *board);
void print_board(const int *board);
void print_game(Game *game);
void initialise_board(int *board);
void server_game_bot(int, Game);
int get_side(Game);
int find_player(int, Game*, Account*, Account);
int initialise_game(Game*, Account*, Account);
void player_vs_player(int, Game);