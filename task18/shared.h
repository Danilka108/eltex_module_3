#include <limits.h>
#include <linux/limits.h>

#ifndef TASK_18_SHARED_H
#define TASK_18_SHARED_H

typedef enum {
  MSG_REQ_ARG,
  MSG_REQ_OP,
  MSG_RET_RES,
  MSG_ERR_INVALID_REPLY,
} msg_kind_t;

typedef struct {
  msg_kind_t kind;
  double data;
} msg_t;

typedef enum {
  OP_SUM = 1,
  OP_SUB = 2,
  OP_DIV = 3,
  OP_MUL = 4,
} op_t;

typedef union {
  double arg;
  op_t op;
  int cont;
} reply_data_t;

typedef struct {
  msg_kind_t recv_msg;
  reply_data_t data;
} reply_t;

#endif
