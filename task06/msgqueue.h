#include <limits.h>
#include <mqueue.h>

#ifndef MQUEUE_H
#define MQUEUE_H

#define UNUSED(x) (void)(x)

#define MSG_SIZE 256

#define MSG_EXIT_TYPE 31
#define MSG_DEFAULT_TYPE 1

typedef struct {
  long mtype;
  char mtext[MSG_SIZE];
} msg_t;

typedef struct {
  int _id;
} msgqueue_t;

msgqueue_t msgqueue_init(const char *pathname, int proj_id, mode_t mode);

void msgqueue_recv(msgqueue_t *self, msg_t *msg, int *exit_msg);

void msgqueue_send(msgqueue_t *self, msg_t *msg, int exit_msg);

void msgqueue_free(msgqueue_t *self);

void msgqueue_delete(msgqueue_t *self);

void msg_scan(msg_t *msg);

char *msg_get_str(msg_t *msg);

#endif
