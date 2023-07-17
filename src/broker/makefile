CC = gcc
CFLAGS = -g
LIBS = -pthread
BINS = server
OBJS = server.o ../job_queue.o ../bounded_buffer.o

all: $(BINS)

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf server *.o $(BINS)