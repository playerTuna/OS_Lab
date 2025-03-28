#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        exit(1);
    }

    long long n = atoll(argv[1]);
    unsigned long long sum = 0; // n * (n + 1) / 2

    for (long long i = 1; i <= n; ++i) {
        sum += i;
    }

    printf("Sum from 1 to %lld is %lld\n", n, sum);
    return 0;
}
