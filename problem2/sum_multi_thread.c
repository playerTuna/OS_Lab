// sum_multi_thread.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct  {
    long long start;
    long long end;
    long long partial_sum;
} ThreadData;

void* thread_sum(void *arg){
    ThreadData * data = (ThreadData*) arg; 
    long long sum = 0;
    for (long i = data->start; i <= data->end; i++) {
        sum += i;
        }
        data->partial_sum = sum;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./sum_multi_thread numThreads n\n");
        return 1;
    }

    int numThreads = atoi(argv[1]);
    long n = atol(argv[2]);

    // pthread_t threads[numThreads];
    // ThreadData thread_data[numThreads];
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    ThreadData *thread_data = malloc(numThreads * sizeof(ThreadData));
    long chunk = n / numThreads;
    long remainder = n % numThreads;

    long current_start = 1;

    for (int i=0; i<numThreads; i++) {
        thread_data[i].start = current_start;
        thread_data[i].end = (int)(n/numThreads)*((i+1)) ;
        pthread_create(&threads[i],NULL,thread_sum,&thread_data[i]);
        current_start = thread_data[i].end +1;
    }

    long long total_sum = 0;
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }
    printf("Multi-threaded Sum = %lld\n", total_sum);
    free(threads);
    free(thread_data);
    pthread_exit(NULL);
    return 0;
}
