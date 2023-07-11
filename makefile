CC = gcc
LIBS = -pthread

gateway: server.o
	$(CC) $(CFLAGS) $(LIBS) -g -o server server.o 

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f server server.o 
