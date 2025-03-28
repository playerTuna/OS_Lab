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
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <numThreads> <number>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    ll n = atoll(argv[2]);
    pthread_t threads[num_threads];
    Sum args[num_threads];

    ll chunk = n / num_threads;
    ll start = 1;

    for (int i = 0; i < num_threads; ++i) {
        args[i].s = start;
        args[i].e = start + chunk - 1;
        if (i == num_threads - 1) args[i].e += n % num_threads; // last thread takes the remainder
        start = args[i].e + 1;
        pthread_create(&threads[i], NULL, partial_sum, &args[i]);
    }

    ll total_sum = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        total_sum += args[i].sum;
    }

    printf("Sum from 1 to %lld using %d threads is %lld\n", n, num_threads, total_sum);
    return 0;
}
