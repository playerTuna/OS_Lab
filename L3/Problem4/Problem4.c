#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_WRITERS 10
#define NUM_READERS 10

int buffer[BUFFER_SIZE];
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

void* writer(void* arg) {
    int id = *(int*)arg;
    pthread_mutex_lock(&mutex);
    while (count == BUFFER_SIZE) {
        // Buffer full => wait
        pthread_cond_wait(&cond_var, &mutex);
    }

    buffer[count++] = id;
    printf("Writer %d wrote value %d (count=%d)\n", id, id, count);

    pthread_cond_signal(&cond_var); // Notify readers
    pthread_mutex_unlock(&mutex);
    sleep(1);
    return NULL;
}

void* reader(void* arg) {
    int id = *(int*)arg;
    pthread_mutex_lock(&mutex);
    while (count == 0) {
        // Buffer empty => wait
        pthread_cond_wait(&cond_var, &mutex);
    }

    int val = buffer[--count];
    printf("Reader %d read value %d (count=%d)\n", id, val, count);

    pthread_cond_signal(&cond_var); // Notify writers
    pthread_mutex_unlock(&mutex);
    sleep(1);
    return NULL;
}

int main() {
    pthread_t writer_threads[NUM_WRITERS];
    pthread_t reader_threads[NUM_READERS];
    int writer_ids[NUM_WRITERS], reader_ids[NUM_READERS];

    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i;
        pthread_create(&writer_threads[i], NULL, writer, &writer_ids[i]);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        pthread_create(&reader_threads[i], NULL, reader, &reader_ids[i]);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writer_threads[i], NULL);
    }
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    return 0;
}