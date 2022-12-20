typedef struct _Account {
    char username[30];
    char password[30];
    int is_signed_in;
    struct _Account *next;
} Account;

typedef struct _SignInFeedback
{
    int feedback;
    Account current_user;
} SignInFeedback;

Account *create_new_account(char *username, char *password);
Account *add_account(Account *account, char *username, char *password);
int check_user(Account *account, char *username);
int check_password(Account *account, char *password);
int check_signed_in(Account *account, char *username);
Account *read_account(Account *account);
Account* account_sign_up(int client_fd, Account *account);
void update_file(Account *account);
void account_sign_in(int client_fd, Account *acc);
void search(Account *account);
void account_log_out(int client_fd, Account *account);
void free_list(Account *account);
int change_password(Account *acc, char* username, char* new_password);