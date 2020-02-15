#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

void QInit(struct Queue* q, int size){
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    q->size = size;
    q->queueIn = 0;
    q->queueOut = 0;    
    
    int err = pthread_mutex_init(&(q->mutex), &attr);
    if((err)){
        printf("Failed to initialize mutex");
        exit(-1);
    }
    
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&(q->empty), &cattr);
    pthread_cond_init(&(q->full), &cattr);
    
}

int QPut(struct Queue* q, char* e){
    pthread_mutex_lock(&(q->mutex));
    while(q->queueIn == ((q->queueOut -1 + q->size) % q->size)){
        pthread_cond_wait(&(q->full), &(q->mutex));
    }
    
    q->elements[q->queueIn] = e;
    q->queueIn = (q->queueIn + 1) % q->size;
    
    pthread_cond_signal(&(q->empty));
    pthread_mutex_unlock(&(q->mutex));
    
    return 0;
}

char* QGet(struct Queue* q){

    pthread_mutex_lock(&(q->mutex));
    while(q->queueIn == q->queueOut){
        pthread_cond_wait(&(q->empty), &(q->mutex));
    }
    
    char* e = q->elements[q->queueOut];
    q->queueOut = (q->queueOut + 1) % q->size;
    
    pthread_cond_signal(&(q->full));
    pthread_mutex_unlock(&(q->mutex));
    
    return e;
}

char* QTryGet(struct Queue* q){
    
    pthread_mutex_lock(&(q->mutex));
    if(q->queueIn == q->queueOut){
        pthread_mutex_unlock(&(q->mutex));
        return NULL;
    }
    
    char* e = q->elements[q->queueOut];
    q->queueOut = (q->queueOut + 1) % q->size;
    
    pthread_cond_signal(&(q->full));
    pthread_mutex_unlock(&(q->mutex));
    return e;
}

void QDel(struct Queue* q){    
    pthread_mutex_destroy(&(q->mutex));
}

int QEmpty(struct Queue* q){
    pthread_mutex_lock(&(q->mutex));
    if(q->queueIn == q->queueOut){
        pthread_mutex_unlock(&(q->mutex));
        return 1;
    }
    pthread_mutex_unlock(&(q->mutex));
    return 0;
}

#define BUFF_SIZE (200)

struct Queue q;

pthread_t* producers = 0;
pthread_t* consumers = 0;
struct config {
    //N
    int size;
    //P
    int prod_num;
    //K
    int cons_num;
    //L
    int L;
    //information mode
    //0 - basic
    //1 - full
    int inf_mode;
    //nk
    int NK;
    //'<', '=', '>'
    char search_mode;
    //file name
    char file_name[1000];
};
struct config conf;

FILE* file;
char* lock_name = "lock2";
sem_t* file_lock;

//producers finished producing
char* finished_name = "finished";
sem_t* finished;

char* consumers_name = "consumers";
sem_t* consumers_locked;

void cleanup(){
    sem_unlink(lock_name);
    sem_unlink(finished_name);
    sem_unlink(consumers_name);
    QDel(&q);
    exit(0);
}

void* produce(void* args){
    while(1){
        char* buffer = (char*)malloc(BUFF_SIZE*sizeof(char));
        int res;
        char newline;
        
        //Read the data
        sem_wait(file_lock);
        res = fscanf(file, "%100[^\n]", buffer);
        fscanf(file, "%c", &newline);
        sem_post(file_lock);
        
        //If the entire file has been read
        if(res == EOF){
            sem_post(finished);
            free(buffer);
            return NULL;
        }
        if(conf.inf_mode)printf("Producer: read line %s.\n", buffer);
        QPut(&q, buffer);
    }
}

void* consume(void* args){
    while(1){
        sem_wait(consumers_locked);
        char* buf = QGet(&q);
        sem_post(consumers_locked);
        int size = strlen(buf);
        int matches = 0;
        switch(conf.search_mode){
        case '<':
            if(conf.L < size) matches = 1;
            break;
        case '>':
            if(conf.L > size) matches = 1;
            break;
        case '=':
            if(conf.L == size) matches = 1;
            break;
        };
        if(conf.inf_mode){
            if(matches)printf("Consumer: read line %s.\n", buf);
            else printf("Consumer: line \"%s\" didn't match search mode!\n", buf);
        }else if(matches)printf("%s\n", buf);
        free(buf);
    }
}

