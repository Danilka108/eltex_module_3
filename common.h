#include <stdio.h>
#include <stdlib.h>

#ifndef COMMON_H
#define COMMON_H

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#endif