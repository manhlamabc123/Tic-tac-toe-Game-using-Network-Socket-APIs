#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "game.h"

const int convert_to_25[9] = {
    6, 7, 8,
    11, 12, 13,
    16, 17, 18};

void initialise_board(int *board)
{
    int i = 0;
    for (i = 0; i < 25; ++i)
    {
        board[i] = BORDER;
    }
    for (i = 0; i < 9; ++i)
    {
        board[convert_to_25[i]] = EMPTY;
    }
}

void print_board(const int *board, Account current_user, Account second_player)
{
    int i = 0;
    char pceChars[] = "OX|-";
    system("clear");
    printf("[+]You: %s\n", current_user.username);
    printf("[+]Opponent: %s\n", second_player.username);
    printf("[+]Board:\n");
    for (i = 0; i < 9; ++i)
    {
        if (i != 0 && i % 3 == 0)
        {
            printf("\n\n");
        }
        printf("%4c", pceChars[board[convert_to_25[i]]]);
    }
    printf("\n");
}

int get_player_move(const int *board, const int side)
{
    char userInput[4];
    int i = 0;
    int moveOk = 0;
    int move = -1;
    while (moveOk == 0)
    {
        printf("[+]Please enter a move from 1 to 9: ");
        fgets(userInput, 3, stdin);
        fflush(stdin);

        if (strlen(userInput) != 2)
        {
            printf("[-]Invalid strlen()\n");
            continue;
        }
        if (sscanf(userInput, "%d", &move) != 1)
        {
            move = -1;
            printf("[-]Invalid sscanf()\n");
            continue;
        }

        if (move < 1 || move > 9)
        {
            move = -1;
            printf("[-]Invalid range\n");
            continue;
        }

        move--;
        if (board[convert_to_25[move]] != EMPTY)
        {
            move = -1;
            printf("[-]Square not available\n");
            continue;
        }
        moveOk = 1;
    }
    printf("[+]Making Move: %d\n", (move + 1));

    return convert_to_25[move];
}

void make_move(int *board, const int sq, const int side)
{
    board[sq] = side;
}

