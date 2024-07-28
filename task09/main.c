#include "rwlock.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void writer(int i, rwlock_t *rwlock);
void reader(int i, rwlock_t *rwlock);

int main() {
  size_t wcount = 3;
  size_t rcount = 12;

  rwlock_t rwlock = rwlock_init(IPC_PRIVATE, 0666, 6);

  /*int fd = open("numbers.bin", O_CREAT | O_RDWR, 0666);*/
  /*if (fd == -1) {*/
  /*  perror("open");*/
  /*  exit(EXIT_FAILURE);*/
  /*}*/

  rwlock_write_lock(&rwlock);

  for (size_t i = 0; i < wcount; ++i) {
    if (fork() == 0) {
      writer(i, &rwlock);
      return EXIT_SUCCESS;
    }
  }

  for (size_t i = 0; i < rcount; ++i) {
    if (fork() == 0) {
      reader(i, &rwlock);
      return EXIT_SUCCESS;
    }
  }

  rwlock_write_unlock(&rwlock);

  return 0;
}

void writer(int i, rwlock_t *rwlock) {
  int fd = open("numbers.bin", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  while (1) {
    rwlock_write_lock(rwlock);

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
    rwlock_write_unlock(rwlock);

    sleep(4);
  }
}

void reader(int i, rwlock_t *rwlock) {
  int fd = open("numbers.bin", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  while (1) {
    rwlock_read_lock(rwlock);

    if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("lseek");
      exit(EXIT_FAILURE);
    }

    int num = 0;
    if (read(fd, &num, sizeof(int)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    usleep(1000 * (rand() % 999));

    printf("file reader %d: %d\n", i, num);
    rwlock_read_unlock(rwlock);

    sleep(2);
  }
}
