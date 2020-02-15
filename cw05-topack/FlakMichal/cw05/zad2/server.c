#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "queue.h"


#define MAX_CLIENTS 10
int clients_no = 0;
int client_pid[MAX_CLIENTS];
mqd_t client_queue[MAX_CLIENTS];

int received_exit = 0;

void intHandler(int dummy) {
    printf("S: Received stop, deleting queue...\n");
    delete_task_queue();
    //todo:: delete all queues
    printf("S: Deleted queue, goodbye\n");
    exit(0);
}

int find_cid(int pid){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(client_pid[i] == pid) {
            return i;
        }
    }
    return -1;
}

void respond(int pid, char *content){
    int cid = find_cid(pid);
    if(cid < 0) {
        printf("S: Client: %d not found.\n", cid);
    }

    mqd_t queue = client_queue[cid];

    struct my_msg msg;
    msg.id = cid;
    msg.mtype = RESP;
    strcpy(msg.content, content);

    int result = mq_send(queue, (const char*)&msg, sizeof(struct my_msg), 0);
    if(result < 0) {
        perror("S: Failed to send response");
        exit(0);
    }
}

//msg handling
void handle_hello(struct my_msg *msgp){
    printf("S: Received HELLO\n");
    clients_no++;
    client_pid[clients_no] = msgp->id;
    mqd_t queue = mq_open(msgp->content, O_CREAT, 777, NULL);

    printf("S: Opening client Queue: key: %s\n", msgp->content);
    printf("S: Opening client Queue: qid: %d\n", queue);
    if(queue < 0) {
        perror("S: Failed to create client queue");
        exit(0);
    }
    client_queue[clients_no] = queue;

    respond(msgp->id, "ACK"); 
}

void handle_time(struct my_msg *msgp){
    printf("S: Received TIME\n");
    time_t timer;
    time(&timer);
    char timeStr[30];
    struct tm * timeinfo = localtime(&timer);
    strftime(timeStr, 20, "%b %d %H:%M", timeinfo);

    respond(msgp->id, timeStr);
}

void handle_stop(struct my_msg *msgp){
    printf("S: Received STOP\n");
    int cid = find_cid(msgp->id);
    mqd_t queue = client_queue[cid];

    int result = mq_close(queue);
    if(result < 0) {
        perror("S: Failed to remove client queue");
        exit(0);
    }
    client_pid[cid] = 0;
    client_queue[cid] = 0;
}

void handle_end(){
    printf("S: Received END\n");
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(client_queue[i] > 0) {
            mq_close(client_queue[i]);
        }
    }
    received_exit = 1;
}

void listen(int task_queue){
    struct my_msg msg;
    int result = 0;
    int flags = 0;
    struct timespec abs_timeout;
    abs_timeout.tv_sec = 0;
    abs_timeout.tv_nsec = 0;
    unsigned int msg_prio;
    while(1){
        if(received_exit){
            result = mq_timedreceive(task_queue, &msg, sizeof(struct my_msg), &msg_prio, &abs_timeout);
        }
        else {
            result = mq_receive(task_queue, &msg, sizeof(struct my_msg), &msg_prio);
        }

        if(result < 0) {
            perror("S: No more messages in the queue");
    	    printf("err: %d\n", result);
            exit(0);
        }


        printf("S: Received message!\n");
        switch(msg.mtype) {
            case HELLO:
                handle_hello(&msg);
                break;
            case TIME:
                handle_time(&msg);
                break;
            case STOP:
                handle_stop(&msg);
                break;
            case END:
                handle_end();
                break;
            default:
                perror("S: Wrong msg type received");
                break;
        }
    }
}

int main(int argc, char **argv)
{

    printf("S: Starting server...\n");
    printf("S: Setting signal handling...\n");
    signal(SIGINT, intHandler);
    printf("S: Creating task queue...\n");
    mqd_t queue = get_task_queue();
    printf("S: Server queue created: %d - listening...\n", queue);
    listen(queue);
    printf("S: Ended, deleting server queue...\n");
    delete_task_queue();
    printf("S: Deleted server queue, exiting...\n");
    return 0;
}
