// sum_serial.c
#include <stdio.h>
#include <stdlib.h>

int main (int arvc, char * arvg[]){
    if (arvc != 2){
        printf("Usage: %s ./sum_serial <number>\n", arvg[0]);
    }
    long  n = atol(arvg[1]);
    long long sum = 0;
    for (long i=1 ;i<=n;i++){
        sum+=i;
    }
    printf("sum_serial = %lld\n",sum);
    return 0;
}
