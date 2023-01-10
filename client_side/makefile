main: client.o exception.o helper.o app.o game.o
	gcc -o client client.o exception.o helper.o app.o game.o

client.o:
	gcc -c client.c

exception.o:
	gcc -c exception/exception.c

helper.o:
	gcc -c helper/helper.c

game.o:
	gcc -c game/game.c

app.o:
	gcc -c app/app.c

clean:
	rm -f *.o *~ client
