#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include "queue.h"

char __resp_queue_key[20];
mqd_t __resp_queue = 0;
mqd_t __task_queue = 0;
int my_id = 0;
int received_exit = 0;

char* create_and_get_resp_queue_key() {
    __resp_queue_key[0] = '/';
    //generate others
    mqd_t queue = mq_open(__resp_queue_key, O_CREAT, 777, NULL);
    if(queue < 0) {
        perror("Failed to create task queue");
        exit(0);
    }
    __resp_queue = queue;
    return __resp_queue_key;
}

void delete_resp_queue(){
    int result = mq_close(__resp_queue);
    if(result < 0) {
        perror("Failed to close resp queue");
        exit(0);
    }
    result = mq_unlink(__resp_queue_key);
    if(result < 0) {
        perror("Failed to unlink resp queue");
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
    struct my_msg msg;
    msg.id = getpid();
    msg.mtype = (long)type;
    strcpy(msg.content, content);

    
    printf("C: Sending id: %d\n", msg.id);
    printf("C: Sending type: %ld\n", msg.mtype);
    printf("C: Sending content: %s\n", msg.content);

    size_t size = sizeof(struct my_msg);

    int result = mq_send(__task_queue, &msg, size, 0);
    if(result < 0) {
        perror("C: Failed to send response");
        exit(0);
    }
}



void wait_for_response(enum MSG_TYPE type){
    struct my_msg msg;
    int result = 0;
    int flags = 0;

    unsigned int msg_prio;
    result = mq_receive(__resp_queue, &msg, sizeof(struct my_msg), 0, &msg_prio);

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
    char* key = create_and_get_resp_queue_key();
    
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