void* consume_special(void* args){
    sem_wait(finished);
    while(1){
        char* buf = QTryGet(&q);
        if(buf == NULL){
            while(1){
                int val;
                sem_getvalue(consumers_locked, &val);
                if(val == 0)break;
            }
            if(conf.inf_mode)printf("Consumers consumed the entire file.\n");
            cleanup(0);
        }else{
            int size = strlen(buf);
            int write = 0;
            switch(conf.search_mode){
            case '<':
                if(conf.L < size) write = 1;
                break;
            case '>':
                if(conf.L > size) write = 1;
                break;
            case '=':
                if(conf.L == size) write = 1;
                break;
            };
            if(conf.inf_mode){
                if(write)printf("Consumer: read line %s.\n", buf);
                else printf("Consumer: line \"%s\" didn't match search mode!\n", buf);
            }else if(write)printf("%s", buf);
            free(buf);
        }
    }
}

int main(int argc, char* argv[]){
    
    printf("Starting program");
    //wyczysc semafory na wszelki
    sem_unlink(lock_name);
    sem_unlink(finished_name);
    sem_unlink(consumers_name);
    QDel(&q);

    printf("Opening config:");
    auto config = fopen("config.txt", "r");
    size_t len = 50;
    char line[1000];
    if(config == NULL){
        printf("Failed to open file");
        exit(0);
    }
    printf("Reading prod_num:\n");
    fgets(line, len, config);
    conf.prod_num = atoi(line);

    printf("Reading cons_num:\n");
    fgets(line, len, config);
    conf.cons_num = atoi(line);

    printf("Reading size:\n");
    fgets(line, len, config);
    conf.size = atoi(line);

    printf("Reading file_name:\n");
    fgets(line, len, config);
    strncpy(conf.file_name, line, 1000);
    conf.file_name[strcspn(conf.file_name, "\r\n")] = 0; // remove trailing newline

    printf("Reading L:\n");
    fgets(line, len, config);
    conf.L = atoi(line);

    printf("Reading search_mode:\n");
    fgets(line, len, config);
    conf.search_mode = line[0];

    printf("Reading inf_mode:\n");
    fgets(line, len, config);
    conf.inf_mode = atoi(line);

    printf("Reading NK:\n");
    fgets(line, len, config);
    conf.NK = atoi(line);

    fclose(config);
    if(conf.NK){
        signal(SIGALRM, cleanup);
    }
    
    signal(SIGINT, cleanup); 
    QInit(&q, conf.size);
    
    file = fopen(conf.file_name, "r");
    if(file == NULL){
        printf("Failed to open file");
        exit(0);
    }
    
    //tworzenie semaforow
    file_lock = sem_open(lock_name, O_RDWR | O_CREAT, 0666, 1);
    if(file_lock == SEM_FAILED){
        printf("Failed to create a semaphore");
        exit(0);
    }
    finished = sem_open(finished_name, O_RDWR | O_CREAT, 0666, 0);
    if(finished == SEM_FAILED){
        printf("Failed to create a semaphore");
        exit(0);
    }
    consumers_locked = sem_open(consumers_name, O_RDWR | O_CREAT, 0666, conf.cons_num);
    
    //tworzenie watkow
    producers = (pthread_t*)malloc(conf.prod_num*sizeof(pthread_t));
    consumers = (pthread_t*)malloc((conf.cons_num+1)*sizeof(pthread_t));
    
    for(int i = 0; i < conf.prod_num; ++i){
        pthread_create(&producers[i], NULL, &produce, NULL);
    }
    
    for(int i = 1; i < conf.cons_num+1; ++i){
        pthread_create(&consumers[i], NULL, &consume, NULL);
    }
    pthread_create(&consumers[0], NULL, &consume_special, NULL);
    
    //ustawianie timera
    if(conf.NK)alarm(conf.NK);
    
    //konczenie watkow
    for(int i = 0; i < conf.prod_num; ++i){
        pthread_join(producers[i], NULL);
    }
    for(int i = 0; i < conf.cons_num+1; ++i){
        pthread_join(consumers[i], NULL);
    }
    
    cleanup();
}



