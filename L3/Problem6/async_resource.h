#ifndef ASYNC_RESOURCE_H
#define ASYNC_RESOURCE_H

#include <pthread.h>

#define NUM_RESOURCES 5

typedef struct {
    int id; // Process ID
    int requested_resources; // Resources the process requests
    void (*callback)(int); // Callback when resource available
} process_request_t;

extern int available_resources;
extern pthread_mutex_t resource_lock;
extern pthread_cond_t resource_cond;

void resource_callback(int process_id);
void* resource_manager(void* arg);

#endif