#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "addr.h"
#include "shared.h"

#define SOCK_QUEUE_SIZE 5

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s server_ip server_port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  parse_addr(argv[1], argv[2], &server_addr);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
    handle_error("socket");

  if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    close(fd);
    handle_error("connect");
  }

  while (1) {
    struct request req;
    union response res;
    int stop = 0;

    if (read(fd, &req, sizeof(req)) == -1)
    {
      close(fd);
      handle_error("read");
    }

    switch (req.type) {
    case GET_ARG: {
      printf("arg: ");
      scanf("%lf", &res.arg);
      break;
    }
    case GET_OP: {
      int op = 0;
      printf("op (1 - sum, 2 - sub, 3 - div, 4 - mul): ");
      scanf("%d", &op);

      if (op < 1 || op > 4) {
        close(fd);
        fprintf(stderr, "invalid operator\n");
        exit(EXIT_FAILURE);
      }

      res.op = op;
      break;
    }
    case OP_RESULT: {
      printf("result: %lf\n", req.data);
      printf("quit (1 - yes, 0 - no): ");
      scanf("%d", &res.stop);
      stop = res.stop;
      break;
    }
    }

    if (write(fd, &res, sizeof(res)) == -1)
    {
      close(fd);
      handle_error("write");
    }

    if (stop)
      break;
  }

  close(fd);
  return 0;
}
