#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    struct timespec start = {0,0};
    struct timespec end = {0,0};

    long long n = 2e9;

    long long first = 0, second = 1, temp = 0;

    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i<n; i++)
    {
        temp = first;
        first = second;
        second += temp;
        //printf("%lld\n", second);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    double dt = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
    printf("%f\n", dt);
    return 0;
}