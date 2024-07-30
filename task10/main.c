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

int open_file(int oflags);

int main() {
  close(open_file(O_CREAT | O_TRUNC));

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

int open_file(int oflags) {
  int fd = open("numbers.bin", oflags, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  return fd;
}

void parent(sem_t *sem, int pipefd[2]) {
  close(pipefd[1]);

  while (1) {
    if (sem_wait(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }

    int fd = open_file(O_WRONLY);
    srand(time(NULL));
    int num = rand();

    if (write(fd, &num, sizeof(num)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    close(fd);

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

    int fd = open_file(O_RDONLY);
    num = 0;
    if (read(fd, &num, sizeof(num)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    close(fd);

    printf("read number %d\n", num);

    if (sem_post(sem) == -1) {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
  }

  close(pipefd[1]);
}
