#include "queue.h"

node_t *head = NULL;
node_t *tail = NULL;

/*
 * Enqueue a client socket to the queue
 */
void thpool_add_job(int *client_socket)
{
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
int *thpool_do_job()
{
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