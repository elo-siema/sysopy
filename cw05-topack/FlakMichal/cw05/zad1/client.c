#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "queue.h"


int __resp_queue = 0;
key_t __task_queue = 0;
int my_id = 0;
int received_exit = 0;

key_t create_and_get_resp_queue_key() {
    char* path = getenv("HOME");
    if(path == NULL){
        perror("server: getting environmental variable 'HOME' failed\n");
        exit(0);
    }
    key_t key = ftok(path, getpid());
    int queue = msgget(key, IPC_CREAT);
    if(queue < 0) {
        perror("Failed to create task queue");
        exit(0);
    }
    __resp_queue = queue;
    return key;
}

void delete_resp_queue(){
    int result = msgctl(__resp_queue, IPC_RMID, NULL);
    if(result < 0) {
        perror("Failed to remove task queue");
        exit(0);
    }
    printf("C: Deleted resp queue\n");
}

void intHandler(int dummy) {

    printf("C: Received ctrl+c, goodbye\n");
    received_exit = 1;
}


void send_msg(enum MSG_TYPE type, char* content){
    printf("C: Sending message\n");  
    struct msgbuf msg;
    msg.id = getpid();
    msg.mtype = (long)type;
    strcpy(msg.content, content);

    
    printf("C: Sending id: %d\n", msg.id);
    printf("C: Sending type: %ld\n", msg.mtype);
    printf("C: Sending content: %s\n", msg.content);

    size_t size = sizeof(struct msgbuf) - sizeof(long);

    int result = msgsnd(__task_queue, &msg, size, 0);
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
    key_t key = create_and_get_resp_queue_key();
    
    char command[40];

    printf("C: Establishing handshake, sending key of privete q:\n");  
    char key_str[100];
    snprintf(key_str, 100, "%d", key);
    send_msg(HELLO, key_str);  
    printf("C: Waiting for response from handshake\n"); 
    wait_for_response(HELLO);

    printf("C: Entering loop\n");
    while(!received_exit){
        fgets(command, 5, stdin);
        //remove newline from command:
        if (command[strlen(command)-1] == '\n') command[strlen(command)-1] = '\0';
        if(!strcmp(command, "STOP")) {
            
            printf("Exiting...\n");
            break;
        }
        if(!strcmp(command, "END")) {
            
            printf("Sending END...\n");
            send_msg(END, "\0"); 
        }
        if(!strcmp(command, "TIME")) {
            
            printf("Sending TIME...\n");
            send_msg(TIME, "\0"); 
            wait_for_response(TIME);
        }
    }    

    printf("C: Exiting...\n");
    delete_resp_queue();
    exit(0);

    return 0;
}
