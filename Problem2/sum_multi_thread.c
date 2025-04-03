#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
    long long start;
    long long end;
} ThreadArg;

void* worker(void* arg) {
    ThreadArg* t_arg = (ThreadArg*)arg;
    long long start = t_arg->start;
    long long end = t_arg->end;
    free(t_arg);

    long long partial_sum = 0;
    for (long long i = start; i <= end; i++) {
        partial_sum += i;
    }
    long long* ret = malloc(sizeof(long long));
    if (ret == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    *ret = partial_sum;
    pthread_exit(ret);
    // sleep(5);
}

long long sum_multi_thread(int n, int num_threads) {
    pthread_t threads[num_threads];
    int chunk_size = n / num_threads;
    int remainder = n % num_threads;
    int current = 1;
    long long total_sum = 0;

    for (int i = 0; i < num_threads; i++) {
        ThreadArg* arg = malloc(sizeof(ThreadArg));
        if (arg == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        arg->start = current;
        arg->end = current + chunk_size - 1;
        if (i < remainder) {
            arg->end += 1; 
        }
        current = arg->end + 1;

        if (pthread_create(&threads[i], NULL, worker, arg) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // printf("Threads created. PID: %d\n", getpid());
    // fflush(stdout);
    // sleep(2);

    for (int i = 0; i < num_threads; i++) {
        void* result;
        if (pthread_join(threads[i], &result) != 0) {
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
        long long partial_sum = *(long long*)result;
        free(result);
        total_sum += partial_sum;
    }

    return total_sum;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_threads> <n>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int n = atoi(argv[2]);
    long long result = sum_multi_thread(n, num_threads);
    printf("Sum from 1 to %d using %d threads: %lld\n", n, num_threads, result);
    return 0;
}

// #include <pthread.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include <unistd.h>

// typedef struct {
//     long long start;
//     long long end;
// } ThreadArg;

// void* worker(void* arg) {
//     ThreadArg* t_arg = (ThreadArg*)arg;
//     long long start = t_arg->start;
//     long long end = t_arg->end;
//     free(t_arg);

//     long long partial_sum = 0;
//     for (long long i = start; i <= end; i++) {
//         partial_sum += i;
//     }

//     // sleep(5);

//     return (void*)(intptr_t)partial_sum;
// }

// long long sum_multi_thread(int n, int num_threads) {
//     pthread_t threads[num_threads];
//     int chunk_size = n / num_threads;
//     int remainder = n % num_threads;
//     int current = 1;
//     long long total_sum = 0;

//     for (int i = 0; i < num_threads; i++) {
//         ThreadArg* arg = malloc(sizeof(ThreadArg));
//         arg->start = current;
//         arg->end = current + chunk_size - 1;
//         if (i < remainder) {
//             arg->end += 1;
//         }
//         current = arg->end + 1;

//         if (pthread_create(&threads[i], NULL, worker, arg) != 0) {
//             perror("pthread_create failed");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // printf("Threads created. PID: %d\n", getpid());
//     // fflush(stdout);
//     // sleep(2);

//     for (int i = 0; i < num_threads; i++) {
//         void* result;
//         if (pthread_join(threads[i], &result) != 0) {
//             perror("pthread_join failed");
//             exit(EXIT_FAILURE);
//         }
//         total_sum += (intptr_t)result;
//     }

//     return total_sum;
// }

// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         fprintf(stderr, "Usage: %s <num_threads> <n>\n", argv[0]);
//         return 1;
//     }

//     int num_threads = atoi(argv[1]);
//     int n = atoi(argv[2]);
//     long long result = sum_multi_thread(n, num_threads);
//     printf("Sum from 1 to %d using %d threads: %lld\n", n, num_threads, result);
//     return 0;
// }