#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <dlfcn.h>
//#include "fibonacci_lib.h"

int main() {
    struct timespec start = {0,0};
    struct timespec end = {0,0};
    clock_gettime(CLOCK_REALTIME, &start);

    long long int n = 2e9;
    long long int result = 0;

    void *handle = dlopen("./fibonacci_lib.so", RTLD_LAZY); 
    if(!handle){
        printf("Could not load fibonacci_lib.so");
        return -1;
    }
    long long int (*lib_fun)(long long int n);
    lib_fun = (long long int (*)(long long int))dlsym(handle,"fib"); 
    if(dlerror() != NULL){        
        printf("Could not find  function \"fib\" in fibonacci_lib.so");
        return -1;
    }
    result = (*lib_fun)(n);
    dlclose(handle);

    printf("%lld\n", result);

    clock_gettime(CLOCK_REALTIME, &end);
    double dt = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
    printf("%f\n", dt);
    return 0;
}