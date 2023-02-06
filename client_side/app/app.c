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
#include "../game/game.h"

void client_app(int socket_fd)
{
	// Clear scene
	system("clear");

	// Variables
	Account current_user;

welcome:
	switch (welcome()) // Welcome
	{
	case 1: // Sign in
		switch (sign_in(socket_fd, &current_user, sizeof(struct _account)))
		{
		case 1:
			current_user.status = 1;
			break;
		case 2:
			goto welcome;
		default:
			printf("[-]Error: sign_in\n");
			return;
		}
		break;
	case 2: // Sign up
		switch (sign_up(socket_fd))
		{
		case 1:
		case 2:
			goto welcome;
		default:
			printf("[-]Error: sign_up\n");
			return;
		}
		break;
	case 0: // Exit program
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
		printf("[-]Program error\n");
		return;
	}

	if (current_user.status) // If sign in successfully
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
		case 0: // Log out
			log_out(socket_fd, current_user);
			goto welcome;
		default:
			printf("[-]Program error\n");
			goto welcome;
		}
	}

	return;
}