#include "bounded_buffer.h"
/*
  * Initialize the bounded buffer
*/
BoundedBuffer bounded_buffer_init()
{
  BoundedBuffer *buffer = (BoundedBuffer *)malloc(sizeof(BoundedBuffer));
  buffer->buffer = (SensorData *)malloc(sizeof(SensorData) * BUFFER_CAPACITY);
  buffer->first = 0;
  buffer->last = 0;
  pthread_mutex_init(&buffer->mutex, NULL);
  sem_init(&buffer->spaces, 0, BUFFER_CAPACITY);
  sem_init(&buffer->items, 0, 0);
  return *buffer;
}
/*
  * Enqueue a message to the bounded buffer
*/
void bounded_buffer_enqueue(char *message, BoundedBuffer *buffer)
{
  if (message == NULL)
  {
    return;
  }
  SensorData data = create_sensor_data(message);
  sem_wait(&buffer->spaces);
  pthread_mutex_lock(&buffer->mutex);
  if ((buffer->last + 1) % BUFFER_CAPACITY == buffer->first)
  {
    printf("Buffer is full (size %d), cannot enqueue message from node #%d: %.2f, %.2f, %s\n", bounded_buffer_size(buffer), data.identifier, data.temperature, data.humidity, data.timestamp);
  }
  else
  {
    printf("Enqueued message from node #%d: %.2f, %.2f, %s\n", data.identifier, data.temperature, data.humidity, data.timestamp);
    printf("Buffer size: %d\n", bounded_buffer_size(buffer));
    buffer->buffer[buffer->last] = data;
    buffer->last = (buffer->last + 1) % BUFFER_CAPACITY;
  }
  pthread_mutex_unlock(&buffer->mutex);
  sem_post(&buffer->items);
}
/*
  * Returns the size of the bounded buffer
*/
int bounded_buffer_size(BoundedBuffer *buffer)
{
  int size = (buffer->last - buffer->first + BUFFER_CAPACITY) % BUFFER_CAPACITY + 1;
  return size;
}
/*
  * Dequeue a message from the bounded buffer
*/
void bounded_buffer_dequeue(BoundedBuffer *buffer)
{
  sem_wait(&buffer->items);
  pthread_mutex_lock(&buffer->mutex);

  SensorData data = buffer->buffer[buffer->first];
  printf("Dequeued message from node #%d: %.2f, %.2f, %s\n", data.identifier, data.temperature, data.humidity, data.timestamp);
  buffer->first = (buffer->first + 1) % BUFFER_CAPACITY;

  pthread_mutex_unlock(&buffer->mutex);
  sem_post(&buffer->spaces);
}
/*
  * Destroy the bounded buffer
*/
void bounded_buffer_destroy(BoundedBuffer *buffer)
{
  pthread_mutex_destroy(&buffer->mutex);
  sem_destroy(&buffer->spaces);
  sem_destroy(&buffer->items);
  free(buffer->buffer);
  free(buffer);
}
/*
  * Consume messages from the bounded buffer
*/
void *bounded_buffer_consume(void *arg)
{
  BoundedBuffer *buffer = (BoundedBuffer *)arg;
  while (1)
  {
    bounded_buffer_dequeue(buffer);
    sleep(2);
  }
  return NULL;
}
/*
  * Convert a raw time to a string
*/
char *convert_time(time_t raw_time)
{
  struct tm *time_info;
  time_info = localtime(&raw_time);
  char *time_string = malloc(sizeof(char) * 9);
  strftime(time_string, sizeof(char) * 9, "%H:%M:%S", time_info);
  return time_string;
}
/*
  * Create a SensorData struct from a raw message
*/
SensorData create_sensor_data(char *message)
{
  SensorData sensor_data;
  char *token = strtok(message, ",");
  int i = 0;
  while (token != NULL)
  {
    switch (i)
    {
    case 0:
      sensor_data.identifier = atoi(token);
      break;
    case 1:
      sensor_data.temperature = atof(token);
      break;
    case 2:
      sensor_data.humidity = atof(token);
      break;
    case 3:
      sensor_data.timestamp = convert_time((time_t)atof(token));
      break;
    default:
      break;
    }
    token = strtok(NULL, ",");
    i++;
  }
  return sensor_data;
}