void play_with_bot(int socket_fd, Account current_user)
{
    // Initialize variables
    int side = NOUGHTS; // O
    int move = 0;
    Game game;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time[BUFFER_SIZE];
    Move next_move;
    Message message;

    // Get current time
    snprintf(time, sizeof(time), "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Set game date
    strcpy(game.date, time);

    // Get first player
    game.first_player = current_user;

    // Set game variables
    game.status = PROCESS;
    game.number_of_moves = 0;

    // Initialise board
    game.board.size = 3;
    initialise_board(game.board.board);

    while (1)
    {
        // Print board
        print_board(game.board.board, current_user, game.second_player);

        // Check game status
        switch (game.status)
        {
        case WIN:
            printf("\n[+]%s won - %s lost\n", game.first_player.username, game.second_player.username);
            break;
        case LOSE:
            printf("\n[+]%s won - %s lost\n", game.second_player.username, game.first_player.username);
            break;
        case DRAW:
            printf("\n[+]A Draw\n");
            break;
        default:
            break;
        }

        if (game.status != PROCESS)
            break;

        // Get user move
        move = get_player_move(game.board.board, side);
        make_move(game.board.board, move, side);

        // Create next move
        next_move.account = current_user;
        next_move.move = move;
        game.moves[game.number_of_moves] = next_move;
        game.number_of_moves = game.number_of_moves + 1;

        // Create message
        message.header = PLAY_BOT;
        message.game = game;

        // Send game bot signal to Server
        if (send(socket_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        print_board(game.board.board, current_user);
        printf("[+]Waiting for opponent...\n");

        // Recv feedback from Server
        if (recv(socket_fd, &message, sizeof(struct _message), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Handling message
        switch (message.header)
        {
        case OK:
            game = message.game;
            break;
        default:
            printf("[-]Disconnected from the server\n");
            return;
        }
    }
}

void find_player(int socket_fd, Account *current_user)
{
    Message message;
    fd_set read;
    struct timeval tv;
    int ready;
    Game game;

    // Create message
    message.header = FIND_PLAYER;
    message.account = *current_user;

    // Send message to Server
    if (send(socket_fd, &message, sizeof(struct _message), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    printf("[+]Finding other player...\n");

    tv.tv_sec = 30;
    tv.tv_usec = 0;
    FD_ZERO(&read);
    FD_SET(socket_fd, &read);
    ready = select(FD_SETSIZE, &read, NULL, NULL, &tv);

    if (ready == 0)
    {
        printf("[+]Time out. Found nobody\n");

        message.header = TIME_OUT;
        game.status = DISCONNECTED;
        message.game = game;

        // Send message to Server
        if (send(socket_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        return;
    }

    if (FD_ISSET(socket_fd, &read))
    {
        // Recv game from Server
        if (recv(socket_fd, &message, sizeof(struct _message), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Handling message
        switch (message.header)
        {
        case OK:
            break;
        default:
            printf("[-]Disconnected from the server\n");
            return;
        }

        play_with_player(socket_fd, *current_user, message.game);
    }

    return;
}

int get_side(Game game, Account current_user)
{
    if (strcmp(current_user.username, game.first_player.username) == 0)
        return NOUGHTS;
    else
        return CROSSES;
    return -1;
}

void play_with_player(int socket_fd, Account current_user, Game game)
{
    // Initialize variables
    int side;
    int move = 0;
    Move next_move;
    int second_player_wait = 1;
    Message message;
    fd_set read;
    struct timeval tv;
    int ready;

    while (1)
    {
        // Print board
        print_board(game.board.board, current_user);

        // Check game status
        switch (game.status)
        {
        case WIN:
            printf("\n[+]%s won - %s lost\n", game.first_player.username, game.second_player.username);
            break;
        case LOSE:
            printf("\n[+]%s won - %s lost\n", game.second_player.username, game.first_player.username);
            break;
        case DRAW:
            printf("\n[+]A Draw\n");
            break;
        default:
            break;
        }

        if (game.status != PROCESS)
            break;

        // Get side
        side = get_side(game, current_user);
        if (side == -1)
        {
            printf("[-]Error: get_side\n");
            return;
        }
        if (side == NOUGHTS)
            printf("[+]You go first\n");
        else if (side == CROSSES)
            printf("[+]You go second\n");

        if (side == CROSSES && second_player_wait == 1)
        {
            second_player_wait--;
            goto wait;
        }

        move = get_player_move(game.board.board, side);
        make_move(game.board.board, move, side);

        // Create next move
        next_move.account = current_user;
        next_move.move = move;
        game.moves[game.number_of_moves] = next_move;
        game.number_of_moves = game.number_of_moves + 1;

        // Create message
        message.header = PLAYER_MADE_MOVE;
        message.game = game;

        // Send message to Server
        if (send(socket_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        print_board(game.board.board, current_user);
    wait:
        printf("[+]Waiting for opponent...\n");

        tv.tv_sec = 180;
        tv.tv_usec = 0;
        FD_ZERO(&read);
        FD_SET(socket_fd, &read);
        ready = select(FD_SETSIZE, &read, NULL, NULL, &tv);

        if (ready == 0)
        {
            printf("[-]Opponent disconnected\n");
            printf("[+]Game's record will not be saved\n");

            message.header = TIME_OUT;
            message.game.status = DISCONNECTED;

            // Send message to Server
            if (send(socket_fd, &message, sizeof(struct _message), 0) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }

            return;
        }

        if (FD_ISSET(socket_fd, &read))
        {
            // Recv feedback from Server
            if (recv(socket_fd, &message, sizeof(struct _message), MSG_WAITALL) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }

            // Handling feedback
            switch (message.header)
            {
            case OK:
                game = message.game;
                break;
            case ERROR:
                if (message.game.status == DISCONNECTED)
                {
                    printf("[-]Opponent disconnected\n");
                    printf("[+]Game's record will not be saved\n");
                }
                return;
            default:
                printf("[-]Disconnected from the server\n");
                return;
            }
        }
    }
}