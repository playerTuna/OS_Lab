#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ll long long

typedef struct {
    ll s; 
    ll e;
    ll sum;
} Sum;

void* partial_sum(void* arg) {
    Sum* args = (Sum*) arg;
    args->sum = 0;
    for (ll i = args->s; i <= args->e; ++i) {
        args->sum += i;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <numThreads> <number>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    ll n = atoll(argv[2]);
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    Sum *args = malloc(num_threads * sizeof(Sum));

    // ll chunk = n / num_threads;
    ll start = 1;
    // ll r = n % num_threads; // remainder

    for (int i = 0; i < num_threads; ++i) {
        args[i].s = start;
        // args[i].e = start + chunk - 1 + (num_threads - i - 1 < r);
        args[i].e = n * (i + 1) / num_threads;
        start = args[i].e + 1;
        if (pthread_create(&threads[i], NULL, partial_sum, &args[i])){
            printf("Error creating thread %d\n", i);
            free(threads);
            free(args);
            return 1;
        }
    }

    ll total_sum = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        total_sum += args[i].sum;
        // printf("Thread %d: Sum from %lld to %lld is %lld\n", i, args[i].s, args[i].e, args[i].sum);
    }

    printf("Sum from 1 to %lld using %d threads is %lld\n", n, num_threads, total_sum);
    // printf(n * (n + 1) / 2 == total_sum ? "Correct\n" : "Incorrect\n");

    free(threads);
    free(args);
    pthread_exit(NULL);

    return 0;
}
