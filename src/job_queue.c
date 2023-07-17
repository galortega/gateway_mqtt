#include "job_queue.h"

job_queue_t job_queue_init()
{
  job_queue_t *queue = malloc(sizeof(job_queue_t));
  node_t *head = malloc(sizeof(node_t));
  node_t *tail = malloc(sizeof(node_t));
  head->next = NULL;
  tail->next = NULL;
  queue->head = head;
  queue->tail = tail;
  return *queue;
}

/*
 * Enqueue a client socket to the queue
 */
void thpool_enqueue_job(int *client_socket, job_queue_t *queue)
{
  node_t *head = queue->head;
  node_t *tail = queue->tail;
  node_t *newnode = malloc(sizeof(node_t));
  newnode->client_socket = client_socket;
  newnode->next = NULL;

  if (tail == NULL)
  {
    head = newnode;
  }
  else
  {
    tail->next = newnode;
  }
  tail = newnode;
}

/*
 * Dequeue a client socket from the queue and free the memory
 * Returns the pointer to the client socket, or NULL if the queue is empty
 */
int *thpool_dequeue_job(job_queue_t *queue)
{
  node_t *head = queue->head;
  node_t *tail = queue->tail;
  if (head == NULL)
  {
    return NULL;
  }
  else
  {
    int *client_socket = head->client_socket;
    node_t *temp = head;
    head = head->next;
    if (head == NULL)
    {
      tail = NULL;
    }
    free(temp);
    return client_socket;
  }
}