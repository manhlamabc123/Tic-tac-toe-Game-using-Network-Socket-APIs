main: server.o client.o exception.o account.o helper.o app.o game.o
	gcc -o server server.o exception.o account.o helper.o app.o game.o
	gcc -o client client.o exception.o account.o helper.o app.o game.o

server.o:
	gcc -c client_server/server.c

client.o:
	gcc -c client_server/client.c

exception.o:
	gcc -c exception/exception.c

account.o:
	gcc -c account/account.c

helper.o:
	gcc -c helper/helper.c

game.o:
	gcc -c game/game.c

app.o:
	gcc -c app/app.c

clean:
	rm -f *.o *~ server client
