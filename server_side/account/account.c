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

Account *account_sign_up(int client_fd, Account *acc, Account user)
{
    printf("[+]Sign up function.\n");

    Message message;

    standardize_input(user.username, sizeof(user.username));
    printf("[+]Client username: %s\n", user.username);
    standardize_input(user.password, sizeof(user.password));
    printf("[+]Client password: %s\n", user.password);

    if (check_user(acc, user.username) == 0)
    {
        // Send feedback to Client
        message.header = ERROR;
        strcpy(message.message, "Account already existed");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return NULL;
        }
        return NULL;
    }

    // Add account to account list
    acc = add_account(acc, user.username, user.password); // Add account
    if (acc == NULL)
    {
        printf("[-]Fail to sign up\n");
        message.header = ERROR;
        strcpy(message.message, "Fail to sign up");
    }
    else
    {
        number_of_account++;
        printf("[+]Sign up successful\n");
        message.header = OK;
        strcpy(message.message, "Sign up successful");

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
    if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return NULL;
    }

    return acc;
}

void account_sign_in(int client_fd, Account *acc, Account user)
{
    printf("[+]Sign in function.\n");

    Message message;

    standardize_input(user.username, sizeof(user.username));
    standardize_input(user.password, sizeof(user.password));
    printf("[+]Client username: %s\n", user.username);
    printf("[+]Client password: %s\n", user.password);

    if (check_user(acc, user.username) != 0)
    {
        // Send feedback to Client
        message.header = ERROR;
        strcpy(message.message, "Cannot find account");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    if (check_signed_in(acc, user.username) != 0)
    {
        // Send feedback to Client
        message.header = ERROR;
        strcpy(message.message, "Already signed in");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    if (check_password(acc, user.username, user.password) != 0)
    {
        // Send feedback to Client
        message.header = ERROR;
        strcpy(message.message, "Password is incorrect");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
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
            message.header = OK;
            if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return;
            }
        }
        cur = cur->next;
    }

    return;
}

Account *account_search_by_account(Account *acc, Account user)
{
    printf("[+]Search account function by Account\n");

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

void account_log_out(int client_fd, Account *acc, Account user)
{
    Message message;

    standardize_input(user.username, sizeof(user.username));

    // Check for Account's existence
    if (check_user(acc, user.username) != 0)
    {
        printf("[-]Account does not exist\n");
        message.header = ERROR;
        strcpy(message.message, "Account does not exist");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return;
        }
        return;
    }

    // Check for Account's sign in status
    if (check_signed_in(acc, user.username) == 0)
    {
        printf("[-]Yet signed in\n");
        message.header = ERROR;
        strcpy(message.message, "Yet signed in");
        if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
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
        if (strcmp(cur->username, user.username) == 0)
        {
            cur->status = 0;
            printf("[+]%s logged out.\n", user.username);
            message.header = OK;
            if (send(client_fd, &message, sizeof(struct _message), 0) < 0)
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

Account* account_search_by_socketfd(Account* acc, int socket_fd)
{
    printf("[+]Search account function by socketfd\n");

    Account *cur = acc;
    while (cur != NULL)
    {
        if (cur->socket_fd == socket_fd)
        {
            print_account_info(cur);
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}