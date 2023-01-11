#include "../mysql/mysql.h"

int check_password(Account *account, char* username, char *password);
int check_signed_in(Account *account, char *username);
Account *read_account(Account *account);
Account* account_sign_up(int client_fd, Account *account);
void update_file(Account *account);
void account_sign_in(int client_fd, Account *account);
Account* account_search(Account *account, Account user);
void account_log_out(int client_fd, Account *account);
void free_list(Account *account);
int change_password(Account *account, char* username, char* new_password);
void print_account_info(Account* user);