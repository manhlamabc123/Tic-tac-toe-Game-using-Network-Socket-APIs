#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"

void finish_with_error(MYSQL *connect)
{
    fprintf(stderr, "%s\n", mysql_error(connect));
    mysql_close(connect);
    return;
}

int check_user(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            return 0;
        }
        cur = cur->next;
    }
    return 1;
}

Account *create_new_account(char *username, char *password)
{
    Account *p = (Account *)malloc(sizeof(struct _Account));
    strcpy(p->username, username);
    strcpy(p->password, password);
    p->socket_fd = -1;
    p->is_signed_in = 0;
    p->next = NULL;
    return p;
}

Account *add_account(Account *account, char *username, char *password)
{
    if (account == NULL)
    {
        Account *temp = create_new_account(username, password);
        return temp;
    }
    if (check_user(account, username))
    {
        Account *cur = account;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        Account *temp = create_new_account(username, password);
        cur->next = temp;
        return account;
    }
    return NULL;
}

MYSQL *connect_to_database()
{
    // Check if MySQL is installed
    printf("[+]MySQL client version: %s\n", mysql_get_client_info());

    // Create connection
    MYSQL *connect = mysql_init(NULL);
    if (connect == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(connect));
        return NULL;
    }

    // Connect to database
    if (mysql_real_connect(connect, LOCAL_HOST, USER, PASSWORD, DATABASE, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(connect));
        mysql_close(connect);
        return NULL;
    }

    return connect;
}

Account *database_read_all_accounts(MYSQL *connect)
{
    Account *account;

    // Retrieve from database
    if (mysql_query(connect, "SELECT * FROM accounts"))
    {
        finish_with_error(connect);
        return NULL;
    }

    // Create result to store query
    MYSQL_RES *result = mysql_store_result(connect);
    if (result == NULL)
    {
        finish_with_error(connect);
        return NULL;
    }

    // Get number of fields
    int num_fields = mysql_num_fields(result);

    // Loop through result
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        printf("[+]Username: %s, Password: %s\n", row[1], row[2]);
        account = add_account(account, row[1], row[2]);
    }

    // Free result
    mysql_free_result(result);

    return account;
}

int database_add_new_user(MYSQL *connect, char *username, char *password)
{
    char query[BUFFER_SIZE];

    sprintf(query, "INSERT INTO accounts(username, password) VALUES('%s', '%s')", username, password);
    printf("[+]Query: %s\n", query);
    if (mysql_query(connect, query))
    {
        finish_with_error(connect);
    }
    return 1;
}

int database_add_new_game(MYSQL *connect, Game game)
{
    char query[BUFFER_SIZE * 5];
    char move[BUFFER_SIZE];
    char one_move[10];
    int player = 0;

    for (int i = 0; i < game.number_of_moves; i++)
    {

        if (strcmp(game.first_player.username, game.moves[i].account.username) == 0)
            player = 1;
        else
            player = 2;

        sprintf(one_move, "%d-%d_", player, game.moves[i].move);

        printf("[+](String) One move: %s\n", one_move);

        strcat(move, one_move);

        bzero(one_move, sizeof(one_move));
    }

    printf("[+](String) Moves: %s\n", move);

    sprintf(query, "INSERT INTO games(date, first_player, second_player, board_size, moves) VALUES('%s', '%s', '%s', %d, '%s')", game.date, game.first_player.username, game.second_player.username, game.board.size, move);
    printf("[+]Query: %s\n", query);
    if (mysql_query(connect, query))
    {
        finish_with_error(connect);
    }
    return 1;
}