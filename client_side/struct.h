#define BUFFER_SIZE 256

enum HEADER
{
    ERROR,
    OK,
    SIGN_IN,
    LOG_OUT,
    SIGN_UP,
    EXIT_PROGRAM,
    PLAY_BOT,
    FIND_PLAYER,
    PLAYER_MADE_MOVE
};

enum STATUS
{
    PROCESS,
    WIN,
    LOSE,
    DRAW,
    DISCONNECTED,
    WAITING
};

enum BOARD
{
    NOUGHTS, // O
    CROSSES, // X
    BORDER,
    EMPTY
};

typedef struct _account
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int socket_fd;
    int status;
    struct _account* next;
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
    struct _game* next;
} Game;

typedef struct _message
{
    int header;
    Account account;
    Game game;
    char message[BUFFER_SIZE];
} Message;