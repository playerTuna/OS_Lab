#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_RESOURCES 5
#define NUM_PROCESSES 4

typedef struct {
    int id;                             // Process ID
    int requested_resources;           // Number of requested resources
    void (*callback)(int);             // Callback function when granted
} process_request_t;

int available_resources = NUM_RESOURCES;
pthread_mutex_t resource_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resource_cond = PTHREAD_COND_INITIALIZER;

// Callback function
void resource_callback(int process_id) {
    printf(">> Process %d: granted resources, now executing...\n", process_id);
    sleep(1); // simulate work
}

// Resource manager for a request
void* resource_manager(void* arg) {
    process_request_t* request = (process_request_t*) arg;

    pthread_mutex_lock(&resource_lock);

    while (request->requested_resources > available_resources) {
        printf("Process %d waiting for %d resources...\n", request->id, request->requested_resources);
        pthread_cond_wait(&resource_cond, &resource_lock);
    }

    // Allocate resources
    available_resources -= request->requested_resources;
    printf("Process %d allocated %d resources, remaining: %d\n",
           request->id, request->requested_resources, available_resources);

    // Callback to signal resource granted
    request->callback(request->id);

    // Release resources
    available_resources += request->requested_resources;
    printf("Process %d released %d resources, available: %d\n",
           request->id, request->requested_resources, available_resources);

    pthread_cond_broadcast(&resource_cond); // Notify others
    pthread_mutex_unlock(&resource_lock);

    free(request);
    return NULL;
}

int main() {
    pthread_t threads[NUM_PROCESSES];
    int i;

    for (i = 0; i < NUM_PROCESSES; ++i) {
        process_request_t* req = malloc(sizeof(process_request_t));
        req->id = i + 1;
        req->requested_resources = (i % 3) + 1; // Request 1–3 resources
        req->callback = resource_callback;

        pthread_create(&threads[i], NULL, resource_manager, req);
        usleep(100000); // slight delay to simulate interleaving
    }

    for (i = 0; i < NUM_PROCESSES; ++i)
        pthread_join(threads[i], NULL);

    return 0;
}
