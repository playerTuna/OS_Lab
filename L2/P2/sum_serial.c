#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Exception handling if input format is wrong
    if (argc != 2) {
        fprintf(stderr, "Usage: %s n\n", argv[0]);
        return 1;
    }

    // Initialize variables from input
    long long n = atoll(argv[1]);
    long long sum = 0;

    // Main computation
    for (long long i = 1; i <= n; i++) {
        sum += i;
    }

    // Output
    printf("Sum of numbers from 1 to %lld is: %lld\n", n, sum);
    return 0;
}
