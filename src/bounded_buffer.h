#ifndef BOUNDED_BUFFER_H_
#define BOUNDED_BUFFER_H_

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

#define BUFFER_CAPACITY 20

typedef struct
{
  int identifier;
  float temperature;
  float humidity;
  char *timestamp;
} sensor_data_t;

typedef struct
{
  sensor_data_t *buffer;
  int first;
  int last;
  int size;
  pthread_mutex_t mutex;
  sem_t spaces;
  sem_t items;
} bounded_buffer_t;

sensor_data_t create_sensor_data(char *message);
bounded_buffer_t bounded_buffer_init();
void bounded_buffer_enqueue(char *message, bounded_buffer_t *bounded_buffer);
void bounded_buffer_dequeue(bounded_buffer_t *bounded_buffer);
void bounded_buffer_destroy(bounded_buffer_t *buffer);
int bounded_buffer_size(bounded_buffer_t *buffer);
void *bounded_buffer_consume(void *arg);
char *convert_time(time_t raw_time);

#endif // BOUNDED_BUFFER_H_
