#ifndef QUEUE_H_
#define QUEUE_H_

// This queue will contain the client sockets
typedef struct node
{
  struct node *next;
  int *client_socket;
} node_t;

void enqueue(int *client_socket);
int *dequeue();

#endif // QUEUE_H_