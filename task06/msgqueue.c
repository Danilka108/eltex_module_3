#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#include "msgqueue.h"

msgqueue_t msgqueue_init(const char *pathname, int proj_id, mode_t mode) {
  msgqueue_t self;

  key_t msgq_key = ftok(pathname, proj_id);
  if (msgq_key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  self._id = msgget(msgq_key, IPC_CREAT | mode);
  if (self._id == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  return self;
}

void msgqueue_recv(msgqueue_t *self, msg_t *msg, int *exit_msg) {
  if (msgrcv(self->_id, msg, MSG_SIZE, 0, 0) == -1) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
  }

  *exit_msg = msg->mtype == MSG_EXIT_TYPE;
}

void msgqueue_send(msgqueue_t *self, msg_t *msg, int exit_msg) {
  msg->mtype = exit_msg ? MSG_EXIT_TYPE : MSG_DEFAULT_TYPE;

  if (msgsnd(self->_id, msg, MSG_SIZE, 0) == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }
}

void msgqueue_free(msgqueue_t *self) { UNUSED(self); }

void msgqueue_delete(msgqueue_t *self) {
  if (msgctl(self->_id, IPC_RMID, NULL) == 0)
    return;

  if (errno == EIDRM || errno == EINVAL) {
    return;
  }

  perror("msgctl");
  exit(EXIT_FAILURE);
}

void msg_scan(msg_t *msg) {
  if (fgets(msg->mtext, MSG_SIZE, stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }

  size_t len = strlen(msg->mtext);
  msg->mtext[len - 1] = '\0';
}

char *msg_get_str(msg_t *msg) { return msg->mtext; }
