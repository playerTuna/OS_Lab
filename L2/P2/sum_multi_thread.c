#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    long long start;
    long long end;
    long long partial_sum;
} ThreadData;

// Hàm tính tổng một phần
void *partial_sum_func(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->partial_sum = 0;

    for (long long i = data->start; i <= data->end; i++) {
        data->partial_sum += i;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <numThreads> <n>\n", argv[0]);
        return 1;
    }

    int numThreads = atoi(argv[1]);
    long long n = atoll(argv[2]);

    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    ThreadData *threadData = malloc(numThreads * sizeof(ThreadData));

    if (!threads || !threadData) {
        perror("Memory allocation failed");
        return 1;
    }

    // Chia công việc
    long long chunk = n / numThreads;
    long long remainder = n % numThreads;

    long long current_start = 1;
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = current_start;
        long long extra = (i < remainder) ? 1 : 0;
        threadData[i].end = current_start + chunk - 1 + extra;
        current_start = threadData[i].end + 1;

        // Tạo thread
        if (pthread_create(&threads[i], NULL, partial_sum_func, &threadData[i]) != 0) {
            perror("pthread_create failed");
            free(threads);
            free(threadData);
            return 1;
        }
    }

    // Chờ tất cả thread hoàn thành
    long long total_sum = 0;
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += threadData[i].partial_sum;
    }

    printf("Sum of numbers from 1 to %lld is: %lld\n", n, total_sum);

    // Giải phóng bộ nhớ
    free(threads);
    free(threadData);

    pthread_exit(NULL);
}
