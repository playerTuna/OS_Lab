#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int finished = 0;

// TODO: perform polling checks
int is_safe() {
    static int check_count = 0;
    check_count++;

    int found = (check_count < 2); // giả lập: lần thứ 2 không còn "safe" nữa

    if (!found) {
        return -1; // Unsafety detected
    }

    return 0; // Safe
}

// Periodic detector thread
void* periodical_detector(void* arg) {
    while (1) {
        sleep(5); // Periodic check every 5 seconds

        pthread_mutex_lock(&lock);
        if (is_safe() < 0) {
            // TODO: Abnormal detected! Taking corrective action ...
            if (!finished) {
                printf("[!] Abnormal state detected. Taking corrective action...\n");
                finished = 1;
                pthread_mutex_unlock(&lock);
                break; // Break after terminating
            }
        }
        pthread_mutex_unlock(&lock);
    }

    return 0;
}

int main() {
    pthread_t tid;

    // TODO: pthread create to activate periodical detector
    pthread_create(&tid, NULL, periodical_detector, NULL);

    while (!finished) {
        printf("[main] Running normally...\n");
        sleep(1);
    }

    pthread_join(tid, NULL);
    printf("[main] Terminated due to unsafe state.\n");

    return 0;
}