#include "shared.h"
#include <netinet/in.h>
#include <sys/epoll.h>

#ifndef TASK_18_CONN_H
#define TASK_18_CONN_H

enum CONN_STATE {
  CONN_STATE_LHS_ARG_MSG,
  CONN_STATE_LHS_ARG_REPLY,
  CONN_STATE_RHS_ARG_MSG,
  CONN_STATE_RHS_ARG_REPLY,
  CONN_STATE_OP_MSG,
  CONN_STATE_OP_REPLY,
  CONN_STATE_RES_MSG,
  CONN_STATE_RES_REPLY,
  CONN_STATE_END_MSG,
  CONN_STATE_ERROR,
};

struct conn_inner {
  int fd;
  double lhs, rhs;
  op_t op;
  int cont;
  msg_kind_t err_msg;
  enum CONN_STATE state;
  struct sockaddr_in addr;
};

void init_conn(struct epoll_event *ev, int epoll_fd, int conn_fd,
               struct sockaddr_in conn_addr);

void handle_conn(struct epoll_event *ev);

#endif
