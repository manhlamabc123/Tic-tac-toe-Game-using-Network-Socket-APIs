#include "../mysql/mysql.h"

int check_password(Account*, char*, char*);
int check_signed_in(Account*, char*);
Account *read_account(Account*);
Account* account_sign_up(int, Account*);
void update_file(Account*);
void account_sign_in(int, Account*, Account);
Account* account_search(Account*, Account);
void account_log_out(int, Account*);
void free_list(Account*);
int change_password(Account*, char*, char*);
void print_account_info(Account*);