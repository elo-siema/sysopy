#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include "queue.h"


#define MAX_CLIENTS 10
int clients_no = 0;
int client_pid[MAX_CLIENTS];
int client_queue[MAX_CLIENTS];

int received_exit = 0;

void intHandler(int dummy) {
    printf("S: Received stop, deleting queue...\n");
    delete_task_queue();
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

    int queue = client_queue[cid];

    struct msgbuf msg;
    msg.id = cid;
    msg.type = atoi(RESP);
    strcpy(msg.content, content);

    int result = msgsnd(queue, &msg, sizeof(struct msgbuf) - sizeof(long), 0);
    if(result < 0) {
        perror("S: Failed to send response");
        exit(0);
    }
}

//msg handling
void handle_hello(struct msgbuf *msgp){
    clients_no++;
    client_pid[clients_no] = msgp->id;
    client_queue[clients_no] = (int)msgp->content[0];

    respond(msgp->id, "ACK"); 
}

void handle_time(struct msgbuf *msgp){
    respond(msgp->id, "ACK TIME");
}

void handle_stop(struct msgbuf *msgp){
    int cid = find_cid(msgp->id);
    int queue = client_queue[cid];

    int result = msgctl(queue, IPC_RMID, NULL);
    if(result < 0) {
        perror("S: Failed to remove client queue");
        exit(0);
    }
    client_pid[cid] = 0;
    client_queue[cid] = 0;
}

void handle_end(){
    received_exit = 1;
}

void listen(int task_queue){
    struct msgbuf msg;
    int result = 0;
    int flags = 0;
    while(1){
        if(received_exit){
            flags = IPC_NOWAIT;
        }
        result = msgrcv(task_queue, &msg, sizeof(struct msgbuf) - sizeof(long), 0, flags);

        if(result < 0) {
            perror("S: Failed to receive msg");
    	    printf("err: %d\n", result);
            exit(0);
        }

        switch(msg.type) {
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
    int queue = get_task_queue();
    printf("S: Server queue created: %d - listening...\n", queue);
    listen(queue);
    printf("S: Ended, deleting server queue...\n");
    delete_task_queue();
    printf("S: Deleted server queue, exiting...\n");
    return 0;
}