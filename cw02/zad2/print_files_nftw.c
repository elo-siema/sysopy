#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define __USE_XOPEN_EXTENDED 
#include <ftw.h>
#include "print_files.h"

struct tm _date; //ugly
date_is _operand;

int date_compare(struct tm *left, struct tm *right, date_is operand) {
    switch (operand){
        case EQUAL:
            return left->tm_year == right->tm_year
                && left->tm_mon == right->tm_mon 
                && left->tm_mday == right->tm_mday;
            break;
        case MORE:
            return left->tm_year < right->tm_year ||
                (left->tm_year == right->tm_year && left->tm_mon < right->tm_mon) ||
                (left->tm_year == right->tm_year && left->tm_mon == right->tm_mon && left->tm_mday < right->tm_mday);
            break;
        case LESS:
            return left->tm_year > right->tm_year ||
                (left->tm_year == right->tm_year && left->tm_mon > right->tm_mon) ||
                (left->tm_year == right->tm_year && left->tm_mon == right->tm_mon && left->tm_mday > right->tm_mday);
            break;
        default:
            perror("Operand not supported: date_compare");
            exit(1);
        
    }
    return 0;
}

void print_perms(mode_t perms) 
{
    printf( (perms & S_IRUSR) ? "r" : "-");
    printf( (perms & S_IWUSR) ? "w" : "-");
    printf( (perms & S_IXUSR) ? "x" : "-");
    printf( (perms & S_IRGRP) ? "r" : "-");
    printf( (perms & S_IWGRP) ? "w" : "-");
    printf( (perms & S_IXGRP) ? "x" : "-");
    printf( (perms & S_IROTH) ? "r" : "-");
    printf( (perms & S_IWOTH) ? "w" : "-");
    printf( (perms & S_IXOTH) ? "x" : "-");
}

int handle_file (const char *fpath, const struct stat *sb,
                   int typeflag, struct FTW *ftwbuf) {
    if(typeflag != FTW_F)  return 0;
    struct tm timeinfo;
    if(localtime_r(&sb->st_mtime, &timeinfo)==NULL){
        perror("Can't convert file stat to tm struct");
        exit(1);
    }
    if(!date_compare(&_date, &timeinfo, _operand)) return 0;
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d", &timeinfo);
    printf("\n");
    printf("path: %s, size: %ld, last modified: %s, permissions: ",
        fpath,
        sb->st_size,
        buffer
    );  
    print_perms(sb->st_mode);
    
    return 0;
}
void print_files(const char* path, date_is operand, struct tm date){
    const int nopenfd = 5;
    const int flags = FTW_PHYS;
    _date = date;
    _operand = operand;
    nftw(path,
        handle_file,
        nopenfd, flags);
}