#include "conn.h"
#include "../common.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define CONCAT2(a, b) a##b
#define CONCAT3(a, b, c) a##b##c

double calc_result(struct conn_inner *conn);
void handle_conn_inner(struct conn_inner *conn);

#define MSG(MSG, DEST, SRC, IN, OUT)                                           \
  do {                                                                         \
    if (conn->state == CONCAT3(CONN_STATE_, IN, _MSG)) {                       \
      msg_t msg;                                                               \
      msg.kind = CONCAT2(MSG_, MSG);                                           \
      msg.data = calc_result(conn);                                            \
      ssize_t bytes = write(conn->fd, &msg, sizeof(msg));                      \
                                                                               \
      if (bytes == -1 && errno == EAGAIN) {                                    \
        return;                                                                \
      }                                                                        \
                                                                               \
      if (bytes == -1) {                                                       \
      }                                                                        \
                                                                               \
      if (bytes == 0) {                                                        \
        conn->state = CONN_STATE_END_MSG;                                      \
        return;                                                                \
      }                                                                        \
                                                                               \
      conn->state = CONCAT3(CONN_STATE_, IN, _REPLY);                          \
    }                                                                          \
                                                                               \
    if (conn->state == CONCAT3(CONN_STATE_, IN, _REPLY)) {                     \
      reply_t reply;                                                           \
      ssize_t bytes;                                                           \
                                                                               \
      bytes = read(conn->fd, &reply, sizeof(reply));                           \
                                                                               \
      if (bytes == -1 && errno == EAGAIN) {                                    \
        return;                                                                \
      }                                                                        \
                                                                               \
      if (bytes == -1) {                                                       \
      }                                                                        \
                                                                               \
      if (bytes == 0) {                                                        \
        conn->state = CONN_STATE_END_MSG;                                      \
        return;                                                                \
      }                                                                        \
                                                                               \
      if (reply.recv_msg != CONCAT2(MSG_, MSG)) {                              \
        conn->state = CONN_STATE_ERROR;                                        \
        conn->err_msg = MSG_ERR_INVALID_REPLY;                                 \
      } else {                                                                 \
        conn->DEST = reply.data.SRC;                                           \
        conn->state = CONCAT3(CONN_STATE_, OUT, _MSG);                         \
      }                                                                        \
    }                                                                          \
  } while (0)

void init_conn(struct epoll_event *ev, int epoll_fd, int conn_fd,
               struct sockaddr_in conn_addr) {
  printf("connected %s:%d\n", inet_ntoa(conn_addr.sin_addr),
         ntohs(conn_addr.sin_port));

  struct conn_inner *conn = malloc(sizeof(struct conn_inner));
  conn->state = CONN_STATE_LHS_ARG_MSG;
  conn->fd = conn_fd;
  conn->cont = 1;
  conn->addr = conn_addr;

  ev->events = EPOLLIN | EPOLLET | EPOLLOUT;
  ev->data.ptr = conn;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, ev) == -1)
    errExit("epoll_ctl: conn_fd");
}

// try to use setjmp.h instead of state machine
void handle_conn(struct epoll_event *ev) {
  struct conn_inner *conn = ev->data.ptr;

  if (ev->events & EPOLLHUP || ev->events & EPOLLERR) {
    printf("disconnected %s:%d\n", inet_ntoa(conn->addr.sin_addr),
           ntohs(conn->addr.sin_port));
    close(conn->fd);
    free(conn);
    return;
  }

  handle_conn_inner(conn);

  if (conn->state == CONN_STATE_END_MSG) {
    printf("disconnected %s:%d\n", inet_ntoa(conn->addr.sin_addr),
           ntohs(conn->addr.sin_port));
    close(conn->fd);
    free(conn);
  }
}

void handle_conn_inner(struct conn_inner *conn) {
  while (1) {
    MSG(REQ_ARG, lhs, arg, LHS_ARG, RHS_ARG);

    MSG(REQ_ARG, rhs, arg, RHS_ARG, OP);

    MSG(REQ_OP, op, op, OP, RES);

    MSG(RET_RES, cont, cont, RES, END);

    if (conn->state == CONN_STATE_ERROR) {
      ssize_t bytes;
      msg_t msg;
      msg.kind = MSG_ERR_INVALID_REPLY;

      bytes = write(conn->fd, &conn->err_msg, sizeof(conn->err_msg));

      if (bytes == -1 && errno == EAGAIN) {
        return;
      }

      if (bytes == -1) {
        errExit("write");
      }

      conn->state = CONN_STATE_END_MSG;
    }

    if (CONN_STATE_END_MSG) {
      if (conn->cont) {
        conn->state = CONN_STATE_LHS_ARG_MSG;
        continue;
      }
    }

    return;
  }
}

double calc_result(struct conn_inner *conn) {
  if (conn->state != CONN_STATE_RES_MSG)
    return NAN;

  switch (conn->op) {
  case OP_SUM:
    return conn->lhs + conn->rhs;
  case OP_SUB:
    return conn->lhs - conn->rhs;
  case OP_DIV:
    return conn->lhs / conn->rhs;
  case OP_MUL:
    return conn->lhs * conn->rhs;
  default:
    return NAN;
  }
}
