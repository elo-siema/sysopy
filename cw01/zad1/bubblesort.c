#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const long long int N = 1e5*2;

void swap(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 
  
// A function to implement bubble sort 
void bubbleSort(int arr[], int n) 
{ 
   int i, j; 
   for (i = 0; i < n-1; i++)       
  
       // Last i elements are already in place    
       for (j = 0; j < n-i-1; j++)  
           if (arr[j] > arr[j+1]) 
              swap(&arr[j], &arr[j+1]); 
} 

int main() {
    int randoms[N];
    
    srand(time(NULL));
    for(int i = 0; i < 100; ++i)
    {
        randoms[i] = rand() % 2000000;
    }

    struct timespec start = {0,0};
    struct timespec end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    bubbleSort(randoms, N);
    printf("%d\n", randoms[N-1]);
    clock_gettime(CLOCK_REALTIME, &end);

    double dt = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
    printf("%f\n", dt);
    return 0;
}
