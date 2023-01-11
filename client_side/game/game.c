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

void print_board(const int *board, Account current_user)
{
    int i = 0;
    char pceChars[] = "OX|-";
    // system("clear");
    printf("[+]You: %s\n", current_user.username);
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
    char game_bot_signal[BUFFER_SIZE] = "4\0";
    Game game;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time[BUFFER_SIZE];
    Move next_move;
    char feedback[BUFFER_SIZE];

    // Get current time
    snprintf(time, sizeof(time), "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Set game date
    strcpy(game.date, time);

    // Get first player
    game.first_player = current_user;

    // Set game variables
    game.status = PROCESS;
    game.number_of_moves = 0;

    // Send game bot signal to Server
    if (send(socket_fd, game_bot_signal, sizeof(game_bot_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    // Recv feedback from Server
    if (recv(socket_fd, feedback, sizeof(feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    // Handling feedback
    switch (atoi(feedback))
    {
    case 1:
        break;
    default:
        printf("[-]Something wrong with server\n");
        return;
    }

    // Initialise board
    initialise_board(game.board.board);

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

        // Get user move
        move = get_player_move(game.board.board, side);
        make_move(game.board.board, move, side);

        // Create next move
        next_move.account = current_user;
        next_move.move = move;
        game.moves[game.number_of_moves] = next_move;
        game.number_of_moves = game.number_of_moves + 1;

        // Send game to Server
        if (send(socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Recv game from Server
        if (recv(socket_fd, &game, sizeof(struct _game), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
    }
}

void find_player(int socket_fd, Account *current_user)
{
    char find_player_signal[BUFFER_SIZE] = "5\0";
    char feedback[BUFFER_SIZE];
    Game game;

    // Send game bot signal to Server
    if (send(socket_fd, find_player_signal, sizeof(find_player_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    // Recv feedback from Server
    if (recv(socket_fd, feedback, sizeof(feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    // Handling feedback
    switch (atoi(feedback))
    {
    case 1:
        break;
    default:
        printf("[-]Something wrong with server\n");
        return;
    }

    // Send current user username to Server
    if (send(socket_fd, current_user, sizeof(struct _Account), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    printf("[+]Finding other player...\n");

    // Recv game from Server
    if (recv(socket_fd, &game, sizeof(struct _game), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    play_with_player(socket_fd, *current_user, game);

    return;
}

int get_side(Game game, Account current_user)
{
    printf("[+]%s %s\n", current_user.username, game.first_player.username);
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
    char game_bot_signal[BUFFER_SIZE] = "6\0";
    Move next_move;
    char feedback[BUFFER_SIZE];
    int second_player_wait = 1;

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

        // Send game bot signal to Server
        if (send(socket_fd, game_bot_signal, sizeof(game_bot_signal), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Recv feedback from Server
        if (recv(socket_fd, feedback, sizeof(feedback), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Handling feedback
        switch (atoi(feedback))
        {
        case 1:
            break;
        default:
            printf("[-]Something wrong with server\n");
            return;
        }

        // Send game to Server
        if (send(socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

    wait:
        print_board(game.board.board, current_user);
        printf("[+]Waiting for opponent...\n");

        // Recv game from Server
        if (recv(socket_fd, &game, sizeof(struct _game), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
    }
}