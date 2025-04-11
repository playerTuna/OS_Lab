#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "seqlock.h"

pthread_seqlock_t seqlock;
int shared_data = 0;

void* writer(void* arg) {
    for (int i = 0; i < 5; ++i) {
        pthread_seqlock_wrlock(&seqlock);
        shared_data++;
        printf("Writer updated shared_data to %d\n", shared_data);
        sleep(1); // giả lập ghi lâu
        pthread_seqlock_wrunlock(&seqlock);
        sleep(1);
    }
    return NULL;
}

void* reader(void* arg) {
    for (int i = 0; i < 10; ++i) {
        pthread_seqlock_rdlock(&seqlock);
        int val = shared_data;
        pthread_seqlock_rdunlock(&seqlock);
        printf("Reader read shared_data as %d\n", val);
        usleep(500000); // đọc nhanh hơn
    }
    return NULL;
}

int main() {
    pthread_seqlock_init(&seqlock);

    pthread_t w, r1, r2;
    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r1, NULL, reader, NULL);
    pthread_create(&r2, NULL, reader, NULL);

    pthread_join(w, NULL);
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);

    pthread_seqlock_destroy(&seqlock);
    return 0;
}
