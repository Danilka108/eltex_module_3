#include <netinet/ip.h>
#include <stdint.h>

#ifndef TASK_14_SOCK_H
#define TASK_14_SOCK_H

#define MSG_SIZE 4096

typedef char msg_t[MSG_SIZE];

typedef struct {
  int fd;
  struct sockaddr_in laddr, raddr;
} sock_t;

void sock_init(sock_t *self, struct sockaddr_in *laddr,
               struct sockaddr_in *raddr);

void sock_recv(sock_t *self);

int sock_send(sock_t *self);

void sock_close(sock_t *self);

#endif
