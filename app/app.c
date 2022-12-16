#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
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
		sign_in(socket_fd, sign_in_feedback, sizeof(sign_in_feedback));
		break;
	case 'n':
		sign_up(socket_fd);
		break;
	case 'b':
		if (program_exit(socket_fd))
			return;
	default:
		printf("[-]Program error.\n");
		return;
	}

	while (1)
	{
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

void *server_app(void *arg)
{
	pthread_detach(pthread_self());
	int client_fd = (int)arg;
	char client_signal[BUFFER_SIZE];
	Account* account = NULL;
	account = read_account(account);

	bzero(client_signal, sizeof(client_signal));
	if (recv(client_fd, client_signal, sizeof(client_signal), 0) < 0)
	{
		printf("[-]Fail to receive client message.\n");
	}
	else
	{
		standardize_input(client_signal, sizeof(client_signal));
		printf("[+]Client message: %s\n", client_signal);
	}

	switch (atoi(client_signal))
	{
	case 0: //exit
		return NULL;
	case 1: //sign up
		printf("[+]Client trying to sign up.\n");
		account = account_sign_up(client_fd, account);
	default:
		break;
	}
}