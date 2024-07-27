#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool parse_number(const char *src, int len, int *dest);

int main(int argc, char *argv[]) {
  int sum = 0;

  for (int i = 1; i < argc; ++i) {
    int arg = 0;

    if (parse_number(argv[i], strlen(argv[i]), &arg)) {
      sum += arg;
    } else {
      fprintf(stderr, "invalid arg '%s'\n", argv[i]);
    }
  }

  printf("sum: %d\n", sum);

  return 0;
}

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
