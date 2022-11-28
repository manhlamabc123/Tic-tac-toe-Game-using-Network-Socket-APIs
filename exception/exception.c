#include "exception.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int check_spaces(char *string, int length)
{
    char *temp = string;
    while ((temp = strpbrk(temp, " ")))
        return 1;
    return 0;
}

void standardize_input(char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (*(buffer + i) == 10)
        {
            *(buffer + i) = 0;
            return;
        }
    }
    return;
}

int check_new_password(char *string)
{
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == 10) return 0;
        if ((string[i] >= 0 && string[i] <= 47) || (string[i] >= 58 && string[i] <= 64) || (string[i] >= 91 && string[i] <= 96) || (string[i] >=123))
        {
            return 1;
        }
    }
    return 0;
}

int check_confirm_password(char *confirm_password, char *new_password)
{
    // Check for number and letter
    if (check_new_password(confirm_password)) {
        printf("Can only contains number or letter. Try again please.\n");
        return 1;
    }

    // Check if the same password
    if (strcmp(confirm_password, new_password) != 0) {
        printf("Passwords not the same.\n");
        return 1;
    }

    return 0;
}