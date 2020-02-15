#ifndef _QUEUE_H_
#define _QUEUE_H_

#define MAX_MSG_LENGTH 200
#define Q_TASK_NAME_CONST "/TASKQ"

extern int get_task_queue();
extern void delete_task_queue();

enum MSG_TYPE{
    HELLO = 1, TIME = 2, STOP = 3, END = 4, RESP = 5
};

struct my_msg {
    char mtype;
    int id;
    char content[MAX_MSG_LENGTH];
};
 
#endif