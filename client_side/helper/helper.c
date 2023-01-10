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

int menu()
{
    // system("clear");

    char user_choice[BUFFER_SIZE];
    int choice;

    printf("[+]MENU:\n");
    printf("[+]1. Tutorial\n");
    printf("[+]2. Play\n");
    printf("[+]3. Log out\n");
menu_choice:
    printf("[+]Your choice: ");
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check input
    if (strlen(user_choice) <= 2)
    {
        choice = atoi(user_choice);
        if (choice <= 0 || choice >= 4)
        {
            printf("[-]Invalid choice\n[+]Try again\n");
            goto menu_choice;
        }
    }
    else
    {
        printf("[-]Invalid choice\n[+]Try again\n");
        goto menu_choice;
    }

    return choice;
}

char welcome()
{
    char user_choice[BUFFER_SIZE];

    printf("[+]Welcome\n");
    printf("[+]Do you have an account?(y/n/bye): ");
welcome_choice:
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check input
    if (check_yes_no_bye(user_choice))
    {
        printf("[-]Invalid choice\n[+]Try again\n");
        goto welcome_choice;
    }

    return user_choice[0];
}

int program_exit(int socket_fd)
{
    char user_choice[BUFFER_SIZE];
    char program_exit_signal[BUFFER_SIZE] = "0\0"; // 0 = exit
    char server_feedback[BUFFER_SIZE];

    printf("[+]Exit\n");
    printf("[+]Do you really really want to exit?(y/n): ");
user_choice:
    // Get user's choice
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check user's input
    if (check_yes_no(user_choice))
    {
        printf("[-]Invalid choice\n[+]Again please: ");
        goto user_choice;
    }

    if (user_choice[0] != 'y')
        return 2;

    // Send exit program signal to Server
    if (send(socket_fd, program_exit_signal, sizeof(program_exit_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    printf("[+]Exiting program...\n");
    return 1;
}

int sign_up(int socket_fd)
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char confirm_password[BUFFER_SIZE];
    char sign_up_signal[BUFFER_SIZE] = "1\0";
    char server_feedback[BUFFER_SIZE];
    Account new_user;

    // Send sign up signal to server
    if (send(socket_fd, sign_up_signal, sizeof(sign_up_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Recv feedback from Server
    if (recv(socket_fd, server_feedback, sizeof(server_feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Handling feedback
    switch (atoi(server_feedback))
    {
    case 1:
        break;
    default:
        printf("[-]Something wrong with server\n");
        return 0;
    }

    printf("[+]Sign up\n");
username:
    // Get username
    printf("[+]Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check username
    if (check_spaces(username, sizeof(username)))
    {
        printf("[-]Contain white scape(s). Try again.\n");
        goto username;
    }
new_password:
    // Get new_password
    printf("[+]New password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check password
    if (check_spaces(password, sizeof(password)))
    {
        printf("[-]Contain white scape(s). Try again.\n");
        goto new_password;
    }
confirm_password:
    // Get confirm password
    printf("[+]Confirm password: ");
    if (fgets(confirm_password, sizeof(confirm_password), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check confirm password
    if (check_spaces(confirm_password, sizeof(confirm_password)))
    {
        printf("[-]Contain white scape(s). Try again.\n");
        goto confirm_password;
    }

    // Check if confirm password and password are the same
    if (check_confirm_password(confirm_password, password))
    {
        goto confirm_password;
    }

    // Create user with new password
    strcpy(new_user.username, username);
    strcpy(new_user.password, confirm_password);
    new_user.socket_fd = socket_fd;

    // Send username & password to Server
    if (send(socket_fd, &new_user, sizeof(struct _Account), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Recv feedback from Server
    if (recv(socket_fd, server_feedback, sizeof(server_feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Handling feedback
    switch (atoi(server_feedback))
    {
    case 0:
        printf("[-]Fail to sign up\n");
        return 0;
    case 1:
        printf("[+]Sign up successfully\n");
        break;
    default:
        printf("[-]Something wrong with server\n");
        return 0;
    }

    return 1;
}

int sign_in(int socket_fd, Account *current_user, int sizeof_current_user)
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char sign_in_signal[BUFFER_SIZE] = "2\0";
    char server_feedback[BUFFER_SIZE];

    // Send sign in signal to server
    if (send(socket_fd, sign_in_signal, sizeof(sign_in_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Recv feedback from Server
    if (recv(socket_fd, server_feedback, sizeof(server_feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Handling feedback
    switch (atoi(server_feedback))
    {
    case 1:
        break;
    default:
        printf("[-]Something wrong with server\n");
        return 0;
    }

    printf("[+]Sign in\n");

username:
    // Clean buffers
    bzero(username, sizeof(username));
    bzero(password, sizeof(password));

    // Get username's input
    printf("[+]Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check username
    if (check_spaces(username, strlen(username)))
    {
        printf("[-]Contain white scape(s). Try again.\n");
        goto username;
    }
password:
    // Get password's input
    printf("[+]Password: ");
    if (fgets(password, sizeof(username), stdin) == NULL)
    {
        printf("[-]Error: fgets\n");
        return 0;
    }

    // Check password
    if (check_spaces(password, strlen(password)))
    {
        printf("[-]Contain white scape(s). Try again.\n");
        goto password;
    }

    // Create current_user
    standardize_input(username, sizeof(username));
    standardize_input(password, sizeof(password));
    strcpy(current_user->username, username);
    strcpy(current_user->password, password);
    current_user->socket_fd = socket_fd;

    // Send current_user to Server
    if (send(socket_fd, current_user, sizeof(struct _Account), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Get sign in feedback
    if (recv(socket_fd, server_feedback, sizeof(server_feedback), MSG_WAITALL) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    // Handling feedback
    switch (atoi(server_feedback))
    {
    case 0: // Sign in success
        printf("[+]Signed in successfully\n");
        printf("[+]Hello, %s\n", current_user->username);
        break;
    case 1: // Sign in fail
        printf("[-]Cannot find account\n");
        return 2;
    case 2: // Sign in fail
        printf("[-]Wrong password\n");
        return 2;
    default: // Sign in fail
        printf("[-]Something wrong with server\n");
        return 0;
    }

    return 1;
}

int log_out(int socket_fd, char *username, int sizeof_username)
{
    char log_out_signal[BUFFER_SIZE] = "3\0";
    char feedback[BUFFER_SIZE];

    // Send log out signal to Server
    if (send(socket_fd, log_out_signal, sizeof(log_out_signal), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Receive server feedback
    if (recv(socket_fd, feedback, sizeof(feedback), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Handling feedback
    switch (atoi(feedback))
    {
    case 1:
        break;
    default:
        printf("[-]Something wrong with server\n");
        return 0;
    }

    // Get username, who is logging out
    if (send(socket_fd, username, sizeof_username, 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    // Receive server feedback
    if (recv(socket_fd, feedback, sizeof(feedback), 0) < 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 1;
    }

    switch (atoi(feedback))
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
        printf("[-]Undefined feedback.\n");
        return 0;
    }

    return 1;
}