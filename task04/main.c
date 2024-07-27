#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main() {
  pid_t pid;

  int fd[2];
  if (pipe(fd) == -1) {
    fprintf(stderr, "failed to open pipe\n");
    return EXIT_FAILURE;
  }

  if ((pid = fork()) < 0) {
    fprintf(stderr, "failed to fork\n");
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  if (pid) {
    close(fd[0]);

    int r;
    while ((r = rand(), write(fd[1], &r, sizeof(r)) > 0))
      sleep(1);

    close(fd[1]);
  } else {
    close(fd[1]);

    int r;
    while (read(fd[0], &r, sizeof(r)) > 0) {
      printf("rand value: %d\n", r);
    }

    close(fd[0]);
  }

  return 0;
}
