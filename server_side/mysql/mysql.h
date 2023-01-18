#include <mysql/mysql.h>
#include "../struct.h"

int check_user(Account *account, char *username);
Account *create_new_account(char *username, char *password);
Account *add_account(Account *account, char *username, char *password);
MYSQL* connect_to_database();
Account* database_read_all_accounts(MYSQL*);
int database_add_new_user(MYSQL*, char*, char*);
int database_add_new_game(MYSQL*, Game);