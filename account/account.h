typedef struct _Account {
    char username[30];
    char password[30];
    int status;
    int is_signed_in;
    struct _Account *next;
} Account;

Account *create_new_account(char *username, char *password, int status);
Account *add_account(Account *account, char *username, char *password, int status);
int check_user(Account *account, char *username);
int check_password(Account *account, char *password);
int check_blocked(Account *account, char *username);
int check_signed_in(Account *account, char *username);
Account *read_account(Account *account);
Account* account_sign_up(int client_fd, Account *account);
void update_file(Account *account);
int account_sign_in(Account *acc, char* username, char* password);
void search(Account *account);
int sign_out(Account *account, char* username);
void free_list(Account *account);
void activate(Account *account);
int change_password(Account *acc, char* username, char* new_password);
void change_current_account_status(Account* acc, char* username, int status);