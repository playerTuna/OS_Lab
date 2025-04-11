#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool finished = false; // Dùng để mô phỏng việc hệ thống đã hoàn tất hoặc dừng lại

// Hàm kiểm tra an toàn
int is_safe() {
    // TODO: thực hiện kiểm tra định kỳ
    int found = rand() % 10 > 2; // 70% an toàn, 30% không an toàn
    if (!found) {
        return -1; // phát hiện không an toàn
    }
    return 0; // an toàn
}

// Thread kiểm tra định kỳ
void* periodical_detector(void* arg) {
    while (1) {
        sleep(5); // kiểm tra mỗi 5 giây
        pthread_mutex_lock(&lock);

        if (is_safe() != 0) {
            printf("Abnormal detected! Taking corrective action...\n");

            if (!finished) {
                printf("Terminating due to unsafe condition.\n");
                pthread_mutex_unlock(&lock);
                break;
            }
        } else {
            printf("System is safe.\n");
        }

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main() {
    srand(time(NULL)); // Seed cho is_safe()
    pthread_t detector_thread;

    // Khởi tạo thread kiểm tra định kỳ
    if (pthread_create(&detector_thread, NULL, periodical_detector, NULL) != 0) {
        perror("Failed to create detector thread");
        return 1;
    }

    // Giả lập làm việc trong 20 giây rồi dừng
    sleep(20);
    pthread_mutex_lock(&lock);
    finished = true;
    pthread_mutex_unlock(&lock);

    pthread_join(detector_thread, NULL);
    printf("Monitoring stopped.\n");
    return 0;
}
