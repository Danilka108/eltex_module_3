#include <stdio.h>
#include <stdlib.h>

#include "addr.h"
#include "sock.h"

int main(int arc, char *argv[]) {
  if (arc != 5) {
    fprintf(stderr,
            "Usage: %s listening_ip listening_port receiver_ip receiver_port\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  sock_t sock;
  struct sockaddr_in laddr;
  struct sockaddr_in raddr;

  parse_addr(argv[1], argv[2], &laddr);
  parse_addr(argv[3], argv[4], &raddr);

  sock_init(&sock, &laddr, &raddr);

  while (1) {
    sock_recv(&sock);

    if (sock_send(&sock) == -1)
      break;
  }

  sock_close(&sock);

  return 0;
}
