CC = gcc
CFLAGS = -g
LIBS = -pthread
OBJS = job_queue.o bounded_buffer.o

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.o $(OBJS)