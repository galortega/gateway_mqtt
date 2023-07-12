#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <semaphore.h>
#include <stdbool.h>
#include "queue.h"

#define SERVERPORT 8080
#define BUFFERSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100
#define THPOOL_SIZE 10

pthread_t th_pool[THPOOL_SIZE];
pthread_mutex_t th_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t th_pool_cond = PTHREAD_COND_INITIALIZER;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void *handle_client(void *arg);
int check(int exp, const char *msg);
void *th_job(void *arg);

int main(int argc, char **argv)
{
  int server_socket, client_socket, addr_size;
  SA_IN server_addr, client_addr;

  // Create thread pool
  for (int i = 0; i < THPOOL_SIZE; i++)
  {
    pthread_create(&th_pool[i], NULL, th_job, NULL);
  }

  check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVERPORT);

  check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Bind failed");
  check(listen(server_socket, SERVER_BACKLOG), "Listen failed");

  printf("Waiting for connections on port %d...\n", SERVERPORT);
  while (1)
  {
    addr_size = sizeof(SA_IN);
    check(client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size), "Accept failed");
    printf("Connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    pthread_t thread;
    int *pclient = malloc(sizeof(int));
    *pclient = client_socket;
    pthread_mutex_lock(&th_pool_mutex);
    enqueue(pclient);
    pthread_cond_signal(&th_pool_cond);
    pthread_mutex_unlock(&th_pool_mutex);
  }

  return 0;
}

int check(int exp, const char *msg)
{
  if (exp == SOCKETERROR)
  {
    perror(msg);
    exit(1);
  }
  return exp;
}

void *th_job(void *arg)
{
  while (1)
  {
    int *pclient;
    pthread_mutex_lock(&th_pool_mutex);
    if ((pclient = dequeue()) == NULL)
    {
      pthread_cond_wait(&th_pool_cond, &th_pool_mutex);
      pclient = dequeue();
    }
    pthread_mutex_unlock(&th_pool_mutex);
    if (pclient != NULL)
    {
      // we have a client
      handle_client(pclient);
    }
  }
}

void *handle_client(void *arg)
{
  int client_socket = *((int *)arg);
  free(arg);
  char raw_message[BUFFERSIZE];
  memset(raw_message, 0, BUFFERSIZE);
  int recv_size, msg_size;

  while (1)
  {
    recv_size = check(recv(client_socket, raw_message, BUFFERSIZE - 1, 0), "Recv failed");
    if (recv_size == 0)
    {
      break;
    }

    msg_size = recv_size;
    printf("Received by client at socket %d: %s\n", client_socket, raw_message);
  }

  close(client_socket);
  return NULL;
}