#include "fibonacci_lib.h"

long long int fib(long long int n) {

    long long first = 0, second = 1, temp = 0;

    for(int i = 0; i<n; i++)
    {
        temp = first;
        first = second;
        second += temp;
    }
    
    return second;
}