#include "msgqueue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  msgqueue_t msgqueue = msgqueue_init("proc1.c", 'A', 0666);
  msg_t msg;
  int is_exit_msg;

  while (1) {
    printf("send message: ");
    msg_scan(&msg);
    is_exit_msg = strcmp(msg_get_str(&msg), "exit") == 0;
    msgqueue_send(&msgqueue, &msg, is_exit_msg);
    printf("message sent\n");

    if (is_exit_msg)
      break;

    usleep(100000);

    printf("receive message...\n");
    msgqueue_recv(&msgqueue, &msg, &is_exit_msg);
    printf("received message: %s\n", msg_get_str(&msg));

    if (is_exit_msg)
      break;
  }

  printf("exit\n");
  msgqueue_free(&msgqueue);
  msgqueue_delete(&msgqueue);

  return 0;
}
