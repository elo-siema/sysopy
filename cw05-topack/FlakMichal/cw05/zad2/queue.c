#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "queue.h"

//keep queue id, reduces code duplication for cleanup
mqd_t __q_task_queue = 0;

int get_task_queue(){
    mqd_t queue = mq_open(Q_TASK_NAME_CONST, O_CREAT, 777, NULL);
    printf("Queue: %d\n", queue);
    if(queue < 0) {
        perror("Failed to create task queue");
        exit(0);
    }
    __q_task_queue = queue;
    return queue;
}

void delete_task_queue(){
    int result = mq_unlink(Q_TASK_NAME_CONST);
    if(result < 0) {
        perror("Failed to remove task queue");
        exit(0);
    }
}
