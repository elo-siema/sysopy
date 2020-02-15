#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <semaphore.h>

struct Queue{
    char* elements[500];
    int size;
    int queueIn;
    int queueOut;
    
    sem_t* read;
    sem_t* empty;
    sem_t* full;
};

void QInit(struct Queue* q, int size);
int QPut(struct Queue* q, char* e);
char* QGet(struct Queue* q);
//Timed QGet version. Returns after time specifed in timer.
char* QTimedGet(struct Queue* q, const struct timespec* timer);
//Try to take QGet version. Leaves if Queue is empty.
char* QTryGet(struct Queue* q);
void QDel(struct Queue* q);
int QEmpty(struct Queue* q);

#endif