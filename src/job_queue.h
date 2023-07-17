#ifndef JOB_QUEUE_H_
#define JOB_QUEUE_H_

#include <stdlib.h>

// This queue will contain the client sockets
typedef struct node
{
  struct node *next;
  int *client_socket;
} node_t;

typedef struct job_queue
{
  node_t *head;
  node_t *tail;
} job_queue_t;

job_queue_t job_queue_init();
void thpool_enqueue_job(int *client_socket, job_queue_t *queue);
int *thpool_dequeue_job(job_queue_t *queue);

#endif // JOB_QUEUE_H_