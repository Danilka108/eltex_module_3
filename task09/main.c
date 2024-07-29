#include "rwlock.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void writer(int i, rwlock_t *rwlock, int pipefd[2]);
void write_to_file(int i, int fd);
void read_from_pipe(int pipefd[2]);

void reader(int i, rwlock_t *rwlock, int pipefd[2]);
void read_from_file(int i, int fd);
void write_to_pipe(int pipefd[2]);

int main() {
  size_t wcount = 3;
  size_t rcount = 12;

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  rwlock_t rwlock = rwlock_init(IPC_PRIVATE, 0666, 6);

  rwlock_write_lock(&rwlock);

  for (size_t i = 0; i < wcount; ++i) {
    if (fork() != 0) {
      continue;
    }

    if (i != 0) {
      close(pipefd[0]);
      close(pipefd[1]);
    }

    writer(i, &rwlock, pipefd);
    return EXIT_SUCCESS;
  }

  for (size_t i = 0; i < rcount; ++i) {
    if (fork() != 0) {
      continue;
    }

    if (i != 0) {
      close(pipefd[0]);
      close(pipefd[1]);
    }

    reader(i, &rwlock, pipefd);
    return EXIT_SUCCESS;
  }

  rwlock_write_unlock(&rwlock);

  return 0;
}

int open_file() {
  int fd = open("numbers.bin", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  return fd;
}

void writer(int i, rwlock_t *rwlock, int pipefd[2]) {
  int fd = open_file();

  if (i == 0) {
    close(pipefd[1]);
  }

  while (1) {
    rwlock_write_lock(rwlock);
    write_to_file(i, fd);
    rwlock_write_unlock(rwlock);

    sleep(4);

    if (i == 0)
      read_from_pipe(pipefd);
  }

  if (i == 0) {
    close(pipefd[0]);
  }
}

void write_to_file(int i, int fd) {
  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL) ^ i);
  int num = rand();

  if (write(fd, &num, sizeof(num)) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  printf("file writer %d: %d\n", i, num);
}

void read_from_pipe(int pipefd[2]) {
  int num = 0;
  if (read(pipefd[0], &num, sizeof(num)) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }

  printf("read from pipe: %d\n", num);
}

void reader(int i, rwlock_t *rwlock, int pipefd[2]) {
  int fd = open_file();

  if (i == 0) {
    close(pipefd[0]);
  }

  while (1) {
    rwlock_read_lock(rwlock);
    read_from_file(i, fd);
    rwlock_read_unlock(rwlock);

    sleep(2);

    if (i == 0)
      write_to_pipe(pipefd);
  }

  if (i == 0) {
    close(pipefd[1]);
  }
}

void read_from_file(int i, int fd) {
  int num = 0;

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    exit(EXIT_FAILURE);
  }

  if (read(fd, &num, sizeof(int)) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  usleep(1000 * (rand() % 999));

  printf("file reader %d: %d\n", i, num);
}

void write_to_pipe(int pipefd[2]) {
  srand(time(NULL));
  int num = rand();
  if (write(pipefd[1], &num, sizeof(int)) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  printf("write to pipe: %d\n", num);
}
