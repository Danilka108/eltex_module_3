#include "common.h"
#include "interproc.h"
#include <stdlib.h>

int init_msgq() {
  key_t msgq_key = ftok(MSGQ_PATHNAME, MSGQ_PROJ_ID);
  if (msgq_key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  int msgq_id = msgget(msgq_key, IPC_CREAT | S_IWUSR | S_IRUSR);
  if (msgq_id == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  return msgq_id;
}

struct msgbuf receive_msg(int msgq, long type) {
  struct msgbuf msg;
  msg.mtype = type;

  printf("wait message...\n");

  if (msgrcv(msgq, &msg, MSG_MAX_SIZE, type, 0) == -1) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
  }

  printf("message received: %s\n", msg.mtext);

  return msg;
}

void send_msg(int msgq, long type) {
  struct msgbuf msg;
  msg.mtype = type;

  printf("message: ");
  if (fgets(msg.mtext, MSG_MAX_SIZE, stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }

  msg.mtext[strlen(msg.mtext) - 1] = '\0';
  if (msgsnd(msgq, &msg, strlen(msg.mtext) + 1, 0) == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }
  printf("message sent\n");
}

void close_msgq(int msgq) {
  if (msgctl(msgq, IPC_RMID, NULL) < 0) {
    perror("msgctl");
    exit(EXIT_FAILURE);
  }
}
