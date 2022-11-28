main: server.o client.o exception.o account.o helper.o
	gcc -o server server.o exception.o account.o helper.o
	gcc -o client client.o exception.o account.o helper.o

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

clean:
	rm -f *.o *~ server client
