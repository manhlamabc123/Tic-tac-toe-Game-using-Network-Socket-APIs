#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "exception/exception.h"
#define BUFFER_SIZE 1024
#define PORT 8080

void func(int sockfd)
{
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
			write(sockfd, exit_program, strlen(exit_program));
			break;
		}

		// Check for scape
		if (check_spaces(password, strlen(password)))
		{
			printf("Password contains white space. Please enter again.\n");
			goto goal1;
		}

		// Send username & password to server
		write(sockfd, username, sizeof(username));
		write(sockfd, password, sizeof(password));

		// Sign in response
		read(sockfd, sign_in_feedback, BUFFER_SIZE);
		switch (atoi(sign_in_feedback))
		{
		case 0:
			printf("OK.\n");
			printf("------------------\n");
		goal2:
			printf("Do you want to change password or sign out?(y/n/bye): ");
			if (fgets(choice, BUFFER_SIZE, stdin) == NULL)
				break;

			if (choice[0] != 121 && choice[0] != 110 && !(strcmp(choice, bye) == 0))
			{
				printf("Wrong input. Only y or n.\n");
				goto goal2;
			}

			if (choice[0] == 121)
			{
			goal3:
				printf("New password: ");
				if (fgets(new_password, BUFFER_SIZE, stdin) == NULL)
					break;

				// Check new_password
				if (check_new_password(new_password))
				{
					printf("Can only contains number or letter. Try again please.\n");
					goto goal3;
				}
			goal4:
				printf("Confirm password: ");
				if (fgets(confirm_password, BUFFER_SIZE, stdin) == NULL)
					break;

				// Check confirm_password
				if (check_confirm_password(confirm_password, new_password))
				{
					goto goal4;
				}

				write(sockfd, confirm_password, sizeof(confirm_password));
				read(sockfd, buffer, BUFFER_SIZE);
				read(sockfd, only_number, BUFFER_SIZE);
				read(sockfd, only_string, BUFFER_SIZE);
				if (atoi(buffer) == 0)
				{
					printf("Encoded password: %s %s\n", only_number, only_string);
					printf("Change password successfully.\n");
				}
			}
			else if (choice[0] == 110)
			{
				char request[2] = "0";
				write(sockfd, request, strlen(request));
			}
			else
			{
				write(sockfd, sign_out_request, strlen(sign_out_request));
				read(sockfd, buffer, BUFFER_SIZE);
				if (strcmp(sign_out_request, buffer) == 0)
				{
					printf("Sign out successfully.\n");
				}
			}
			break;
		case 1:
			printf("Cannot find account.\n");
			break;
		case 2:
			printf("Account is not ready.\n");
			break;
		case 3:
			printf("Not OK.\n");
			break;
		case 4:
			printf("Not OK. Account is blocked.\n");
			break;
		default:
			break;
		}

		printf("---------------------\n");
	}
}

int main(int argc, char *argv[])
{
	// Check input
	if (argc != 3)
	{
		printf("Please input IP address and Port number\n");
		return 0;
	}

	char *ip_address = argv[1];
	char *port_number = argv[2];
	int port = atoi(port_number);
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// Check if address valid
	if (inet_addr(ip_address) == -1)
	{
		printf("Invalid IP address.\n");
		return 0;
	}

	// Check if port valid
	if (port < 1 || port > 65535)
	{
		printf("Invalid port.\n");
		return 0;
	}

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip_address);
	servaddr.sin_port = htons(port);

	// connect the client socket to server socket
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
