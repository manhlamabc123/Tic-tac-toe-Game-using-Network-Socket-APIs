main: server.o exception.o account.o game.o
	gcc -o server server.o account.o exception.o game.o

server.o:
	gcc -c server.c

account.o:
	gcc -c account/account.c

exception.o:
	gcc -c exception/exception.c

game.o:
	gcc -c game/game.c

clean:
	rm -f *.o *~ server