#define BUFFER_SIZE 1024

typedef struct _Account {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int socket_fd;
    int is_signed_in;
    struct _Account *next;
} Account;

char welcome();
int menu();
int program_exit(int);
int sign_up(int);
int sign_in(int, Account*, int);
int log_out(int, char*, int);