#include "../game/game.h"

Game* add_room(Game game, Game* rooms);
Game* delete_room(Game game, Game* rooms);
Game* search_room_by_player(Game* rooms, Account* account);
Game* search_room_by_fd(Game* rooms, int* socket_fd);
void print_rooms(Game* rooms);