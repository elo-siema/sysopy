#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define __USE_XOPEN_EXTENDED 
#include "print_files.h"


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

void traverse(const char *name, date_is operand, struct tm *date)
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    struct tm timeinfo;
    char buffer[80];

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        switch(entry->d_type) {

            case DT_DIR: //directory
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                traverse(path, operand, date); //recurse
                break;

            case DT_REG: //file
                if(stat(path, &st)){
                    perror("Stat failed\n");
                    perror(path);
                    exit(1);
                }

                if(localtime_r(&st.st_mtime, &timeinfo)==NULL){
                    perror("Can't convert file stat to tm struct");
                    exit(1);
                }

                if(!date_compare(date, &timeinfo, operand)) break;

                strftime(buffer, 80, "%Y-%m-%d", &timeinfo);

                printf("\npath: %s, size: %ld, last modified: %s, permissions: ",
                    path,
                    st.st_size,
                    buffer
                );  
                print_perms(st.st_mode);
                break;
        }
    }
    closedir(dir);
}

void print_files(const char* path, date_is operand, struct tm date){
    char buffer[256];
    strcpy(buffer, path);
    if (buffer && *buffer) {                      // make sure result has at least
        if (buffer[strlen(buffer) - 1] == '/')    // one character
            buffer[strlen(buffer) - 1] = 0;
    }
    traverse(buffer, operand, &date);
}