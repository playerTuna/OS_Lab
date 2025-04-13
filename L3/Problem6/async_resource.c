#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "async_resource.h"

int available_resources = NUM_RESOURCES;
pthread_mutex_t resource_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resource_cond = PTHREAD_COND_INITIALIZER;

void resource_callback(int process_id) {
    printf("Callback: Process %d has been allocated resources.\n", process_id);
}

void* resource_manager(void* arg) {
    process_request_t* request = (process_request_t*)arg;

    pthread_mutex_lock(&resource_lock);

    while (request->requested_resources > available_resources) {
        printf("Process %d waiting for resources\n", request->id);
        pthread_cond_wait(&resource_cond, &resource_lock);
    }

    available_resources -= request->requested_resources;
    request->callback(request->id);

    // Simulate some processing time
    sleep(1);

    available_resources += request->requested_resources;
    printf("Process %d released resources\n", request->id);
    pthread_cond_broadcast(&resource_cond);

    pthread_mutex_unlock(&resource_lock);
    free(request);
    return NULL;
}