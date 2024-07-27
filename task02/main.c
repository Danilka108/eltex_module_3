#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  setbuf(stdout, NULL);
  printf("Hello!\n");

  while (1) {
    printf("$ ");

    char cmd[255];

    int argc = 0;
    char *argv[25];
    argv[0] = cmd;

    int i = 0;
    int new_arg = 0;
    while (1) {
      char sym = getchar();

      if (sym == '\n' || sym == '\0' || sym == EOF || i == 254) {
        cmd[i++] = '\0';
        break;
      }

      if (sym == ' ') {
        cmd[i++] = '\0';
        new_arg = 1;
        continue;
      }

      if (new_arg) {
        argv[++argc] = cmd + i;
        new_arg = 0;
      }

      cmd[i] = sym;
      ++i;
    }

    argv[++argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
      fprintf(stderr, "failed to run command '%s'", argv[0]);
      continue;
    }

    if (pid) {
      waitpid(pid, NULL, 0);
    } else {
      execv(argv[0], argv);
      fprintf(stderr, "command '%s' not found\n", argv[0]);
    }
  }

  return 0;
}
