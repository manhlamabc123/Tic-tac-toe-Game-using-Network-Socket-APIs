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

Account *create_new_account(char *username, char *password, int socket_fd)
{
    Account *p = (Account *)malloc(sizeof(struct _Account));
    strcpy(p->username, username);
    strcpy(p->password, password);
    p->socket_fd = socket_fd;
    p->is_signed_in = 0;
    p->next = NULL;
    return p;
}

Account *add_account(Account *account, char *username, char *password, int socket_fd)
{
    if (account == NULL)
    {
        Account *temp = create_new_account(username, password, socket_fd);
        return temp;
    }
    if (check_user(account, username))
    {
        Account *cur = account;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        Account *temp = create_new_account(username, password, socket_fd);
        cur->next = temp;
        return account;
    }
    return NULL;
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

int check_password(Account *account, char* username, char *password)
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
            return cur->is_signed_in;
        }
        cur = cur->next;
    }
}

Account *read_account(Account *acc)
{
    char username[30];
    char password[30];
    number_of_account = 0;

    FILE *inp = fopen("data/nguoidung.txt", "r");
    if (!inp)
    {
        printf("Error: Can't open this file! \n");
        return NULL;
    }

    do
    {
        if (fscanf(inp, "%s %s", username, password) > 0)
        {
            acc = add_account(acc, username, password, -1);
            number_of_account++;
        }
        else
            break;
    } while (1);
    fclose(inp);
    return acc;
}

Account *account_sign_up(int client_fd, Account *acc)
{
    printf("[+]Sign up function.\n");

    char feedback[BUFFER_SIZE];
    Account user;

    // Get username from Client
    if (recv(client_fd, &user, sizeof(struct _Account), MSG_WAITALL) < 0) // If fail
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return NULL;
    }

    standardize_input(user.username, sizeof(user.username));
    printf("[+]Client username: %s\n", user.username);
    standardize_input(user.password, sizeof(user.password));
    printf("[+]Client password: %s\n", user.password);
    printf("[+]Client fd: %d\n", user.socket_fd);

    // Add account to account list
    acc = add_account(acc, user.username, user.password, user.socket_fd); // Add account
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
        update_file(acc); // Update database
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
    if (recv(client_fd, &user, sizeof(struct _Account), MSG_WAITALL) < 0) // If fail
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
            cur->is_signed_in = 1;
            cur->socket_fd = client_fd;

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

void search(Account *acc)
{
    printf("----Welcome to search function.----\n");
    char username[30];

    printf("Input username: ");
    scanf("%s", username);
    if (check_user(acc, username) != 0)
    {
        printf("Account does not exist!\n");
        return;
    }

    if (check_signed_in(acc, username) == 0)
    {
        printf("Yet signed in.\n");
        return;
    }

    return;
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
            cur->is_signed_in = 0;
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