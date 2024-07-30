#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "msgqueue.h"

msgqueue_t msgqueue_init(const char *name, int capacity, mode_t mode) {
  msgqueue_t self;

  struct mq_attr attr;
  attr.mq_maxmsg = capacity;
  attr.mq_msgsize = MSG_SIZE;

  self._desc = mq_open(name, O_CREAT | O_RDWR, mode, &attr);
  if (self._desc < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  strcpy(self._name, name);

  return self;
}

void msgqueue_recv(msgqueue_t *self, msg_t *msg, int *exit_msg) {
  unsigned int prior;

  if (mq_receive(self->_desc, (char *)msg->text, sizeof(msg->text), &prior) < 0) {
    perror("mq_receive");
    exit(EXIT_FAILURE);
  }

  *exit_msg = prior == MSG_EXIT_PRIOR;
}

void msgqueue_send(msgqueue_t *self, msg_t *msg, int exit_msg) {
  unsigned int prior = exit_msg ? MSG_EXIT_PRIOR : MSG_DEFAULT_PRIOR;

  if (mq_send(self->_desc, (char *)msg->text, sizeof(msg->text), prior) < 0) {
    perror("mq_send");
    exit(EXIT_FAILURE);
  }
}

void msgqueue_free(msgqueue_t *self) {
  if (mq_close(self->_desc) == -1) {
    perror("mq_close");
    exit(EXIT_FAILURE);
  }
}

void msgqueue_delete(msgqueue_t *self) {
  if (mq_unlink(self->_name) == 0)
    return;

  if (errno == ENOENT) {
    return;
  }

  perror("mq_unlink");
  exit(EXIT_FAILURE);
}

void msg_scan(msg_t *self) {
  if (fgets(self->text, MSG_SIZE, stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }

  size_t len = strlen(self->text);
  self->text[len - 1] = '\0';
}

char *msg_get_str(msg_t *self) { return self->text; }
