#define _POSIX_C_SOURCE 199309L  // For CLOCK_MONOTONIC
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>  // Cho clock_gettime()

// Structure to pass data to threads
typedef struct {
    int thread_id;
    long long start;
    long long end;
    long long partial_sum;
    double thread_time;
    struct timespec start_time;
    struct timespec end_time;
} ThreadData;

// Function to calculate sum serially (for comparison)
long long calculate_sum_serial(long long n) {
    long long sum = 0;
    for (long long i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

// Hàm tính thời gian giữa hai điểm (theo giây)
double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

// Thread function to calculate partial sum
void* calculate_partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_sum = 0;

    // Dùng CLOCK_MONOTONIC để có độ chính xác cao và không bị ảnh hưởng bởi thay đổi thời gian hệ thống
    clock_gettime(CLOCK_MONOTONIC, &(data->start_time));


    for (long long i = data->start; i <= data->end; i++) {
        data->partial_sum += i;
    }

    clock_gettime(CLOCK_MONOTONIC, &(data->end_time));

    // Tính thời gian thực thi của thread theo giây
    data->thread_time = time_diff(data->start_time, data->end_time);


    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_threads> <n>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    long long n = atoll(argv[2]);

    if (num_threads <= 0 || n <= 0) {
        printf("Please provide positive integers for num_threads and n\n");
        return 1;
    }

    // Adjust num_threads if it's larger than n
    if (num_threads > n) {
        num_threads = n;
        printf("Adjusted number of threads to %d\n", num_threads);
    }

    printf("\n=== Multi-threaded Sum Calculation ===\n");

    struct timespec mt_start_time, mt_end_time;
    clock_gettime(CLOCK_MONOTONIC, &mt_start_time);

    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));

    long long current_start = 1;

    // Create threads
    printf("\nDistribution of work among threads:\n");
    printf("----------------------------------\n");
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].start = current_start;

        // Distribute remaining numbers evenly
        thread_data[i].end = (int)(n * (i + 1) / num_threads);
        current_start = thread_data[i].end + 1;

        printf("Thread %d: start = %lld, end = %lld, range size = %lld\n",
            thread_data[i].thread_id, thread_data[i].start, thread_data[i].end,
            thread_data[i].end - thread_data[i].start + 1);

        int rc = pthread_create(&threads[i], NULL, calculate_partial_sum, (void*)&thread_data[i]);
        if (rc) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }
    
    long long total_sum = 0;

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }

    clock_gettime(CLOCK_MONOTONIC, &mt_end_time);
    double total_time = time_diff(mt_start_time, mt_end_time);

    printf("\nMulti-threaded Summary:\n");
    printf("----------------------------------\n");
    printf("Sum of numbers from 1 to %lld using %d threads is %lld\n", n, num_threads, total_sum);
    printf("Total clock time: %.9f seconds\n", total_time);

    // Now calculate using serial method for comparison
    printf("\n=== Serial Sum Calculation ===\n");
    printf("----------------------------------\n");

    struct timespec serial_start_time, serial_end_time;
    clock_gettime(CLOCK_MONOTONIC, &serial_start_time);
    
    long long serial_sum = calculate_sum_serial(n);
    
    clock_gettime(CLOCK_MONOTONIC, &serial_end_time);
    double serial_time = time_diff(serial_start_time, serial_end_time);

    printf("Serial sum: %lld\n", serial_sum);
    printf("Serial execution time: %.9f seconds\n", serial_time);

    // Calculate and print speedup based on wall-clock time
    double speedup_total = serial_time / total_time;
    
    printf("\n=== Performance Comparison ===\n");
    printf("----------------------------------\n");
    printf("Serial time:           %.9f seconds\n", serial_time);
    printf("Multi-thread total time:   %.9f seconds\n", total_time);
    printf("Speedup:       %.4f\n", speedup_total);
    printf("----------------------------------\n");

    // Verify results
    if (total_sum == serial_sum) {
        printf("Results match! Both methods calculated the same sum.\n");
    }
    else {
        printf("ERROR: Results don't match! Multi-threaded: %lld vs Serial: %lld\n", total_sum, serial_sum);
    }

    // Check against mathematical formula n*(n+1)/2
    long long formula_sum = n * (n + 1) / 2;
    printf("Formula sum: %lld %s\n", formula_sum,
        (total_sum == formula_sum) ? "(matches calculated sum)" : "(doesn't match calculated sum)");

    // Clean up
    free(threads);
    free(thread_data);

    return 0;
}