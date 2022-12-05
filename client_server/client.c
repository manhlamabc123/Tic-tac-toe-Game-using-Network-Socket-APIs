#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "../exception/exception.h"
#include "../helper/helper.h"
#define BUFFER_SIZE 1024
#define PORT 8080

void func(int socket_fd)
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

int main(int argc, char *argv[])
{
	// Check input
	if (argc != 3)
	{
		printf("[-]Please input IP address and Port number\n");
		return 0;
	}

	// Variables
	char *ip_address = argv[1];
	char *port_number = argv[2];
	int port = atoi(port_number);
	int socket_fd;
	struct sockaddr_in server_address;

	// Check if address valid
	if (inet_addr(ip_address) == -1)
	{
		printf("[-]Invalid IP address\n");
		return 0;
	}

	// Check if port valid
	if (port < 1 || port > 65535)
	{
		printf("[-]Invalid port\n");
		return 0;
	}

	// socket create and verification
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		printf("[-]Socket creation failed\n");
		exit(0);
	}
	else
		printf("[+]Socket successfully created\n");

	// Clear server_address
	bzero(&server_address, sizeof(server_address));

	// server_address's info
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip_address);
	server_address.sin_port = htons(port);

	// Connect the client socket to server socket
	if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
	{
		printf("[-]Connection with the server failed\n");
		exit(0);
	}
	else
		printf("[+]Connected to the server\n");

	// Main function
	func(socket_fd);

	// Close socket
	close(socket_fd);
	printf("[+]Exit program\n");
	
	return 0;
}
