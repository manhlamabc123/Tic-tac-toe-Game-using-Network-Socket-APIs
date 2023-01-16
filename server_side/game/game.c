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

void print_board(const int *board)
{
    int i = 0;
    char pceChars[] = "OX|-";
    printf("[+]Board:\n");
    for (i = 0; i < 9; ++i)
    {
        if (i != 0 && i % 3 == 0)
        {
            printf("\n\n");
        }
        printf("%4c", pceChars[board[convert_to_25[i]]]);
    }
}

void print_game(Game *game)
{
    printf("[+]Game's date: %s\n", game->date);
    printf("[+]Game's first player: %s\n", game->first_player.username);
    printf("[+]Game's second player: %s\n", game->second_player.username);
    printf("[+]Last move: %s - %d\n", game->moves[game->number_of_moves - 1].account.username, game->moves[game->number_of_moves - 1].move);
    printf("[+]Game's Status: %d\n", game->status);
    print_board(game->board.board);
    printf("\n");

    return;
}

void server_game_bot(int client_fd)
{
    // Initialise variables
    Game game;
    int move;
    int side;
    char bot_name[BUFFER_SIZE] = "bot\0";
    Account bot;
    Move next_move;
    strcpy(bot.username, bot_name);

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
        
        // Save game to database
        // Connect to database
        MYSQL *connect = connect_to_database();
        if (connect == NULL)
        {
            printf("[-]Fail to connect to database\n");
            return;
        }

        // Update database
        if (database_add_new_game(connect, game) == 0)
        {
            printf("[-]Fail to update database\n");
            return;
        }

        // Close connection
        mysql_close(connect);
        return;
    }

    // Print game's info
    game.second_player = bot;
    print_game(&game);

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
        // Save game to database
        // Connect to database
        MYSQL *connect = connect_to_database();
        if (connect == NULL)
        {
            printf("[-]Fail to connect to database\n");
            return;
        }

        // Update database
        if (database_add_new_game(connect, game) == 0)
        {
            printf("[-]Fail to update database\n");
            return;
        }

        // Close connection
        mysql_close(connect);
        return;
    }

    return;
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

int initialise_game(Game *in_waiting_game, Account *acc, Account current_user)
{
    int return_value = 0;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time[BUFFER_SIZE];
    Account *player;

    // Get current time
    snprintf(time, sizeof(time), "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Set game date
    strcpy(in_waiting_game->date, time);

    // Search player using current user
    player = account_search(acc, current_user);
    if (player == NULL)
    {
        printf("[-]Cannot find this user\n");
        return return_value;
    }

    // Get first player
    if (in_waiting_game->status == -1)
    {
        in_waiting_game->first_player = *player;
    }
    else
    {
        in_waiting_game->second_player = *player;
        return_value = 1;
    }

    // Set game variables
    in_waiting_game->status = PROCESS;
    in_waiting_game->number_of_moves = 0;

    // Initialise board
    in_waiting_game->board.size = 3;
    initialise_board(in_waiting_game->board.board);

    return return_value;
}

int find_player(int client_fd, Game *in_waiting_game, Account *acc)
{
    Account current_user;
    char feedback[BUFFER_SIZE];

    // Recv current user username to Server
    if (recv(client_fd, &current_user, sizeof(struct _account), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return -1;
    }

    // Setup game state
    if (initialise_game(in_waiting_game, acc, current_user))
    {
        print_game(in_waiting_game);
        printf("[+]Found 2 player for this room\n");
        if (send(client_fd, in_waiting_game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return -1;
        }

        if (send(in_waiting_game->first_player.socket_fd, in_waiting_game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return -1;
        }

        return 1;
    }

    return 0;
}

void player_vs_player(int client_fd)
{
    Game game;
    int side;

    // Recv game from Server
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
        // Send game to both Client
        if (send(game.first_player.socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        if (send(game.second_player.socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        // Save game to database
        // Connect to database
        MYSQL *connect = connect_to_database();
        if (connect == NULL)
        {
            printf("[-]Fail to connect to database\n");
            return;
        }

        // Update database
        if (database_add_new_game(connect, game) == 0)
        {
            printf("[-]Fail to update database\n");
            return;
        }

        // Close connection
        mysql_close(connect);
        return;
    }

    // Send game to Client
    if (game.first_player.socket_fd == client_fd)
    {
        if (send(game.second_player.socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
    }
    else
    {
        if (send(game.first_player.socket_fd, &game, sizeof(struct _game), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
    }

    return;
}