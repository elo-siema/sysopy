#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

void sito(bool *tab, unsigned int n)
{
	for (int i=2; i*i<=n; i++) 
    {						
        if(!tab[i])				
		for (int j = i*i ; j<=n; j+=i) 
            tab[j] = 1;	
    }
}

int main()
{    
    struct timespec start = {0,0};
    struct timespec end = {0,0};
	long long int n = 1e8;
	bool *tab;
	
	tab = malloc(n+1 * sizeof(bool));
	
	for(int i=2; i<=n; i++) //zerowanie tablicy
		tab[i] = 0;
	
    
    clock_gettime(CLOCK_REALTIME, &start);
	sito(tab, n); //przesianie liczb
    clock_gettime(CLOCK_REALTIME, &end);
	
    //wypisanie ostatniej liczby, żeby mieć pewność że sito się wykona
	for(long long int i=n-1;i>0;i--)
    {
		if(!tab[i])
        {
            printf("%lld\n",i);
            break;
        }
    }
	
    double dt = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
    printf("%f\n", dt);
	free(tab);
    return 0;
}