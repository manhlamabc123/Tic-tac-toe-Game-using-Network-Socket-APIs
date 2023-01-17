#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include "game/game.h"

int main(int argc, char *argv[])
{
    // Catch wrong input
    if (argc != 2)
    {
        printf("[-]Please input port number\n");
        return 0;
    }

    // Variables
    char *port_number = argv[1];
    int port = atoi(port_number);
    struct sockaddr_in server_address, client_address;
    int socket_fd, connect_fd, len = sizeof(client_address);
    bzero(&server_address, sizeof(server_address));
    bzero(&client_address, sizeof(client_address));
    int socket_count = 0;
    Message message;

    // Load database
    Account *acc = NULL;
    acc = read_account(acc);

    // Create game
    Game in_waiting_game;
    in_waiting_game.status = -1;

    if (port < 1 || port > 65535)
    {
        printf("[-]Invalid port\n");
        return 0;
    }

    // Socket create and verification
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }
    else
        printf("[+]Socket successfully created\n");

    // Assign IP, PORT
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }
    else
        printf("[+]Socket successfully bind\n");

    // Now server is ready to listen and verification
    if ((listen(socket_fd, 5)) != 0)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }
    else
        printf("[+]Server listening\n");

    // Initialize udfs
    int ready, nfds = BUFFER_SIZE;
    struct pollfd *ufds;
    ufds = calloc(nfds + 1, sizeof(struct pollfd));
    if (ufds == NULL)
    {
        fprintf(stderr, "[-]%s\n", strerror(errno));
        return 0;
    }

    ufds[0].fd = socket_fd;
    ufds[0].events = POLLIN;
    socket_count = 1;
    for (int i = 1; i <= nfds; i++)
    {
        ufds[i].fd = -1;
    }

    int i;
    while (1)
    {
        // Call poll()
        printf("[+]Socket count: %d\n", socket_count);
        printf("[+]Poll is blocking...\n");
        if ((ready = poll(ufds, socket_count, -1)) == -1)
        {
            fprintf(stderr, "[-]%s\n", strerror(errno));
            return 0;
        }
        printf("[+]Ready: %d\n", ready);

        // Handling returned ufds
        if (ufds[0].revents & POLLIN)
        {
            printf("[+]fd=%d; events: %s%s%s\n", ufds[0].fd,
                   (ufds[0].revents & POLLIN) ? "POLLIN " : "",
                   (ufds[0].revents & POLLHUP) ? "POLLHUP " : "",
                   (ufds[0].revents & POLLERR) ? "POLLERR " : "");

            /* new client connection */
            connect_fd = accept(socket_fd, (struct sockaddr *)&client_address, &len);
            if (connect_fd < 0)
            {
                fprintf(stderr, "[-]%s\n", strerror(errno));
                return 0;
            }

            printf("[+]Server accept new client: %d\n", connect_fd);
            for (i = 1; i < BUFFER_SIZE; i++)
            {
                if (ufds[i].fd < 0)
                {
                    /* save descriptor */
                    ufds[i].fd = connect_fd;
                    ufds[i].events = POLLIN;
                    break;
                }
            }

            if (i == BUFFER_SIZE)
            {
                printf("[-]Too many clients\n");
                return 0;
            }

            // Increase socket count
            if (i >= socket_count)
            {
                socket_count = i + 1;
                printf("[+]Socket count: %d\n", socket_count);
            }

            /* no more readable descriptors */
            ready = ready - 1;
            if (ready <= 0)
                continue;
        }

        for (i = 1; i <= socket_count; i++)
        {
            /* check all clients for data */
            if (ufds[i].fd < 0)
                continue;

            printf("[+]fd=%d; events: %s%s%s\n", ufds[i].fd,
                   (ufds[i].revents & POLLIN) ? "POLLIN " : "",
                   (ufds[i].revents & POLLHUP) ? "POLLHUP " : "",
                   (ufds[i].revents & POLLERR) ? "POLLERR " : "");

            if (ufds[i].revents & (POLLIN | POLLERR))
            {
                // Recv client's message
                if (recv(ufds[i].fd, &message, sizeof(struct _message), MSG_WAITALL) < 0)
                {
                    fprintf(stderr, "[-]%s\n", strerror(errno));
                    return 0;
                }

                switch (message.header)
                {
                case EXIT_PROGRAM: // Program exit signal
                    printf("[+]Close socket: %d.\n", ufds[i].fd);
                    close(ufds[i].fd);
                    ufds[i].fd = -1;
                    break;
                case SIGN_UP: // sign up signal
                    printf("[+]Client trying to sign up.\n");
                    acc = account_sign_up(ufds[i].fd, acc, message.account); // Account sign up
                    break;
                case SIGN_IN: // sign in signal
                    printf("[+]Client trying to sign in.\n");
                    account_sign_in(ufds[i].fd, acc, message.account); // Account sign in
                    break;
                case LOG_OUT: // log out signal
                    printf("[+]Client trying to log out.\n");
                    account_log_out(ufds[i].fd, acc, message.account); // Account log out
                    break;
                case PLAY_BOT: // play with bot signal
                    printf("[+]Client trying to play with bot.\n");
                    server_game_bot(ufds[i].fd, message.game); // Play with bot
                    break;
                case FIND_PLAYER:
                    printf("[+]Client trying to find player.\n");
                    if (find_player(ufds[i].fd, &in_waiting_game, acc, message.account))
                    {
                        printf("[+]Clear waiting_game\n");
                        bzero(&in_waiting_game, sizeof(struct _game));
                        in_waiting_game.status = -1;
                    }
                    break;
                case PLAYER_MADE_MOVE:
                    printf("[+]Player made a move.\n");
                    player_vs_player(ufds[i].fd);
                    break;
                default:
                    printf("[-]Server don't understand this signal.\n");
                    break;
                }

                /* no more readable descriptors */
                if (--ready <= 0)
                    break;
            }
        }
    }
}