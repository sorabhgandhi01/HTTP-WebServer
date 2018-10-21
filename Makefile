all : server
CFLAGS = -g -Wall -Werror -O0
cc = gcc

server : http_server.o
	$(CC) $(CFLAGS) -o server http_server.o

http_server.o : http_server.c
	$(CC) $(CFLAGS) -c http_server.c

clean:
	rm -f server *.o
