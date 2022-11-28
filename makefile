main: server.o client.o exception.o account.o
	gcc -o server server.o exception.o account.o
	gcc -o client client.o exception.o account.o

server.o:
	gcc -c server.c

client.o:
	gcc -c client.c

exception.o:
	gcc -c exception/exception.c

account.o:
	gcc -c account/account.c

clean:
	rm -f *.o *~ server client
