#include "../common.h"
#include "shared_mem.h"
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SERIES_SIZE 16

struct shmbuf {
  sem_t sem;

  struct series {
    int numbers[SERIES_SIZE];
    size_t size;
  } series;

  struct {
    int max, min, calculated;
  } extrema;
};

volatile int stop = 0;

void sighandler(int signum);

void parent(struct shmbuf *buf);

void child(struct shmbuf *buf);

void generate_series(int *i, int *first_iter, struct shmbuf *buf);

void calc_extrema(struct shmbuf *buf);

int main() {
  if (signal(SIGINT, sighandler) == SIG_ERR)
    errExit("signal");

  shared_mem_t shared_mem;
  struct shmbuf *buf;

  shared_mem_open(&shared_mem, "task13", sizeof(struct shmbuf), 0600);
  shared_mem_unlink(&shared_mem);

  buf = shared_mem_get(&shared_mem);
  sem_init(&buf->sem, 1, 1);
  buf->extrema.calculated = 1;
  buf->series.size = 0;

  pid_t pid;
  if ((pid = fork()) == -1)
    errExit("fork");

  if (pid)
    parent(buf);
  else
    child(buf);

  sem_destroy(&buf->sem);
  shared_mem_close(&shared_mem);

  return EXIT_SUCCESS;
}

void sighandler(int signum) {
  if (signum == SIGINT)
    stop = 1;
}

void parent(struct shmbuf *buf) {
  int i = 0;
  int first_iter = 1;

  while (!stop) {
    if (sem_wait(&buf->sem) == -1)
      errExit("sem_wait");

    generate_series(&i, &first_iter, buf);

    if (sem_post(&buf->sem) == -1)
      errExit("sem_post");

    sleep(1);
  }

  printf("\niterations: %d\n", i);
}

void child(struct shmbuf *buf) {
  while (!stop) {
    if (sem_wait(&buf->sem) == -1)
      errExit("sem_wait");

    calc_extrema(buf);

    if (sem_post(&buf->sem) == -1)
      errExit("sem_post");
  }
}

void generate_series(int *i, int *first_iter, struct shmbuf *buf) {
  if (!buf->extrema.calculated)
    return;

  if (*first_iter)
    *first_iter = 0;
  else
    printf("min: %d, max: %d\n", buf->extrema.min, buf->extrema.max);

  srand(*i + 1);

  buf->extrema.calculated = 0;
  buf->series.size = 1 + rand() % (SERIES_SIZE - 1);

  printf("numbers:");
  for (size_t j = 0; j < buf->series.size; ++j)
    printf(" %d", buf->series.numbers[j] = rand());
  printf("\n");

  *i += 1;
}

void calc_extrema(struct shmbuf *buf) {
  if (buf->extrema.calculated)
    return;

  buf->extrema.min = INT_MAX;
  buf->extrema.max = INT_MIN;
  buf->extrema.calculated = 1;

  for (size_t j = 0; j < buf->series.size; ++j) {
    if (buf->series.numbers[j] > buf->extrema.max)
      buf->extrema.max = buf->series.numbers[j];

    if (buf->series.numbers[j] < buf->extrema.min)
      buf->extrema.min = buf->series.numbers[j];
  }
}
