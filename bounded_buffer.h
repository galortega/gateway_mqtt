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
} SensorData;

typedef struct
{
  SensorData *buffer;
  int first;
  int last;
  int size;
  pthread_mutex_t mutex;
  sem_t spaces;
  sem_t items;
} BoundedBuffer;

SensorData create_sensor_data(char *message);
BoundedBuffer bounded_buffer_init();
void bounded_buffer_enqueue(char *message, BoundedBuffer *bounded_buffer);
void bounded_buffer_dequeue(BoundedBuffer *bounded_buffer);
void bounded_buffer_destroy(BoundedBuffer *buffer);
int bounded_buffer_size(BoundedBuffer *buffer);
void *bounded_buffer_consume(void *arg);
char *convert_time(time_t raw_time);
int check(int exp, const char *msg);
size_t serialize_sensor_data(SensorData *data, char *buffer);
void send_to_broker(char *buffer, size_t size);

#endif // BOUNDED_BUFFER_H_
