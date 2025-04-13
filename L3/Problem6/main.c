#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "async_resource.h"

int main() {
    pthread_t threads[3];

    int requests[3] = {2, 3, 4};

    for (int i = 0; i < 3; i++) {
        process_request_t* req = malloc(sizeof(process_request_t));
        req->id = i + 1;
        req->requested_resources = requests[i];
        req->callback = resource_callback;

        pthread_create(&threads[i], NULL, resource_manager, (void*)req);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}