#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <pthread.h>

struct Queue{
    char* elements[500];
    int size;
    int queueIn;
    int queueOut;
    
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
};

void QInit(struct Queue* q, int size);
int QPut(struct Queue* q, char* e);
char* QGet(struct Queue* q);
char* QTryGet(struct Queue* q);
void QDel(struct Queue* q);
int QEmpty(struct Queue* q);

#endif