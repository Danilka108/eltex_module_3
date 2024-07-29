#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int open_file();

void parent(sem_t *sem, int pipefd[2]);

void child(sem_t *sem, int pipefd[2]);

int main() {
  sem_t *sem;
  if ((sem = sem_open("/task10", O_CREAT, 0666, 1)) == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  if (fork() == 0) {
    child(sem, pipefd);
  } else {
    parent(sem, pipefd);
  }

  sem_close(sem);

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

void parent(sem_t *sem, int pipefd[2]) {
  close(pipefd[1]);

  int fd = open_file();

  while (1) {
    if (sem_wait(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    int num = rand();

    if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("lseek");
      exit(EXIT_FAILURE);
    }

    if (write(fd, &num, sizeof(num)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    printf("write number %d\n", num);

    if (sem_post(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }

    sleep(1);

    num = 0;
    if (read(pipefd[0], &num, sizeof(num)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    printf("read from pipe %d\n", num);
  }

  close(pipefd[0]);
}

void child(sem_t *sem, int pipefd[2]) {
  close(pipefd[0]);

  int fd = open_file();

  while (1) {
    srand(time(NULL));
    int num = rand();
    if (write(pipefd[1], &num, sizeof(num)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    printf("write to pipe %d\n", num);

    sleep(1);

    if (sem_wait(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("lseek");
      exit(EXIT_FAILURE);
    }

    num = 0;
    if (read(fd, &num, sizeof(num)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    printf("read number %d\n", num);

    if (sem_post(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
  }

  close(pipefd[1]);
}
