#include <limits.h>
#include <mqueue.h>

#ifndef MQUEUE_H
#define MQUEUE_H

#define MSG_SIZE 256
#define MSG_EXIT_PRIOR 31
#define MSG_DEFAULT_PRIOR 1

typedef struct {
  char text[MSG_SIZE];
} msg_t;

typedef struct {
  char _name[NAME_MAX];
  mqd_t _desc;
} msgqueue_t;

msgqueue_t msgqueue_init(const char *name, int capacity, mode_t mode);

void msgqueue_recv(msgqueue_t *self, msg_t *msg, int *exit_msg);

void msgqueue_send(msgqueue_t *self, msg_t *msg, int exit_msg);

void msgqueue_free(msgqueue_t *self);

void msgqueue_delete(msgqueue_t *self);

void msg_scan(msg_t *self);

char *msg_get_str(msg_t *self);

#endif
