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

	char sign_in_feedback[BUFFER_SIZE];
	int is_signed_in = 0;
	char current_username[BUFFER_SIZE];

goal:
	switch (welcome()) // Welcome
	{
	case 'y': // Sign in
		sign_in(socket_fd, sign_in_feedback, sizeof(sign_in_feedback), current_username);
		standardize_input(current_username, sizeof(current_username));
		switch (atoi(sign_in_feedback))
		{
		case 0: // Sign in success
			printf("[+]Signed in successfully: %s\n", current_username);
			is_signed_in = 1;
			break;
		case 1: // Sign in fail
			printf("[-]Cannot find account.\n");
			goto goal;
		case 2: // Sign in fail
			printf("[-]Wrong password.\n");
			goto goal;
		default: // Sign in fail
			printf("[-]Program error.\n");
			goto goal;
		}
		break;
	case 'n':// Sign up
		sign_up(socket_fd);
		goto goal;
		break;
	case 'b': // Exit program
		if (program_exit(socket_fd)) // if True then exit
			return;
		else // if False then go back to Welcome
			goto goal;
	default:
		printf("[-]Program error.\n");
		goto goal;
	}

	if (is_signed_in) // If sign in successfully
	{
		switch (menu()) // Menu
		{
		case 1: // Tutorial (Play with bot)
			printf("[+]Coming soon...\n");
			goto goal;
		case 2: // Play (Play with other player)
			printf("[+]Coming soon...\n");
			goto goal;
		case 3: // Log out 
			log_out(socket_fd, current_username, sizeof(current_username));
			goto goal;
		default:
			printf("[-]Program error.\n");
			goto goal;
		}
	}

	return;
}

void *server_app(void *arg)
{
	pthread_detach(pthread_self());
	int client_fd = (int)arg;
	char client_signal[BUFFER_SIZE];
	Account *account = NULL;
	account = read_account(account);
	pthread_t thread_id = pthread_self();

	while (1)
	{	
		// Get client's signal
		if (recv(client_fd, client_signal, sizeof(client_signal), 0) < 0)
		{
			printf("[-]Fail to receive client message.\n");
		}
		else
		{
			standardize_input(client_signal, sizeof(client_signal));
			printf("[+]Client message: %s\n", client_signal);

			switch (atoi(client_signal))
			{
			case 0: // Program exit signal
				printf("[+]Closing thread: %ld.\n", thread_id);
				return NULL;
			case 1: // sign up signal
				printf("[+]Client trying to sign up.\n");
				account = account_sign_up(client_fd, account); // Account sign up
				break;
			case 2: // sign in signal
				printf("[+]Client trying to sign in.\n");
				account_sign_in(client_fd, account); // Account sign in
				break;
			case 3: // log out signal
				printf("[+]Client trying to log out.\n");
				account_log_out(client_fd, account); // Account log out
				break;
			default:
				printf("[-]Server don't understand this signal.\n");
				break;
			}
		}
	}
}