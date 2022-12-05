#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include "../exception/exception.h"
#include "../account/account.h"
#define BUFFER_SIZE 1024
#define PORT 8080

void func(int connect_fd)
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

    for (;;)
    {
        // Clean buffers
        bzero(username, sizeof(username));
        bzero(password, sizeof(password));

        // Receive username & password from client_addressent
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
                if (split(is_password_changing, only_number, only_string))
                {
                    write(connect_fd, only_number, sizeof(only_number));
                    write(connect_fd, only_string, sizeof(only_string));
                }
            }
        }

        printf("---------------------\n");
    }
}

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

        if((pid = fork()) == -1)
        {
            printf("[-]fork() failed\n");
            close(connect_fd);
            continue;
        } else if (pid == 0)
        {
            printf("[+]fork() successfully\n");
            // Function for chatting between client_address and server
            func(connect_fd);
            close(connect_fd);
        }
    }

    // After chatting close the socket
    printf("[+]Closing server...\n");
    close(socket_fd);
    printf("[+]Server's closed.\n");
}
