#ifndef TASK_06_INTERPROC_H
#define TASK_06_INTERPROC_H

#define MSGQ_PATHNAME "msgqueue"
#define MSGQ_PROJ_ID 'A'
#define MSG_MAX_SIZE 64
#define EXIT_MSG "exit"
#define PROC_1_TYPE 1
#define PROC_2_TYPE 2

struct msgbuf {
  long mtype;
  char mtext[MSG_MAX_SIZE];
};

#endif
