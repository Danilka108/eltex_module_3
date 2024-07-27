#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

#ifndef TASK_06_COMMON_H
#define TASK_06_COMMON_H

int init_msgq();

struct msgbuf receive_msg(int msgq, long type);

void send_msg(int msgq, long type);

void close_msgq(int msgq);

#endif
