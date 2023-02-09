#include "../mysql/mysql.h"

int check_password(Account*, char*, char*);
int check_signed_in(Account*, char*);
Account *read_account(Account*);
Account* account_sign_up(int, Account*, Account);
void account_sign_in(int, Account*, Account);
Account* account_search_by_account(Account*, Account);
Account* account_search_by_socketfd(Account*, int);
void account_log_out(int, Account*, Account);
void free_list(Account*);
void print_account_info(Account*);