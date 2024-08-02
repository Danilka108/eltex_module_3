#ifndef TASK_15_SHARED_H
#define TASK_15_SHARED_H

#define handle_error(str)                                                      \
  do {                                                                         \
    perror(str);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

enum REQ_TYPE {
  GET_ARG = 1,
  GET_OP = 2,
  OP_RESULT = 3,
};

enum OP {
  OP_SUM = 1,
  OP_SUB = 2,
  OP_DIV = 3,
  OP_MUL = 4,
};

struct request {
  enum REQ_TYPE type;
  double data;
};

union response {
  double arg;
  enum OP op;
  int stop;
};

#endif
