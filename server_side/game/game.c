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

const int in_middle = 4;
const int corners[4] = {0, 2, 6, 8};
const int direction[4] = {1, 5, 4, 6};

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

int find_three_in_a_row(const int *board, const int our_index, const int us)
{
    int dir = 0, i = 0;
    int three_count = 1;
    for (i = 0; i < 4; ++i)
    {
        dir = direction[i];
        three_count += get_number_for_dir(our_index + dir, dir, board, us);
        three_count += get_number_for_dir(our_index + dir * -1, dir * -1, board, us);
        if (three_count == 3)
        {
            break;
        }
        three_count = 1;
    }
    return three_count;
}

int has_empty(const int *board)
{
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[convert_to_25[i]] == EMPTY)
            return 1;
    }
    return 0;
}

int get_next_best(const int *board)
{
    int our_move = convert_to_25[in_middle];
    if (board[our_move] == EMPTY)
    {
        return our_move;
    }
    our_move = -1;
    int i = 0;
    for (i = 0; i < 4; ++i)
    {
        our_move = convert_to_25[corners[i]];
        if (board[our_move] == EMPTY)
        {
            break;
        }
        our_move = -1;
    }
    return our_move;
}

int get_winning_move(int *board, const int side)
{
    int our_move = -1;
    int win_move = 0;
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[convert_to_25[i]] == EMPTY)
        {
            our_move = convert_to_25[i];
            board[our_move] = side;

            if (find_three_in_a_row(board, our_move, side) == 3)
            {
                win_move = 1;
            }
            board[our_move] = EMPTY;
            if (win_move == 1)
            {
                return our_move;
            }
            our_move = -1;
        }
    }
    return our_move;
}

int get_bot_move(int *board, const int side)
{
    int num_free = 0;
    int available_moves[9];
    int random_move = 0;

    random_move = get_winning_move(board, side);
    if (random_move != -1)
    {
        return random_move;
    }

    random_move = get_winning_move(board, side ^ 1);
    if (random_move != -1)
    {
        return random_move;
    }

    random_move = get_next_best(board);
    if (random_move != -1)
    {
        return random_move;
    }
    random_move = 0;
    
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[convert_to_25[i]] == EMPTY)
        {
            available_moves[num_free++] = convert_to_25[i];
        }
    }
    random_move = (rand() % num_free);
    return available_moves[random_move];
}

void make_move(int *board, const int sq, const int side)
{
    board[sq] = side;
}

int get_side(Game game)
{
    Move last_move = game.moves[game.number_of_moves - 1];
    if (strcmp(game.first_player.username, last_move.account.username) == 0)
        return CROSSES;
    if (strcmp(game.second_player.username, last_move.account.username) == 0)
        return NOUGHTS;
    return -1;
}

void server_game_bot(int client_fd, Account *account)
{
    // Initialise variables
    Game game;
    int move;
    int side;
    char bot_name[BUFFER_SIZE] = "bot\0";
    Account bot;
    Move next_move;
    strcpy(bot.username, bot_name);

    while (1)
    {
        // Receive game from Client
        if (recv(client_fd, &game, sizeof(struct _game), MSG_WAITALL) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Check win-con
        side = get_side(game);
        if (find_three_in_a_row(game.board.board, game.moves[game.number_of_moves - 1].move, side ^ 1) == 3)
        {
            printf("[+]Game over\n");
            if (side == NOUGHTS)
            {
                printf("[+]Second player wins\n");
                game.status = LOSE;
            }
            else
            {
                printf("[+]First player wins\n");
                game.status = WIN;
            }
        }
        if (!has_empty(game.board.board))
        {
            printf("[+]Game over!\n");
            game.status = DRAW;
            printf("[+]It's a draw!\n");
        }

        // Check status
        if (game.status != PROCESS)
        {
            // Send game to Client
            if (send(client_fd, &game, sizeof(struct _game), 0) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }
            printf("[+]Exit to menu\n");
            break; // Exit loop
        }

        // Print game's info
        game.second_player = bot;
        printf("[+]Game's date: %s\n", game.date);
        printf("[+]Game's first player: %s\n", game.first_player.username);
        printf("[+]Game's second player: %s\n", game.second_player.username);
        printf("[+]Last move: %s - %d\n", game.moves[game.number_of_moves - 1].account.username, game.moves[game.number_of_moves - 1].move);
        printf("[+]Game's Status: %d\n", game.status);

        // Get bot move
        move = get_bot_move(game.board.board, side);
        make_move(game.board.board, move, side);

        // Create next move
        next_move.account = bot;
        next_move.move = move;
        game.moves[game.number_of_moves] = next_move;
        game.number_of_moves = game.number_of_moves + 1;

        // Check win-con
        side = get_side(game);
        if (find_three_in_a_row(game.board.board, game.moves[game.number_of_moves - 1].move, side ^ 1) == 3)
        {
            printf("[+]Game over\n");
            if (side == NOUGHTS)
            {
                printf("[+]Second player wins\n");
                game.status = LOSE;
            }
            else
            {
                printf("[+]First player wins\n");
                game.status = WIN;
            }
        }
        if (!has_empty(game.board.board))
        {
            printf("[+]Game over!\n");
            printf("[+]It's a draw!\n");
            game.status = DRAW;
        }

        // Send game to Client
        if (send(client_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Check game status
        if (game.status != PROCESS)
        {
            printf("[+]Exit to menu\n");
            break; // Exit loop
        }
    }

    return;
}