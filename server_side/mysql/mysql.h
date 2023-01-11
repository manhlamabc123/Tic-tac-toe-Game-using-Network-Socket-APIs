#include <mysql/mysql.h>
#define BUFFER_SIZE 1024

typedef struct _Account {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int socket_fd;
    int is_signed_in;
    struct _Account *next;
} Account;

int check_user(Account *account, char *username);
Account *create_new_account(char *username, char *password);
Account *add_account(Account *account, char *username, char *password);
MYSQL* connect_to_database();
Account* database_read_all_accounts(MYSQL*);
int database_add_new_user(MYSQL*, char*, char*);