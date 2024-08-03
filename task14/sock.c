#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../common.h"
#include "sock.h"

void sock_init(sock_t *self, struct sockaddr_in *laddr,
               struct sockaddr_in *raddr) {
  memcpy(&self->laddr, laddr, sizeof(*laddr));
  memcpy(&self->raddr, raddr, sizeof(*raddr));

  if ((self->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    errExit("socket");
  }

  if (bind(self->fd, (struct sockaddr *)&self->laddr, sizeof(self->laddr)) ==
      -1) {
    sock_close(self);
    errExit("bind");
  }
}

void sock_recv(sock_t *self) {
  struct sockaddr_in raddr;
  socklen_t raddr_len = sizeof(raddr);

  msg_t buf;
  printf("waiting message...\n");
  ssize_t buf_size = recvfrom(self->fd, &buf, sizeof(buf), 0,
                              (struct sockaddr *)&raddr, &raddr_len);

  if (buf_size < 0) {
    sock_close(self);
    errExit("recvfrom");
  }

  printf("%s:%d> %s\n", inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port), buf);
}

int sock_send(sock_t *self) {
  msg_t buf;

  printf("%s:%d> ", inet_ntoa(self->laddr.sin_addr),
         ntohs(self->laddr.sin_port));

  memset(buf, 0, sizeof(buf));
  if (fgets(buf, MSG_SIZE, stdin) == NULL)
  {
    sock_close(self);
    errExit("fgets");
  }

  buf[strlen(buf) - 1] = '\0';

  int is_exit_msg = strcmp(buf, "exit") == 0;
  ssize_t bytes = sendto(self->fd, &buf, sizeof(buf), 0,
                         (struct sockaddr *)&self->raddr, sizeof(self->raddr));
  if (bytes < 0) {
    sock_close(self);
    errExit("sendto");
  }

  printf("%ld bytes sent\n", bytes);
  return is_exit_msg ? -1 : 0;
}

void sock_close(sock_t *self) {
  close(self->fd);
}
