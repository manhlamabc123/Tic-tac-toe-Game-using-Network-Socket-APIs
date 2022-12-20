#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include "game.h"

/*int board[25] = {
       :,:,:,:,:,
       :,-,-,-,:,
       :,-,-,-,:,
       :,-,-,-,:,
       :,:,:,:,:
};*/

const int InMiddle = 4;
const int Corners[4] = {0, 2, 6, 8};
const int Direction[4] = {1, 5, 4, 6};

const int ConvertTo25[9] = {
    6, 7, 8,
    11, 12, 13,
    16, 17, 18};

int GetNumForDir(int startSq, const int dir, const int *board, const int us)
{
    int found = 0;
    while (board[startSq] != BORDER)
    {
        if (board[startSq] != us)
        {
            break;
        }
        found++;
        startSq += dir;
    }
    return found;
}

int FindThreeInARow(const int *board, const int our_index, const int us)
{
    int Dir = 0, i = 0;
    int threeCount = 1;
    for (i = 0; i < 4; ++i)
    {
        Dir = Direction[i];
        threeCount += GetNumForDir(our_index + Dir, Dir, board, us);
        threeCount += GetNumForDir(our_index + Dir * -1, Dir * -1, board, us);
        if (threeCount == 3)
        {
            break;
        }
        threeCount = 1;
    }
    return threeCount;
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
        board[ConvertTo25[i]] = EMPTY;
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
        printf("%4c", pceChars[board[ConvertTo25[i]]]);
    }
}

int has_empty(const int *board)
{
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
            return 1;
    }
    return 0;
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
        if (board[ConvertTo25[move]] != EMPTY)
        {
            move = -1;
            printf("[-]Square not available\n");
            continue;
        }
        moveOk = 1;
    }
    printf("[+]Making Move: %d\n", (move + 1));

    return ConvertTo25[move];
}

int GetNextBest(const int *board)
{
    int ourMove = ConvertTo25[InMiddle];
    if (board[ourMove] == EMPTY)
    {
        return ourMove;
    }
    ourMove = -1;
    int i = 0;
    for (i = 0; i < 4; ++i)
    {
        ourMove = ConvertTo25[Corners[i]];
        if (board[ourMove] == EMPTY)
        {
            break;
        }
        ourMove = -1;
    }
    return ourMove;
}

int GetWinningMove(int *board, const int side)
{
    int ourMove = -1;
    int winMove = 0;
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
        {
            ourMove = ConvertTo25[i];
            board[ourMove] = side;

            if (FindThreeInARow(board, ourMove, side) == 3)
            {
                winMove = 1;
            }
            board[ourMove] = EMPTY;
            if (winMove == 1)
            {
                return ourMove;
            }
            ourMove = -1;
        }
    }
    return ourMove;
}

int get_bot_move(int *board, const int side)
{
    int num_free = 0;
    int available_moves[9];
    int random_move = 0;

    random_move = GetWinningMove(board, side);
    if (random_move != -1)
    {
        return random_move;
    }

    random_move = GetWinningMove(board, side ^ 1);
    if (random_move != -1)
    {
        return random_move;
    }

    random_move = GetNextBest(board);
    if (random_move != -1)
    {
        return random_move;
    }
    random_move = 0;
    /* available_moves[0]
     */
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
        {
            available_moves[num_free++] = ConvertTo25[i];
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
        printf("[-]Fail to send client message: %s\n", game_bot_signal);
    else
        printf("[+]Success in sending client message: %s\n", game_bot_signal);

    initialise_board(game.board.board);

    while (game.status == PROCESS)
    {
        // Print board
        print_board(game.board.board, current_user);

        // Get user move
        move = get_player_move(game.board.board, side);
        make_move(game.board.board, move, side);

        // Create next move
        next_move.account = current_user;
        next_move.move = move;
        game.moves[game.number_of_moves] = next_move;
        game.number_of_moves = game.number_of_moves + 1;

        // Send game to Server
        send(socket_fd, &game, sizeof(struct _game), 0);

        // Recv game from Server
        recv(socket_fd, &game, sizeof(struct _game), MSG_WAITALL);

        // Print game status
        switch (game.status)
        {
        case WIN:
            printf("[+]%s won - %s lost\n", game.first_player.username, game.second_player.username);
            break;
        case LOSE:
            printf("[+]%s won - %s lost\n", game.second_player.username, game.first_player.username);
            break;
        case DRAW:
            printf("A Draw\n");
            break;
        default:
            break;
        }
    }
}

void server_game_bot(int client_fd, Account *account)
{
    Game game;
    int next_move;
    int side;
    char bot_name[BUFFER_SIZE] = "bot\0";
    Account bot;
    strcpy(bot.username, bot_name);

    while (1)
    {
        // Receive game
        recv(client_fd, &game, sizeof(struct _game), MSG_WAITALL);
        game.second_player = bot;

        // Print game
        printf("[+]Game's date: %s\n", game.date);
        printf("[+]Game's first player: %s\n", game.first_player.username);
        printf("[+]Game's second player: %s\n", game.second_player.username);
        printf("[+]Last move: %s - %d\n", game.moves[game.number_of_moves - 1].account.username, game.moves[game.number_of_moves - 1].move);
        printf("[+]Game's Status: %d\n", game.status);

        // Get side
        side = get_side(game);

        // Get bot move
        next_move = get_bot_move(game.board.board, side);
        make_move(game.board.board, next_move, side);

        // Check win-con
        if (FindThreeInARow(game.board.board, next_move, NOUGHTS ^ 1) == 3)
        {
            printf("[+]Game over\n");
            printf("[+]First player wins\n");
            game.status = WIN;
        }
        if (FindThreeInARow(game.board.board, next_move, CROSSES ^ 1) == 3)
        {
            printf("[+]Game over\n");
            printf("[+]Second player wins\n");
            game.status = LOSE;
        }

        // If board don't have any empty zone, then both draw
        if (!has_empty(game.board.board))
        {
            printf("[+]Game over!\n");
            game.status = DRAW;
            printf("[+]It's a draw!\n");
        }

        send(client_fd, &game, sizeof(struct _game), 0);

        if (game.status != PROCESS)
        {
            printf("[+]Exit to menu\n");
            break;
        }
    }

    return;
}