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

int get_number_for_dir(int start_square, const int dir, const int *board, const int us)
{
    int found = 0;
    while (board[start_square] != BORDER)
    {
        if (board[start_square] != us)
        {
            break;
        }
        found++;
        start_square += dir;
    }
    return found;
}

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
    printf("[+]Board:\n"); // Change here
    for (i = 0; i < 9; ++i)
    {
        if (i != 0 && i % 3 == 0)
        {
            printf("\n\n"); //
        }
        printf("%4c", pceChars[board[convert_to_25[i]]]);
    }
}

int get_player_move(const int *board, const int side)
{
    char userInput[4];
    int i = 0;
    int moveOk = 0;
    int move = -1;
    while (moveOk == 0)
    {
        printf("\n[+]Please enter a move from 1 to 9: ");
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