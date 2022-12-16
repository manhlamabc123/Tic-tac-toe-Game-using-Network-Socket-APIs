#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
#include "../exception/exception.h"
#include "../account/account.h"
#include "../app/app.h"
#define BUFFER_SIZE 1024

// Driver function
int main(int argc, char *argv[])
{
    // Catch wrong input
    if (argc != 2)
    {
        printf("[-]Please input port number\n");
        return 0;
    }

    char *port_number = argv[1];
    int port = atoi(port_number);
    struct sockaddr_in server_address, client_address;
    int socket_fd, connect_fd, len = sizeof(client_address);
    int pid;
    pthread_t tid;

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
        return 0;
    }
    else
        printf("[+]Socket successfully created\n");
    bzero(&server_address, sizeof(server_address));

    // assign IP, PORT
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    {
        printf("[-]Socket bind failed\n");
        return 0;
    }
    else
        printf("[+]Socket successfully bind\n");

    // Now server is ready to listen and verification
    if ((listen(socket_fd, 5)) != 0)
    {
        printf("[-]Listen failed\n");
        return 0;
    }
    else
        printf("[+]Server listening\n");

    while (1)
    { // Accept the data packet from client_address and verification
        connect_fd = accept(socket_fd, (struct sockaddr *)&client_address, &len);
        if (connect_fd < 0)
        {
            printf("[-]Server accept failed\n");
            return 0;
        }
        else
            printf("[+]Server accept the client_address\n");

        pthread_create(&tid, NULL, &server_app, (void *) connect_fd);
    }

    // After chatting close the socket
    printf("[+]Closing server...\n");
    close(socket_fd);
    printf("[+]Server's closed.\n");
}
