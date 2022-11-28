#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

int menu()
{
    char user_choice[1024];
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
        return -1;
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