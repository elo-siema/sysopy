#include <stdio.h>
#include <time.h>
#include <stdlib.h>

long long _fib(long long n, long long left, long long right) {
  switch (n) {
    case 0: return 0;
    case 1: return right;    
  }
  return _fib(n - 1, right, left + right);
}

long long fib(long long n) {
  return _fib(n, 0, 1);
}

int main() {
    struct timespec start = {0,0};
    struct timespec end = {0,0};

    long long n = 2e5;

    clock_gettime(CLOCK_REALTIME, &start);
    printf("%llu\n", fib(n));
    clock_gettime(CLOCK_REALTIME, &end);

    double dt = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
    printf("%f\n", dt);
    return 0;
}
