#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <errno.h>
#include "../exception/exception.h"
#include "../game/game.h"
#include "../app/app.h"
#define BUFFER_SIZE 1024

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
		fprintf(stderr, "[-]%s\n", strerror(errno));
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
		fprintf(stderr, "[-]%s\n", strerror(errno));
		exit(0);
	}
	else
		printf("[+]Connected to the server\n");

	// Main function
	client_app(socket_fd);

	// Close socket
	close(socket_fd);
	printf("[+]Exit program\n");
	
	return 0;
}
