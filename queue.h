#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>


// This queue will contain the client sockets
typedef struct node
{
  struct node *next;
  int *client_socket;
} node_t;

void thpool_add_job(int *client_socket);
int *thpool_do_job();

#endif // QUEUE_H_