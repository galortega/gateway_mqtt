#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

node_t *head = NULL;
node_t *tail = NULL;

/*
 * Enqueue a client socket to the queue
 */
void enqueue(int *client_socket)
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
int *dequeue()
{
  if (head == NULL)
  {
    return NULL;
  }
  else
  {
    int *client_socket = head->client_socket;
    printf("Dequeueing client socket %d\n", *client_socket);
    node_t *temp = head;
    printf("Freeing node\n");
    head = head->next;
    printf("Freed node\n");
    if (head == NULL)
    {
      printf("Head is NULL\n");
      tail = NULL;
    }
    free(temp);
    printf("Returning client socket\n");
    return client_socket;
  }
}