#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to calculate sum of numbers from 1 to n
long long calculate_sum(long long n) {
    long long sum = 0;
    for (long long i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }

    long long n = atoll(argv[1]);
    
    if (n <= 0) {
        printf("Please provide a positive integer for n\n");
        return 1;
    }

    clock_t start = clock();
    
    long long result = calculate_sum(n);
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Sum of numbers from 1 to %lld is %lld\n", n, result);
    printf("Time taken: %f seconds\n", time_spent);

    return 0;
}