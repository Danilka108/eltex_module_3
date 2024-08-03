#include <fcntl.h>
#include <math.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "addr.h"
#include "shared.h"

#define SOCK_QUEUE_SIZE 5

void do_request(int fd, struct request *req, union response *res) {
  ssize_t bytes;

  if (write(fd, req, sizeof(*req)) == -1) {
    close(fd);
    handle_error("write");
  }

  if ((bytes = read(fd, res, sizeof(*res))) == -1) {
    close(fd);
    handle_error("read");
  }

  if (bytes != sizeof(*res)) {
    close(fd);
    fprintf(stderr, "invalid response\n");
    exit(EXIT_FAILURE);
  }
}

void handle_client(int fd) {
  enum OP op;
  struct request req;
  union response res;
  double lhs, rhs;
  char filename[NAME_MAX];

  while (1) {
    memset(&req, 0, sizeof(req));
    memset(&res, 0, sizeof(req));
    req.type = GET_OP;
    do_request(fd, &req, &res);
    op = res.op;

    if (op != OP_FILE) {
      req.type = GET_ARG;
      do_request(fd, &req, &res);
      lhs = res.arg;

      req.type = GET_ARG;
      do_request(fd, &req, &res);
      rhs = res.arg;
    } else {
      req.type = GET_FILENAME;
      do_request(fd, &req, &res);
      memcpy(filename, res.filename, NAME_MAX);
      printf("filename: %s\n", filename);
    }

    req.type = OP_RESULT;
    switch (op) {
    case OP_SUM: {
      req.data = lhs + rhs;
      break;
    }
    case OP_SUB: {
      req.data = lhs - rhs;
      break;
    }
    case OP_DIV: {
      req.data = lhs / rhs;
      break;
    }
    case OP_MUL: {
      req.data = lhs * rhs;
      break;
    }
    case OP_FILE: {
      req.type = START_FILE_SENDING;
      if (write(fd, &req, sizeof(req)) == -1)
        handle_error("write");

      int ffd = open(filename, O_RDONLY);
      if (ffd == -1) {
        close(fd);
        handle_error("open");
      }

      char buff[4 * 1024];
      ssize_t bytes;
      while (1) {
        if ((bytes = read(ffd, buff, sizeof(buff))) == -1) {
          close(fd);
          close(ffd);
          handle_error("read");
        }

        if (bytes == 0)
          break;

        if (write(fd, buff, bytes) == -1) {
          close(fd);
          close(ffd);
          handle_error("write");
        }
      }

      char end[2] = "\r\n";
      if (write(fd, &end, sizeof(end)) == -1)
        handle_error("write");

      if (read(fd, &res, sizeof(res)) == -1) {
        close(fd);
        close(ffd);
        handle_error("read");
      }

      close(ffd);
      if (res.stop)
        break;

      continue;
    }
    }

    do_request(fd, &req, &res);
    if (res.stop)
      break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s listening_ip listening_port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  parse_addr(argv[1], argv[2], &server_addr);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
    handle_error("socket");

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {

    close(server_fd);
    handle_error("bind");
  }

  if (listen(server_fd, SOCK_QUEUE_SIZE) == -1) {
    close(server_fd);
    handle_error("listen");
  }

  printf("listening %s:%d\n", inet_ntoa(server_addr.sin_addr),
         ntohs(server_addr.sin_port));

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
      close(server_fd);
      handle_error("accept");
    }

    printf("accept connection %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    pid_t pid = fork();
    if (pid == -1) {
      close(client_fd);
      close(server_fd);
      handle_error("fork");
    }

    if (pid == 0) {
      handle_client(client_fd);
      printf("refuse connection %s:%d\n", inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));
      close(server_fd);
      exit(EXIT_SUCCESS);
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
