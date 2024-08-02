#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "addr.h"

int parse_port(const char *src, uint16_t *dest) {
  char *endptr = NULL;
  unsigned long value = strtoul(src, &endptr, 10);
  if (src == endptr || *endptr != '\0' || errno == ERANGE || value > UINT16_MAX)
    return -1;

  *dest = htons((uint16_t)value);
  return 0;
}

void parse_addr(char *ip, char *port, struct sockaddr_in *addr) {
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;

  if (inet_aton(ip, &addr->sin_addr) == 0)
    errExit("inet_aton");

  if (parse_port(port, &addr->sin_port) == -1) {
    fprintf(stderr, "invalid numeric string\n");
    exit(EXIT_FAILURE);
  }
}
