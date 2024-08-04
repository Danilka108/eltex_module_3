#include "../common.h"
#include "addr.h"
#include "conn.h"
#include "shared.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define SOCK_QUEUE_SIZE 5
#define MAX_EVENTS 10

int set_nonblocking(int fd);

void handle_conn(struct epoll_event *ev);

int main(int argc, char *argv[]) {
  struct sockaddr_in laddr;
  struct epoll_event ev;
  int listen_fd, epoll_fd;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s ip port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  parse_addr(argv[1], argv[2], &laddr);

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd == -1)
    errExit("socket");

  if (set_nonblocking(listen_fd) == -1)
    errExit("set_nonblocking");

  if (bind(listen_fd, (struct sockaddr *)&laddr, sizeof(laddr)) == -1)
    errExit("bind");

  if (listen(listen_fd, SOCK_QUEUE_SIZE) == -1)
    errExit("listen");

  printf("listening %s:%d\n", inet_ntoa(laddr.sin_addr), ntohs(laddr.sin_port));

  epoll_fd = epoll_create1(0);
  if (epoll_fd == -1)
    errExit("epoll_create1");

  ev.events = EPOLLIN;
  ev.data.fd = listen_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
    errExit("epoll_ctl: lfd");

  while (1) {
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == listen_fd) {
        struct sockaddr_in conn_addr;
        socklen_t conn_addr_len = sizeof(conn_addr);

        int conn_fd =
            accept(listen_fd, (struct sockaddr *)&conn_addr, &conn_addr_len);
        if (conn_fd == -1)
          errExit("accept");

        if (set_nonblocking(conn_fd) == -1)
          errExit("set_nonblocking");

        printf("connected %s:%d\n", inet_ntoa(conn_addr.sin_addr),
               ntohs(conn_addr.sin_port));

        init_conn(&events[i], epoll_fd, conn_fd);
      } else {
        handle_conn(&events[i]);
      }
    }
  }

  close(listen_fd);
  close(epoll_fd);
  exit(EXIT_SUCCESS);
}

int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    return -1;

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    return -1;

  return 0;
}
