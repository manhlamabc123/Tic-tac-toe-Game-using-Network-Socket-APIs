#include "account.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include "../exception/exception.h"
#include <errno.h>
#define BUFFER_SIZE 1024

int number_of_account;

void print_account_info(Account *user)
{
    printf("[+]Account's username: %s\n", user->username);
    printf("[+]Account's password: %s\n", user->password);
    printf("[+]Account's socket fd: %d\n", user->socket_fd);
    printf("[+]Account's sign_in_status: %d\n", user->status);

    return;
}

int check_password(Account *account, char *username, char *password)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if ((strcmp(cur->username, username) == 0) && (strcmp(cur->password, password) == 0))
        {
            return 0;
        }
        cur = cur->next;
    }
    return 1;
}

int check_signed_in(Account *account, char *username)
{
    Account *cur = account;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            return cur->status;
        }
        cur = cur->next;
    }
}

Account *read_account(Account *acc)
{
    char username[30];
    char password[30];
    number_of_account = 0;
    MYSQL *connect;

    if ((connect = connect_to_database()) == NULL)
    {
        printf("[-]Fail to connect to database\n");
        return NULL;
    }
    if ((acc = database_read_all_accounts(connect)) == NULL)
    {
        printf("[-]Error: database_read_all_accounts\n");
        return NULL;
    }

    // Close connection
    mysql_close(connect);

    return acc;
}

Account *account_sign_up(int client_fd, Account *acc)
{
    printf("[+]Sign up function.\n");

    char feedback[BUFFER_SIZE];
    Account user;

    // Get username from Client
    if (recv(client_fd, &user, sizeof(struct _account), MSG_WAITALL) < 0) // If fail
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return NULL;
    }

    standardize_input(user.username, sizeof(user.username));
    printf("[+]Client username: %s\n", user.username);
    standardize_input(user.password, sizeof(user.password));
    printf("[+]Client password: %s\n", user.password);

    // Add account to account list
    acc = add_account(acc, user.username, user.password); // Add account
    if (acc == NULL)
    {
        printf("[-]Fail to sign up\n");
        sprintf(feedback, "%d", 0);
    }
    else
    {
        number_of_account++;
        printf("[+]Sign up successful.\n");
        sprintf(feedback, "%d", 1);

        // Connect to database
        MYSQL *connect = connect_to_database();
        if (connect == NULL)
        {
            printf("[-]Fail to connect to database\n");
            return NULL;
        }

        // Update database
        if (database_add_new_user(connect, user.username, user.password) == 0)
        {
            printf("[-]Fail to update database\n");
            return NULL;
        }

        // Close connection
        mysql_close(connect);
    }

    // Send feedback to Client
    if (send(client_fd, feedback, sizeof(feedback), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return NULL;
    }

    return acc;
}

void update_file(Account *acc)
{
    FILE *inp = fopen("data/nguoidung.txt", "w+");
    if (inp == NULL)
    {
        printf("Cannot open file.\n");
        return;
    }

    Account *cur = acc;
    while (cur != NULL)
    {
        fprintf(inp, "%s %s\n", cur->username, cur->password);
        cur = cur->next;
    }
    fclose(inp);
}

void account_sign_in(int client_fd, Account *acc)
{
    printf("[+]Sign in function.\n");

    char feedback[BUFFER_SIZE];
    Account user;

    // Get user from Client
    if (recv(client_fd, &user, sizeof(struct _account), MSG_WAITALL) < 0) // If fail
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }

    standardize_input(user.username, sizeof(user.username));
    standardize_input(user.password, sizeof(user.password));
    printf("[+]Client username: %s\n", user.username);
    printf("[+]Client password: %s\n", user.password);

    if (check_user(acc, user.username) != 0)
    {
        // Send feedback to Client
        sprintf(feedback, "%d", 1);
        if (send(client_fd, &feedback, sizeof(feedback), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    if (check_signed_in(acc, user.username) != 0)
    {
        // Send feedback to Client
        sprintf(feedback, "%d", 3);
        if (send(client_fd, &feedback, sizeof(feedback), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    if (check_password(acc, user.username, user.password) != 0)
    {
        // Send feedback to Client
        sprintf(feedback, "%d", 2);
        if (send(client_fd, &feedback, sizeof(feedback), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }

        return;
    }

    // Update account list
    printf("[+]Sign in is successful to: %s\n", user.username);
    Account *cur = acc;
    while (cur != NULL)
    {
        if (strcmp(cur->username, user.username) == 0)
        {
            cur->status = 1;
            cur->socket_fd = client_fd;
            printf("[+]Client socket: %d\n", cur->socket_fd);

            // Send feedback to Client
            sprintf(feedback, "%d", 0);
            if (send(client_fd, &feedback, sizeof(feedback), 0) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }
        }
        cur = cur->next;
    }

    return;
}

Account *account_search(Account *acc, Account user)
{
    printf("[+]Search account function\n");

    if (check_signed_in(acc, user.username) == 0)
    {
        printf("Yet signed in.\n");
        return NULL;
    }

    Account *cur = acc;
    while (cur != NULL)
    {
        if (strcmp(cur->username, user.username) == 0)
        {
            print_account_info(cur);
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

void account_log_out(int client_fd, Account *acc)
{
    char log_out_feedback[BUFFER_SIZE];
    char username[BUFFER_SIZE];

    // Get username from Client
    if (recv(client_fd, username, sizeof(username), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return;
    }
    standardize_input(username, sizeof(username));

    // Check for Account's existence
    if (check_user(acc, username) != 0)
    {
        printf("[-]Account does not exist!\n");
        sprintf(log_out_feedback, "%d", 1);
        if (send(client_fd, log_out_feedback, sizeof(log_out_feedback), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    // Check for Account's sign in status
    if (check_signed_in(acc, username) == 0)
    {
        printf("[-]Yet signed in.\n");
        sprintf(log_out_feedback, "%d", 2);
        if (send(client_fd, log_out_feedback, sizeof(log_out_feedback), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    // Update Account list
    Account *cur = acc;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            cur->status = 0;
            printf("[+]%s logged out.\n", username);
            sprintf(log_out_feedback, "%d", 0);
            // Send feedback to Client
            if (send(client_fd, log_out_feedback, sizeof(log_out_feedback), 0) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }
            return;
        }
        cur = cur->next;
    }
    return;
}

void free_list(Account *head)
{
    Account *tmp;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int check_activate_code(char *activate_code, char *correct_activate_code)
{
    return strcmp(activate_code, correct_activate_code);
}

int change_password(Account *acc, char *username, char *new_password)
{
    int success = 0;
    Account *cur = acc;
    while (cur != NULL)
    {
        if (strcmp(cur->username, username) == 0)
        {
            strcpy(cur->password, new_password);
            success = 1;
        }
        cur = cur->next;
    }
    update_file(acc);
    return success;
}