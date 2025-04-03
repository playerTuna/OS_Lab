#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

long long sum_serial(long long n) {
    int sum = 0;
    for (long long i = 1; i <= n; i++)
        sum += i;
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }
    long long n = atoi(argv[1]);
    printf("%lld\n", sum_serial(n));
    return 0;
}