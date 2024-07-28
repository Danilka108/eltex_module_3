#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

#define FILE_PATH "numbers.bin"

typedef struct {
  int semid;
  struct sembuf inc, dec;
} task_08_mutex_t;

int open_file(int oflag);

task_08_mutex_t mutex_init();
void mutex_lock(task_08_mutex_t *self);
void mutex_unlock(task_08_mutex_t *self);
void mutex_destroy(task_08_mutex_t *self);

void parent(task_08_mutex_t *mutex, int pipefd[2], int fd);
void child(task_08_mutex_t *mutex, int pipefd[2], int fd);

int main() {
  task_08_mutex_t mutex = mutex_init();

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  int fd = open_file(O_RDWR);

  pid_t pid;
  if ((pid = fork()) == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid) {
    parent(&mutex, pipefd, fd);
  } else {
    child(&mutex, pipefd, fd);
  }

  close(fd);
  mutex_destroy(&mutex);

  return EXIT_SUCCESS;
}

int open_file(int oflag) {
  int fd;

  if ((fd = open(FILE_PATH, oflag | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) ==
      -1) {
    perror("file open");
    exit(EXIT_FAILURE);
  }

  return fd;
}

void parent(task_08_mutex_t *mutex, int pipefd[2], int fd) {
  close(pipefd[1]);

  int r;

  srand(time(NULL));
  while (1) {
    mutex_lock(mutex);
    {
      r = rand();

      if (write(fd, &r, sizeof(r)) == -1) {
        perror("file write");
        exit(EXIT_FAILURE);
      }

      printf("write to file: %d\n", r);
    }
    mutex_unlock(mutex);

    {
      if (read(pipefd[0], &r, sizeof(r)) < 0) {
        perror("pipe read");
        exit(EXIT_FAILURE);
      }

      printf("read from pipe: %d\n", r);
    }
  }

  close(pipefd[0]);
}

void child(task_08_mutex_t *mutex, int pipefd[2], int fd) {
  close(pipefd[0]);

  size_t i = 0;
  int r;

  srand(time(NULL));
  while (1) {
    {
      r = rand();

      if (write(pipefd[1], &r, sizeof(r)) < 0) {
        perror("pipe write");
        exit(EXIT_FAILURE);
      }

      printf("write to pipe: %d\n", r);
    }

    mutex_lock(mutex);
    {
      lseek(fd, (i++) * sizeof(r), SEEK_SET);

      if ((read(fd, &r, sizeof(r))) == -1) {
        perror("file read");
        exit(EXIT_FAILURE);
      }

      printf("read from file: %d\n", r);
      sleep(1);
    }
    mutex_unlock(mutex);
  }

  close(pipefd[1]);
}

task_08_mutex_t mutex_init() {
  union semun {
    int val;
    struct semid_ds *buf;
    ushort array[1];
  } sem_attr;

  task_08_mutex_t self;
  self.inc.sem_flg = self.dec.sem_flg = 0;

  self.inc.sem_num = 0;
  self.dec.sem_num = 0;

  self.inc.sem_op = 1;
  self.dec.sem_op = -1;

  self.semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  if (self.semid == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  sem_attr.val = 1;
  if (semctl(self.semid, 0, SETVAL, sem_attr) == -1) {
    perror("semctl");
    exit(EXIT_FAILURE);
  }

  return self;
}

void mutex_lock(task_08_mutex_t *self) {
  if (semop(self->semid, &self->dec, 1) == -1) {
    perror("semop decrement");
    exit(EXIT_FAILURE);
  }
}

void mutex_unlock(task_08_mutex_t *self) {
  if (semop(self->semid, &self->inc, 1) == -1) {
    perror("semop increment");
    exit(EXIT_FAILURE);
  }
}

void mutex_destroy(task_08_mutex_t *self) {
  if (semctl(self->semid, 0, IPC_RMID) == -1) {
    perror("semctl");
    exit(EXIT_FAILURE);
  }
}
