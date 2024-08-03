#include "../common.h"
#include "addr.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s ip port dump_filename\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  parse_addr(argv[1], argv[2], &addr);

  int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == -1)
    errExit("socket");

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    errExit("bind");

  int ffd = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
  if (ffd == -1)
    errExit("open");

  char buf[4 * 1024];
  ssize_t bytes;
  while ((bytes = read(sfd, buf, sizeof(buf))) > 0) {
    if (bytes == -1) errExit("read");
    if (write(ffd, buf, bytes) == -1)
      errExit("write");
  }

  close(ffd);
  close(sfd);
}
