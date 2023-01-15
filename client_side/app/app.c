#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
#include <errno.h>
#include "app.h"
#include "../exception/exception.h"
#include "../game/game.h"

void client_app(int socket_fd)
{
	// Clear scene
	system("clear");

	Account current_user;
	int is_signed_in = 0;

welcome:
	switch (welcome()) // Welcome
	{
	case 'y': // Sign in
		switch (sign_in(socket_fd, &current_user, sizeof(struct _account)))
		{
		case 1:
			is_signed_in = 1;
			break;
		case 2:
			goto welcome;
		default:
			printf("[-]Error: sign_in\n");
			return;
		}
		break;
	case 'n': // Sign up
		if (!sign_up(socket_fd))
		{
			printf("[-]Error: sign_up\n");
			return;
		}
		goto welcome;
		break;
	case 'b': // Exit program
		switch (program_exit(socket_fd))
		{
		case 1:
			return;
		case 2:
			goto welcome;
		default:
			printf("[-]Error: program_exit\n");
			return;
		}
	default:
		printf("[-]Program error.\n");
		return;
	}

	if (is_signed_in) // If sign in successfully
	{
	menu:
		switch (menu()) // Menu
		{
		case 1: // Tutorial (Play with bot)
			play_with_bot(socket_fd, current_user);
			goto menu;
		case 2: // Play (Play with other player)
			find_player(socket_fd, &current_user);
			goto menu;
		case 3: // Log out
			log_out(socket_fd, current_user.username, sizeof(current_user.username));
			goto welcome;
		default:
			printf("[-]Program error.\n");
			goto welcome;
		}
	}

	return;
}