
all : server client

server : server.c
	gcc -std=c99 -Wall server.c -o server

client : client.c
	gcc -std=c99 -Wall client.c -o client

.PHONY : clean
clean :
	rm -rf *.o server client
