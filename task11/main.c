#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "../common.h"
#include "rwlock.h"
#include "shared_mem.h"

#define FILE_NAME "numbers.bin"

struct shmbuf {
  rwlock_t rwlock;
};

void writer(int pipefd[2], int i, shared_mem_t *shared_mem);
void reader(int pipefd[2], int i, shared_mem_t *shared_mem);
int open_file(int oflag);

int main() {
  close(open_file(O_WRONLY | O_CREAT | O_TRUNC));

  int pipefd[2];
  if (pipe(pipefd) == -1)
    errExit("pipe");

  size_t wrcount = 3, rdcount = 12;
  shared_mem_t shared_mem;
  shared_mem_init(&shared_mem, "/task11", sizeof(struct shmbuf));
  shared_mem_unlink(&shared_mem);

  struct shmbuf *buf = shared_mem_get(&shared_mem);
  rwlock_init(&buf->rwlock, 1);

  rwlock_write_lock(&buf->rwlock);

  for (size_t i = 0; i < wrcount; ++i) {
    if (fork() != 0)
      continue;

    if (i != 0) {
      close(pipefd[0]);
      close(pipefd[1]);
    }

    writer(pipefd, i, &shared_mem);
    exit(EXIT_SUCCESS);
  }

  for (size_t i = 0; i < rdcount; ++i) {
    if (fork() != 0)
      continue;

    if (i != 0) {
      close(pipefd[0]);
      close(pipefd[1]);
    }

    reader(pipefd, i, &shared_mem);
    exit(EXIT_SUCCESS);
  }

  close(pipefd[0]);
  close(pipefd[1]);

  rwlock_write_unlock(&buf->rwlock);

  shared_mem_close(&shared_mem);
}

void writer(int pipefd[2], int i, shared_mem_t *shared_mem) {
  struct shmbuf *buf = shared_mem_get(shared_mem);

  if (i == 0)
    close(pipefd[1]);

  while (1) {
    rwlock_write_lock(&buf->rwlock);
    {
      srand(time(NULL) ^ i);
      int r = rand();
      int fd = open_file(O_WRONLY);
      if (write(fd, &r, sizeof(r)) == -1) {
        errExit("write");
      }
      printf("writer %d: %d\n", i, r);
      close(fd);
    }
    rwlock_write_unlock(&buf->rwlock);

    if (i == 0) {
      int r = 0;
      if (read(pipefd[0], &r, sizeof(r)) == -1)
        errExit("read");

      printf("pipe reader: %d\n", r);
    }

    sleep(4);
  }

  if (i == 0)
    close(pipefd[0]);

  shared_mem_close(shared_mem);
}

void reader(int pipefd[2], int i, shared_mem_t *shared_mem) {
  struct shmbuf *buf = shared_mem_get(shared_mem);

  if (i == 0)
    close(pipefd[0]);

  while (1) {
    rwlock_read_lock(&buf->rwlock);
    {
      int fd = open_file(O_RDONLY);
      srand(time(NULL) ^ i);

      int r = 0;
      ssize_t bytes;
      if ((bytes = read(fd, &r, sizeof(r))) == -1) {
        errExit("read");
      }

      if (bytes == 0) {
        printf("reader %d: NULL\n", i);
      } else {
        printf("reader %d: %d\n", i, r);
      }
      close(fd);
    }
    rwlock_read_unlock(&buf->rwlock);

    if (i == 0) {
      srand(time(NULL));
      int r = rand();
      if (write(pipefd[1], &r, sizeof(r)) == -1)
        errExit("write");

      printf("pipe writer: %d\n", r);
    }

    sleep(2);
  }

  if (i == 0)
    close(pipefd[1]);

  shared_mem_close(shared_mem);
}

int open_file(int oflag) {
  int fd = open(FILE_NAME, oflag, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  return fd;
}
