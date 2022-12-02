#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"
#include "../exception/exception.h"
#include "../account/account.h"
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

int exit()
{
    char user_choice[BUFFER_SIZE];

    printf("-------------Exit----------\n");
    printf("Do you really really want to exit?(y/n): ");
goal:
    if (fgets(user_choice, sizeof(user_choice), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }

    // Check input
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
    char control_signal[BUFFER_SIZE] = "sign in\0"; // for now

    printf("---------Sign up-----------\n");
goal:
    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }
    if (check_spaces(username, sizeof(username)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal;
    }
goal1:
    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }
    if (check_spaces(password, sizeof(password)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal1;
    }
goal2:
    printf("New password: ");
    if (fgets(confirm_password, sizeof(confirm_password), stdin) == NULL)
    {
        printf("Input error.\n");
        return 0;
    }
    if (check_spaces(confirm_password, sizeof(confirm_password)))
    {
        printf("Contain white scape(s). Try again.\n");
        goto goal2;
    }
    if (check_confirm_password(confirm_password, password))
    {
        goto goal2;
    }

    send(socket_fd, control_signal, sizeof(control_signal), 0);
    send(socket_fd, username, sizeof(username), 0);
    send(socket_fd, confirm_password, sizeof(confirm_password), 0);
    return 1;
}