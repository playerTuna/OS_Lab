#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define ull unsigned long long

typedef struct {
    int start;
    int end;
} ThreadArg;

void* worker(void* arg) {
    ThreadArg* t_arg = (ThreadArg*)arg;
    int start = t_arg->start;
    int end = t_arg->end;
    // printf("Thread %ld: start = %llu, end = %llu\n", pthread_self(), start, end);
    free(t_arg);

    ull partial_sum = 0;
    for (int i = start; i <= end; i++) {
        partial_sum += i;
    }
    ull* retval = malloc(sizeof(ull));
    if (retval == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    *retval = partial_sum;
    pthread_exit(retval);
}

long long sum_multi_thread(int n, int num_threads) {
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    if (threads == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    int current = 1;
    ull total_sum = 0;

    for (int i = 0; i < num_threads; i++) {
        ThreadArg* arg = malloc(sizeof(ThreadArg));
        if (arg == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        arg->start = current;
        arg->end = (i + 1) * n / num_threads;
        current = arg->end + 1;

        if (pthread_create(&threads[i], NULL, worker, arg) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        void* result;
        if (pthread_join(threads[i], &result) != 0) {
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
        ull partial_sum = *(ull*)result;
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
    ull result = sum_multi_thread(n, num_threads);
    printf("Sum from 1 to %i using %d threads: %lld\n", n, num_threads, result);
    return 0;
}
/* Compile with: gcc -o sum_multi_thread sum_multi_thread.c -lpthread
   Run with: ./sum_multi_thread
   Code by Nem, Optimize by ZaMin

To use a Makefile, create a file named 'Makefile' in the same directory with the following content:

all: sum_serial sum_multi_thread

clean:
    rm -f sum_multi_thread

benchmark:
    Running benchmark...
    N = 1000000000, Threads = 10
    --------------------------------
    [Serial Version]
    Serial time: ... sec

    [Multi-thread Version]
    Multi_thread time: ... sec

    Speed-up: ...
*/