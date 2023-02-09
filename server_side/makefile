main: server.o exception.o account.o game.o mysql.o room.o
	gcc -o server server.o account.o exception.o game.o mysql.o room.o -lmysqlclient

server.o:
	gcc -c server.c

account.o:
	gcc -c account/account.c

exception.o:
	gcc -c exception/exception.c

game.o:
	gcc -c game/game.c

mysql.o:
	gcc -c mysql/mysql.c

room.o:
	gcc -c room/room.c

clean:
	rm -f *.o *~ server