#include <stdint.h>
#include <arpa/inet.h>

#ifndef TASK_14_ADDR_H
#define TASK_14_ADDR_H

int parse_port(const char *src, uint16_t *dest);

void parse_addr(char *ip, char *port, struct sockaddr_in *addr);

#endif
