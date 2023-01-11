#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"

void finish_with_error(MYSQL *connect)
{
    fprintf(stderr, "%s\n", mysql_error(connect));
    mysql_close(connect);
    return;
}

MYSQL *connect_to_database()
{
    // Check if MySQL is installed
    printf("MySQL client version: %s\n", mysql_get_client_info());

    // Create connection
    MYSQL *connect = mysql_init(NULL);
    if (connect == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(connect));
        return NULL;
    }

    // Connect to database
    if (mysql_real_connect(connect, "localhost", "root", "20194616", "socket_project", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(connect));
        mysql_close(connect);
        return NULL;
    }

    return connect;
}

int database_read_all_accounts(MYSQL *connect)
{
    // Retrieve from database
    if (mysql_query(connect, "SELECT * FROM accounts"))
    {
        finish_with_error(connect);
        return 0;
    }

    // Create result to store query
    MYSQL_RES *result = mysql_store_result(connect);
    if (result == NULL)
    {
        finish_with_error(connect);
        return 0;
    }

    // Get number of fields
    int num_fields = mysql_num_fields(result);

    // Loop through result
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 1; i < num_fields; i++) // start from 1, skip ID field
        {
            printf("%s ", row[i] ? row[i] : "NULL");
        }

        printf("\n");
    }

    // Free result
    mysql_free_result(result);

    return 1;
}

int database_add_new_user(MYSQL *connect, char *username, char* password)
{
    char query[1024];
    sprintf(query, "INSERT INTO accounts(username, password) VALUES('%s', '%s')", username, password);
    printf("Query: %s\n", query);
    if (mysql_query(connect, query))
    {
        finish_with_error(connect);
    }
    return 1;
}