#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "queue.h"

//keep queue id, reduces code duplication for cleanup
int __q_task_queue = 0;

int get_task_queue(){
    key_t key = ftok("/home", Q_TASK_KEY_CONST);
    printf("Key: %d\n", key);
    int queue = msgget(key, IPC_CREAT);
    printf("Queue: %d\n", queue);
    if(queue < 0) {
        perror("Failed to create task queue");
        exit(0);
    }
    __q_task_queue = queue;
    return queue;
}

void delete_task_queue(){
    int result = msgctl(__q_task_queue, IPC_RMID, NULL);
    if(result < 0) {
        perror("Failed to remove task queue");
        exit(0);
    }
}
