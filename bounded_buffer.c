#include "bounded_buffer.h"

#define BROKER_IP "127.0.0.1"
#define BROKER_PORT 8081
#define SOCKETERROR (-1)

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
/*
 * Initialize the bounded buffer
 */
BoundedBuffer bounded_buffer_init()
{
  BoundedBuffer *buffer = (BoundedBuffer *)malloc(sizeof(BoundedBuffer));
  buffer->buffer = (SensorData *)malloc(sizeof(SensorData) * BUFFER_CAPACITY);
  buffer->first = 0;
  buffer->last = 0;
  buffer->size = 0;
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
  // check if the data is valid
  if (data.identifier < 0 || data.temperature < -40 || data.temperature > 80 || data.humidity < 0 || data.humidity > 100 || data.timestamp == NULL)
  {
    return;
  }
  sem_wait(&buffer->spaces);
  pthread_mutex_lock(&buffer->mutex);
  buffer->buffer[buffer->last] = data;
  buffer->last = (buffer->last + 1) % BUFFER_CAPACITY;
  buffer->size++;
  printf("Enqueued message from node #%d: %.2f, %.2f, %s\n", data.identifier, data.temperature, data.humidity, data.timestamp);
  printf("Buffer size: %d\n", bounded_buffer_size(buffer));
  if (bounded_buffer_size(buffer) == BUFFER_CAPACITY)
  {
    printf("*********** Buffer is full *************\n");
  }
  pthread_mutex_unlock(&buffer->mutex);
  sem_post(&buffer->items);
}
/*
 * Returns the size of the bounded buffer
 */
int bounded_buffer_size(BoundedBuffer *buffer)
{
  return buffer->size;
}
/*
 * Dequeue a message from the bounded buffer
 */
void bounded_buffer_dequeue(BoundedBuffer *buffer)
{
  sem_wait(&buffer->items);
  pthread_mutex_lock(&buffer->mutex);

  SensorData data = buffer->buffer[buffer->first];
  buffer->first = (buffer->first + 1) % BUFFER_CAPACITY;
  buffer->size--;
  char *serialized_buffer = malloc(sizeof(char) * 1024);
  size_t serialized_size = serialize_sensor_data(&data, serialized_buffer);
  send_to_broker(serialized_buffer, serialized_size);
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

// Serialization function
size_t serialize_sensor_data(SensorData *data, char *buffer)
{
  char *identifier = malloc(sizeof(int));
  sprintf(identifier, "%d", data->identifier);
  char *temperature = malloc(sizeof(float));
  sprintf(temperature, "%.2f", data->temperature);
  char *humidity = malloc(sizeof(float));
  sprintf(humidity, "%.2f", data->humidity);
  char *timestamp = malloc(sizeof(char) * 9);
  sprintf(timestamp, "%s", data->timestamp);
  printf("Serialized message: %s,%s,%s,%s\n", identifier, temperature, humidity, timestamp);
  size_t size = sizeof(int) + sizeof(float) + sizeof(float) + sizeof(char) * 9;
  sprintf(buffer, "%s,%s,%s,%s", identifier, temperature, humidity, timestamp);
  return size;
}

// Send to broker function
void send_to_broker(char *buffer, size_t size)
{
  int client_socket;
  SA_IN server_addr;

  check((client_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(BROKER_IP);
  server_addr.sin_port = htons(BROKER_PORT);
  printf("Sending to broker at %s:%d\n", BROKER_IP, BROKER_PORT);
  printf("Message: %s\n", buffer);
  check(connect(client_socket, (SA *)&server_addr, sizeof(server_addr)), "Broker connection failed");

  check(send(client_socket, buffer, size, 0), "Send failed");

  free(buffer);
  close(client_socket);
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