#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "room.h"

Game *create_new_game(Game game)
{
    Game *temp = (Game *)malloc(sizeof(struct _game));

    temp->first_player = game.first_player;
    temp->second_player = game.second_player;
    temp->board = game.board;
    strcpy(temp->date, game.date);
    temp->number_of_moves = game.number_of_moves;
    for (int i = 0; i < temp->number_of_moves; i++)
        temp->moves[i] = game.moves[i];
    temp->status = game.status;
    temp->next = NULL;

    return temp;
}

Game *add_room(Game game, Game *rooms)
{
    Game *current = rooms;
    Game *temp = create_new_game(game);

    if (rooms == NULL)
    {
        current = temp;
        return current;
    }

    while (current->next != NULL)
        current = current->next;
    current->next = temp;

    return rooms;
}

Game *delete_room(Game game, Game *rooms)
{
    Game *current = rooms;
    Game *temp;
    Game *previous = NULL;

    if (current == NULL)
    {
        printf("[-]Game list empty\n");
        return NULL;
    }

    while (current != NULL)
    {
        if (strcmp(current->date, game.date) == 0)
        {
            if (previous == NULL)
            {
                rooms = current->next;
                free(current);
                return rooms;
            }

            previous->next = current->next;
            free(current);
            return rooms;
        }

        previous = current;
        current = current->next;
    }

    return rooms;
}

Game *search_room_by_player(Game *rooms, Account *account)
{
    Game *current = rooms;

    if (current == NULL)
    {
        printf("[-]Game list empty\n");
        return NULL;
    }

    while (current != NULL)
    {
        if ((strcmp(current->first_player.username, account->username) == 0) || (strcmp(current->second_player.username, account->username) == 0))
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

Game *search_room_by_fd(Game *rooms, int *socket_fd)
{
    Game *current = rooms;

    if (current == NULL)
    {
        printf("[-]Game list empty\n");
        return NULL;
    }

    while (current != NULL)
    {
        if ((current->first_player.socket_fd == *socket_fd) || (current->second_player.socket_fd == *socket_fd))
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

void print_rooms(Game *rooms)
{
    Game *current = rooms;

    if (current == NULL)
    {
        printf("[-]Game list empty\n");
        return;
    }

    while (current != NULL)
    {
        print_game(current);

        current = current->next;
    }

    return;
}