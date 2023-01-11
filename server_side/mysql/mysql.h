#include <mysql/mysql.h>
#define BUFFER_SIZE 1024

typedef struct _Account {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int socket_fd;
    int is_signed_in;
    struct _Account *next;
} Account;

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

int check_user(Account *account, char *username);
Account *create_new_account(char *username, char *password);
Account *add_account(Account *account, char *username, char *password);
MYSQL* connect_to_database();
Account* database_read_all_accounts(MYSQL*);
int database_add_new_user(MYSQL*, char*, char*);
void convert_move_to_string(Game game, char *move);
int database_add_new_game(MYSQL*, Game);