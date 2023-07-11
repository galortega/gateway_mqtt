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

#define SERVERPORT 8080
#define BUFFERSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void handle_client(int client_socket);
int check(int exp, const char *msg);

int main(int argc, char **argv)
{
  int server_socket, client_socket, addr_size;
  SA_IN server_addr, client_addr;

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

    handle_client(client_socket);
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

void handle_client(int client_socket)
{
  char buffer[BUFFERSIZE];
  int recv_size, msg_size;

  while (1)
  {
    recv_size = check(recv(client_socket, buffer, BUFFERSIZE, 0), "Recv failed");
    if (recv_size == 0)
    {
      printf("Client disconnected\n");
      break;
    }

    msg_size = recv_size;
    printf("Received: %s", buffer);
  }

  close(client_socket);
  return;
}