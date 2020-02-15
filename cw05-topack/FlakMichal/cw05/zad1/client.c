#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"

void intHandler(int dummy) {
    printf("C: Received ctrl+c, goodbye\n");
    exit(0);
}

int __resp_queue = 0;
int __task_queue = 0;
int my_id = 0;

int get_resp_queue() {
    key_t key = ftok("$HOME", Q_TASK_KEY_CONST);
    int queue = msgget(key, IPC_CREAT);
    if(queue < 0) {
        perror("Failed to create task queue");
        exit(0);
    }
    __resp_queue = queue;
    return queue;
}

void delete_resp_queue(){
    int result = msgctl(__resp_queue, IPC_RMID, NULL);
    if(result < 0) {
        perror("Failed to remove task queue");
        exit(0);
    }
}

void send_msg(enum MSG_TYPE type, char* content){
    printf("C: Sending message\n");  
    struct msgbuf msg;
    msg.id = getpid();
    msg.type = (long)type;
    strcpy(msg.content, content);

    
    printf("C: Sending id: %d\n", msg.id);
    printf("C: Sending type: %d\n", msg.type);
    printf("C: Sending content: %s\n", msg.content);

    int result = msgsnd(__task_queue, &msg, sizeof(struct msgbuf) - sizeof(long), 0);
    if(result < 0) {
        perror("C: Failed to send response");
        exit(0);
    }
}



void wait_for_response(enum MSG_TYPE type){
    struct msgbuf msg;
    int result = 0;
    int flags = 0;
    result = msgrcv(__resp_queue, &msg, sizeof(struct msgbuf) - sizeof(long), 0, flags);

    if(result < 0) {
        perror("C: Failed to receive msg");
        printf("err: %d\n", result);
        exit(0);
    }
    switch(type) {
            case HELLO:
                printf("C: Received ACK\n");
                break;
            case TIME:
                printf("C: Received time: %s\n", msg.content);
                break;
            default:
                perror("C: Wrong msg type received");
                break;
        }
    printf("Received msg: %s\n", msg.content);
}


int main(int argc, char **argv)
{
    
    printf("C: Initializing signal handling\n");
    signal(SIGINT, intHandler);
    printf("C: Creating task queue\n");
    int task_queue = get_task_queue();
    __task_queue = task_queue;
    printf("C: Creating resp queue\n");
    int resp_queue = get_resp_queue();

    int stopped = 0;
    
    char command[40];

    printf("C: Establishing handshake\n");  
    char resp_queue_str[100];
    snprintf(resp_queue_str, 100, "%d", resp_queue);
    send_msg(HELLO, resp_queue_str);  
    printf("C: Waiting for response from handshake\n"); 
    wait_for_response(HELLO);

    printf("C: Entering loop\n");
    while(1){
        gets(command, 4);
        if(!strcmp(command, "STOP")) {
            
            printf("Sending STOP, Exiting...\n");
            return 0;        
        }
        if(!strcmp(command, "END")) {
            
            printf("Sending END...\n");
            return 0;        
        }
        if(!strcmp(command, "DATE")) {
            
            printf("Sending DATE...\n");
            return 0;        
        }
    }    

    return 0;
}
