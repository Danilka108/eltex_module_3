#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../common.h"
#include "addr.h"
#include "shared.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s server_ip server_port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  parse_addr(argv[1], argv[2], &server_addr);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
    errExit("socket");

  if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    close(fd);
    errExit("connect");
  }

  int cont = 1;
  while (cont) {
    msg_t msg;
    reply_t reply;
    memset(&reply, 0, sizeof reply);

    if (read(fd, &msg, sizeof(msg)) == -1) {
      close(fd);
      errExit("read");
    }

    reply.recv_msg = msg.kind;
    switch (msg.kind) {
    case MSG_REQ_ARG: {
      printf("arg: ");
      scanf("%lf", &reply.data.arg);
      break;
    }
    case MSG_REQ_OP: {
      int op = 0;
      printf("op (1 - sum, 2 - sub, 3 - div, 4 - mul): ");
      scanf("%d", &op);

      if (op < 1 || op > 4) {
        close(fd);
        fprintf(stderr, "invalid operator\n");
        exit(EXIT_FAILURE);
      }

      reply.data.op = op;
      break;
    }
    case MSG_RET_RES: {
      printf("result: %lf\n", msg.data);
      printf("continue (1 - yes, 0 - no): ");
      scanf("%d", &reply.data.cont);
      cont = reply.data.cont;
      break;
    }
    case MSG_ERR_INVALID_REPLY:
      fprintf(stderr, "internal error\n");
      exit(EXIT_FAILURE);
    }

    if (write(fd, &reply, sizeof(reply)) == -1) {
      close(fd);
      errExit("write");
    }
  }

  close(fd);
  return 0;
}
