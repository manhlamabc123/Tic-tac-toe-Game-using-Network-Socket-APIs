#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include "exception/exception.h"
#include "account/account.h"
#define BUFFER_SIZE 1024
#define PORT 8080

int split(char *buffer, char *only_number, char *only_string)
{
    // Only number in buffer converts to string only_number
    strcpy(only_string, buffer);
    int k = 0;
    strcpy(only_number, buffer);
    int j = 0;

    // if number, copy to only_number
    // if character, copy to only_string
    int m = 0;
    for (int i = 0; i < 100; i++)
    {
        char ch = only_number[i];
        if (ch == '\0')
            break;
        if (ch >= '0' && ch <= '9')
        {
            only_number[j] = ch;
            j++;
        }
        else if ((ch >= 'a' && ch <= 'z') || (ch == ' '))
        {
            only_string[k] = ch;
            k++;
        }
        else
        {
            return 0;
        }
    }
    only_number[j] = '\0';
    only_string[k] = '\0';
    return 1;
}

void func(int connfd)
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

        // Receive username & password from client
        read(connfd, username, sizeof(username));
        read(connfd, password, sizeof(password));

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
        write(connfd, sign_in_feedback, sizeof(sign_in_feedback));

        if (feedback == 0) // If signed in
        {
            read(connfd, is_password_changing, sizeof(is_password_changing));
            standardize_input(is_password_changing, sizeof(is_password_changing));

            if (strcmp(bye, is_password_changing) == 0)
            {
                if (sign_out(acc, username))
                {
                    write(connfd, bye, sizeof(bye));
                }
            }
            else if (strlen(is_password_changing) > 1)
            {
                if (change_password(acc, username, is_password_changing))
                {
                    write(connfd, sign_in_feedback, sizeof(sign_in_feedback));
                }
                if (split(is_password_changing, only_number, only_string))
                {
                    write(connfd, only_number, sizeof(only_number));
                    write(connfd, only_string, sizeof(only_string));
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
        printf("Please input port number\n");
        return 0;
    }

    char *port_number = argv[1];
    int port = atoi(port_number);
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    if (port < 1 || port > 65535) {
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    printf("Closing server...\n");
    close(sockfd);
    printf("Server's closed.\n");
}
