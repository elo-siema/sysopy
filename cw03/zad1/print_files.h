#ifndef PRINT_FILES_H
#define PRINT_FILES_H
#include <time.h>

typedef enum {
    LESS,
    MORE,
    EQUAL,
    D_DEFAULT
} date_is;


void print_files(const char* path, date_is operand, struct tm date);
#endif

