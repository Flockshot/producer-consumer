//Muhammad Somaan
//2528404

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef int buffer_item;
#define BUFFER_SIZE 5
#define RAND_SLEEP_TIME 3


void *producer(void *param);
void *consumer(void *param);
int insert_item(buffer_item item);
int remove_item(buffer_item *item);


buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t empty;
sem_t full;


int main (int argc, const char * argv[])
{

    if(argc<4)
    {
        printf("Please enter 3 arguments\n");
        return -1;
    }

    int sleep_time = atoi(argv[1]);
    int producer_threads = atoi(argv[2]);
    int consumer_threads = atoi(argv[3]);

    srandom((unsigned)time(NULL));

    sem_init(&empty, 0, 5);
    sem_init(&full, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    pthread_t producers[producer_threads];
    pthread_t consumers[consumer_threads];

    int i = 0;
    for(i=0; i< producer_threads; i++)
        pthread_create(&producers[i], NULL, producer, NULL);
    for(i=0; i< consumer_threads; i++)
        pthread_create(&consumers[i], NULL, consumer, NULL);

    sleep(sleep_time);

    sem_destroy(&full);
    sem_destroy(&empty);
    pthread_mutex_destroy(&mutex);
    return 0;
}


void *producer(void *param)
{
    buffer_item item;
    while(1)
    {
        /* sleep for a random period of time */
        sleep(rand()%RAND_SLEEP_TIME);
        /* generate a random number */
        item = rand();
        if (insert_item(item))
            printf("Producer tried to add but buffer was full\n");
        else
            printf("Producer produced %d\n", item);
    }
}

void *consumer(void *param)
{
    buffer_item item;
    while(1)
    {
        /* sleep for a random period of time */
        sleep(rand()%RAND_SLEEP_TIME);
        if (remove_item(&item))
            printf("Consumer tried to consume but buffer already empty\n");
        else
            printf("Consumer consumed %d\n", item);
    }
}

int insert_item(buffer_item item)
{
    pthread_mutex_lock(&mutex);

    int full_val;
    sem_getvalue(&full, &full_val);

    if(full_val == BUFFER_SIZE)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    buffer[full_val] = item;

    sem_wait(&empty);
    sem_post(&full);
    pthread_mutex_unlock(&mutex);

    return 0;
}
int remove_item(buffer_item *item)
{
    pthread_mutex_lock(&mutex);

    int empty_val;
    sem_getvalue(&empty, &empty_val);

    if(empty_val == BUFFER_SIZE)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    *item = buffer[empty_val];

    sem_wait(&full);
    sem_post(&empty);
    pthread_mutex_unlock(&mutex);

    return 0;
}


