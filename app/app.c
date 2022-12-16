#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "app.h"
#include "../account/account.h"
#include "../exception/exception.h"
#include "../helper/helper.h"
#define BUFFER_SIZE 1024

void client_app(int socket_fd)
{
	// Clear scene
	system("clear");

	char username[BUFFER_SIZE];
	char password[BUFFER_SIZE];
	char sign_in_feedback[BUFFER_SIZE];
	int n;
	char choice[BUFFER_SIZE];
	char bye[BUFFER_SIZE] = "bye\n\0";
	char new_password[BUFFER_SIZE];
	char confirm_password[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	char only_number[BUFFER_SIZE];
	char only_string[BUFFER_SIZE];
	char sign_out_request[100] = "bye\0";

	switch (welcome())
	{
	case 'y':
		break;
	case 'n':
		printf("This is sign in option.\n");
		// sign_in();
		break;
	case 'b':
		return;
	default:
		printf("Program error.\n");
		return;
	}

	for (;;)
	{
	goal0:
		// Clean buffers
		bzero(username, sizeof(username));
		bzero(password, sizeof(password));

		// Get user's input
		printf("Username: ");
		if (fgets(username, sizeof(username), stdin) == NULL)
			break;

		// Check for exception
		if (check_spaces(username, strlen(username)))
		{
			printf("Username contains white space. Please enter again.\n");
			goto goal0;
		}
	goal1:
		printf("Password: ");
			if (fgets(password, sizeof(username), stdin) == NULL)
				break;

		// Check for '\n' input
		if (strcmp(username, "\n") == 0 && strcmp(password, "\n") == 0)
		{
			printf("Exit Program.\n");
			char exit_program[100] = "exit_program\0";
			write(socket_fd, exit_program, strlen(exit_program));
			break;
		}

		// Check for scape
		if (check_spaces(password, strlen(password)))
		{
			printf("Password contains white space. Please enter again.\n");
			goto goal1;
		}

		// Send username & password to server
		write(socket_fd, username, sizeof(username));
		write(socket_fd, password, sizeof(password));

		// Sign in response
		read(socket_fd, sign_in_feedback, BUFFER_SIZE);
		switch (atoi(sign_in_feedback))
		{
		case 0:
			printf("Signed in successfully.\n");
			printf("------------------\n");
			switch (menu())
			{
			case 1:
				printf("Choice: Tutorial\n");
				break;
			case 2:
				printf("Choice: Play\n");
				break;
			case 3:
				printf("Choice: Log out\n");
				break;
			default:
				break;
			}
			break;
		case 1:
			printf("Cannot find account.\n");
			break;
		case 2:
			printf("Account is not ready.\n");
			break;
		case 3:
			printf("Wrong password.\n");
			break;
		case 4:
			printf("Wrong password. Account is blocked.\n");
			break;
		default:
			break;
		}

		printf("---------------------\n");
	}
}

void server_app(int connect_fd)
{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int n;
    char sign_in_feedback[BUFFER_SIZE];
    Account *acc = NULL;
    acc = read_account(acc);
    int feedback;
    int password_incorrect_times = 3;
    char bye[100] = "bye\0";
    char is_password_changing[BUFFER_SIZE];
    char only_number[BUFFER_SIZE];
    char only_string[BUFFER_SIZE];
    char exit_program[100] = "exit_program\0";

    while(1)
    {
        // Clean buffers
        bzero(username, sizeof(username));
        bzero(password, sizeof(password));

        // Receive username & password from client_address
        read(connect_fd, username, sizeof(username));
        read(connect_fd, password, sizeof(password));

        // Standardize strings
        standardize_input(username, sizeof(username));
        standardize_input(password, sizeof(password));

        // Check for exit program
        if (strcmp(exit_program, username) == 0)
            break;

        // Print username & password
        printf("Username: %s\n", username);
        printf("Password: %s\n", password);

        // Sign in
        feedback = sign_in(acc, username, password);
        if (feedback == 3) // If wrong password
        {
            password_incorrect_times--;
            if (password_incorrect_times == 0)
            {
                change_current_account_status(acc, username, 2);

                feedback++; // 4 mean account is blocked
            }
        }

        sprintf(sign_in_feedback, "%d", feedback);
        write(connect_fd, sign_in_feedback, sizeof(sign_in_feedback));

        if (feedback == 0) // If signed in
        {
            read(connect_fd, is_password_changing, sizeof(is_password_changing));
            standardize_input(is_password_changing, sizeof(is_password_changing));

            if (strcmp(bye, is_password_changing) == 0)
            {
                if (sign_out(acc, username))
                {
                    write(connect_fd, bye, sizeof(bye));
                }
            }
            else if (strlen(is_password_changing) > 1)
            {
                if (change_password(acc, username, is_password_changing))
                {
                    write(connect_fd, sign_in_feedback, sizeof(sign_in_feedback));
                }
            }
        }

        printf("---------------------\n");
    }
}