#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#include "print_files.h"

const char* date_is_str[] = {
    "<",
    ">",
    "="
};

//Assuming d -> m -> Y:
const char* strptime_formats[] = {
    "%d/%m/%Y",
    "%d.%m.%Y",
    "%d-%m-%Y",
    "%Y/%m/%d",
    "%Y.%m.%d",
    "%Y-%m-%d"
};


int main(int argc, char ** argv) {
    char* path;
    date_is dis = D_DEFAULT;
    struct tm date;

    


    if (argc != 4) {
        perror("Please provide 3 arguments:");
        perror("1) path to directory");
        perror("2) '<', '>' or '='");
        perror("3) date in d/m/Y or d.m.Y or d-m-Y format");
        exit(1);
    }

    path = argv[1];
    
    if(strcmp(argv[2], date_is_str[LESS])==0) dis = LESS;
    else if(strcmp(argv[2], date_is_str[MORE])==0) dis = MORE;
    else if(strcmp(argv[2], date_is_str[EQUAL])==0) dis = EQUAL;
    else {
        perror("Wrong second argument, allowed values:");
        perror(" '<', '>' or '='");
        return 1;
    }
    for(int i = 0; i<NELEMS(strptime_formats); i++) {
        if(strptime(argv[3], strptime_formats[i], &date) != NULL) {
            break;
        }
        else if(i == NELEMS(strptime_formats) - 1) {
            perror("Error parsing provided date");
            return 1;
        }
    }

    //debug:
    //printf(path);
    //printf("\n");
    //printf(date_is_str[dis]);
    //printf("\n");
    //printf("%d",(date.tm_year));
    //printf("\n");
    //printf("%d",(date.tm_mon));
    //printf("\n");
    //printf("%d",(date.tm_mday));
    printf("\n");

    print_files(path, dis, date);
    printf("\n");


    return 0;
}