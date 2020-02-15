#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 20

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Prosze podac nazwe pliku. \n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if(!f){
        printf("Podany plik nie istnieje. \n");
        return -1;
    }

    int sign = 0;
    while(sign != EOF){
        char* arg[MAX_ARGS] = {NULL};
        arg[0] = (char*)malloc(sizeof(char) * 256);
        int size;
        if(fscanf(f, "%s", arg[0]) == EOF) break;
        for(size = 1; size < MAX_ARGS-1; ++size){
            sign = fgetc(f);
            if(sign == (int)'\n' || sign == EOF) break;
            arg[size] = (char*)malloc(sizeof(char) * 100);
            fscanf(f, "%s", arg[size]);
        }
        if(!vfork()){
            execvp(arg[0], arg);
            return -1;
        }
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status)){
                printf("\nPolecenie zakonczylo sie bledem, polecenie:%s, kod: %i\n", arg[0], WEXITSTATUS(status));
                exit(-1);
            }
        }
        for(int i = 0; i < size; ++i){
            free(arg[i]);
        }
        
        printf("\n");
    }
    
    
    
    return 0;
}


