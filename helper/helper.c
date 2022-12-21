#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <errno.h>
#include "helper.h"
#include "../exception/exception.h"
#define BUFFER_SIZE 1024

int menu()
{
    char user_choice[BUFFER_SIZE];
    int choice;

    printf("----------MENU----------\n");
    printf("1. Tutorial\n");
    printf("2. Play\n");
    printf("3. Log out\n");
goal:
    printf("Your choice: ");
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check input
    if (strlen(user_choice) <= 2)
    {
        choice = atoi(user_choice);
        if (choice <= 0 || choice >= 4)
        {
            printf("Invalid choice. Try again.\n");
            goto goal;
        }
    }
    else
    {
        printf("Invalid choice. Try again.\n");
        goto goal;
    }

    return choice;
}

char welcome()
{
    char user_choice[BUFFER_SIZE];

    printf("---------Welcome-----------\n");
    printf("Do you have an account?(y/n/bye): ");
goal:
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check input
    if (check_yes_no_bye(user_choice))
    {
        printf("Invalid choice. Again please: ");
        goto goal;
    }

    return user_choice[0];
}

int program_exit(int socket_fd)
{
    char user_choice[BUFFER_SIZE];
    char program_exit_signal[BUFFER_SIZE] = "0\0"; // 0 = exit

    // Send exit program signal to Server
    if (send(socket_fd, program_exit_signal, sizeof(program_exit_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    printf("-------------Exit----------\n");
    printf("Do you really really want to exit?(y/n): ");
goal:
    // Get user's choice
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check user's input
    if (check_yes_no(user_choice))
    {
        printf("Invalid choice. Again please: ");
        goto goal;
    }

    if (user_choice[0] == 'y')
        return 1;
    else
        return 0;
}

int sign_up(int socket_fd)
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char confirm_password[BUFFER_SIZE];
    char sign_up_signal[BUFFER_SIZE] = "1\0"; // for now

    // Send sign up signal to server
    if (send(socket_fd, sign_up_signal, sizeof(sign_up_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    printf("---------Sign up-----------\n");
goal:
    // Get username
    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check username
    if (check_spaces(username, sizeof(username)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal;
    }
goal1:
    // Get password
    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check password
    if (check_spaces(password, sizeof(password)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal1;
    }
goal2:
    // Get confirm password
    printf("Confirm password: ");
    if (fgets(confirm_password, sizeof(confirm_password), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check confirm password
    if (check_spaces(confirm_password, sizeof(confirm_password)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal2;
    }

    // Check if confirm password and password are the same
    if (check_confirm_password(confirm_password, password))
    {
        printf("[-]Incorrect confirm password\n");
        goto goal2;
    }

    // Send username & password to Server
    if (send(socket_fd, username, sizeof(username), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }
    if (send(socket_fd, confirm_password, sizeof(confirm_password), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    return 1;
}

int sign_in(int socket_fd, SignInFeedback *sign_in_feedback, int sizeof_sign_in_feedback)
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char sign_in_signal[BUFFER_SIZE] = "2\0";

    // Send sign in signal to server
    if (send(socket_fd, sign_in_signal, sizeof(sign_in_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

goal0:
    bzero(username, sizeof(username));
    bzero(password, sizeof(password));

    // Get username's input
    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
        return 1;

    // Check username
    if (check_spaces(username, strlen(username)))
    {
        printf("Username contains white space. Please enter again.\n");
        goto goal0;
    }
goal1:
    // Get password's input
    printf("Password: ");
    if (fgets(password, sizeof(username), stdin) == NULL)
        return 1;

    // Check password
    if (check_spaces(password, strlen(password)))
    {
        printf("Password contains white space. Please enter again.\n");
        goto goal1;
    }

    // Send username & password to server
    if (send(socket_fd, username, sizeof(username), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }
    if (send(socket_fd, password, sizeof(password), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Get sign in feedback
    if (recv(socket_fd, sign_in_feedback, sizeof_sign_in_feedback, MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    return 0;
}

int log_out(int socket_fd, char *username, int sizeof_username)
{
    char log_out_signal[BUFFER_SIZE] = "3\0";
    char log_out_feedback[BUFFER_SIZE];

    // Send log out signal to Server
    if (send(socket_fd, log_out_signal, sizeof(log_out_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Get username, who is logging out
    if (send(socket_fd, username, sizeof_username, 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Receive server feedback
    if (recv(socket_fd, log_out_feedback, sizeof(log_out_feedback), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    switch (atoi(log_out_feedback))
    {
    case 0: // Log out successfully
        printf("[+]%s logged out.\n", username);
        return 0;
    case 1: // Account does not exist
        printf("[-]Account does not exist!\n");
        return 0;
    case 2: // Account've yet signed in
        printf("[-]Yet signed in.\n");
        return 0;
    default:
        printf("[-]Undefined log_out_feedback.\n");
        return 0;
    }

    return 1;
}