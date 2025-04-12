#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 3

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

void* writer(void* arg) {
    int id = *(int*)arg;
    pthread_mutex_lock(&mutex);
    while (count == BUFFER_SIZE) {
        printf("Buffer full: write(%d) waiting...\n", id);
        pthread_cond_wait(&cond_var, &mutex);
    }
    buffer[count++] = id;
    printf("write(): %d \n", id);
    pthread_cond_signal(&cond_var);
    pthread_mutex_unlock(&mutex);
    usleep(100000); // Simulate some work
    return NULL;
}
void* reader(void* arg) {
    int id = *(int*)arg;
    pthread_mutex_lock(&mutex);
    while (count == 0) {
        printf("Buffer empty: read(%d) waiting...\n", id);
        pthread_cond_wait(&cond_var, &mutex);
    }
    int value = buffer[--count];
    printf("read(%d): %d \n", id, value);
    pthread_cond_signal(&cond_var);
    pthread_mutex_unlock(&mutex);
    usleep(100000); // Simulate some work
    return NULL;
}
int main() {
    pthread_t writers[5], readers[3];
    int writers_count = 5;
    int readers_count = 3;
    int writer_id[5] = {1, 2, 3, 4, 5};
    int reader_id[3] = {1, 2, 3};
    printf("Buffer size: %d\n", BUFFER_SIZE);
    printf("Number of writers: %d\n", writers_count);
    printf("Number of readers: %d\n", readers_count);
    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);
    // Creae first reader thread
    // This thread will be created first to ensure it waits for the buffer to be filled
    // before any writer starts writing
    pthread_create(&readers[0], NULL, reader, &reader_id[0]);

    for (int i = 0; i < writers_count; i++) {
        pthread_create(&writers[i], NULL, writer, &writer_id[i]);
    }

    for (int i = 1; i < readers_count; i++) {
        pthread_create(&readers[i], NULL, reader, &reader_id[i]);
    }
    // Wait for all writers and readers to finish
    for (int i = 0; i < writers_count; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < readers_count; i++) {
        pthread_join(readers[i], NULL);
    }
    // Clean up
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);
    return 0;
}
// Compile with: gcc -o wait_notify wait_notify.c -lpthread
// Run with: ./wait_notify