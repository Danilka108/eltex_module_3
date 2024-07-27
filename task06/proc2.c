#include <string.h>

#include "common.h"
#include "interproc.h"

int main() {
  int msgq = init_msgq();

  while (1) {
    struct msgbuf recv_msg = receive_msg(msgq, PROC_1_TYPE);
    if (strcmp(recv_msg.mtext, EXIT_MSG) == 0) {
      break;
    }

    send_msg(msgq, PROC_2_TYPE);
  }

  close_msgq(msgq);

  return 0;
}
