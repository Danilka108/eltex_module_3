#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

bool parse_number(const char *src, int len, int *dest) {
  if (src == NULL || dest == NULL) {
    return false;
  }

  int value = 0;

  for (int i = len - 1; i >= 0; --i) {
    if (src[i] >= '0' && src[i] <= '9') {
      value += (src[i] - '0') * (int)pow(10, i);
      continue;
    }

    return false;
  }

  *dest = value;
  return true;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "invalid args\n");
    return EXIT_FAILURE;
  }

  int *numbers = malloc(sizeof(int) * (argc - 1));

  for (int i = 1; i < argc; ++i) {
    if (!parse_number(argv[i], strlen(argv[i]), &numbers[i - 1])) {
      fprintf(stderr, "invalid arg\n");
      return EXIT_FAILURE;
    }
  }

  pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "failed to fork process\n");
    return EXIT_FAILURE;
  }

  if (pid) {
    for (int i = 0; i < (argc - 1) / 2; ++i) {
      printf("pid: %d, side: %d, square: %d\n", pid, numbers[i],
             numbers[i] * numbers[i]);
    }
  } else {
    for (int i = (argc - 1) / 2; i < argc - 1; ++i) {
      printf("pid: %d, side: %d, square: %d\n", pid, numbers[i],
             numbers[i] * numbers[i]);
    }
  }

  return 0;
}
