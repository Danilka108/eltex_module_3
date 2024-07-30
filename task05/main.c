#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FILE_PATH "numbers.bin"

volatile int is_file_blocked = 0;

void sighandler(int signum);

void parent_proc(pid_t pid, int pipefd[2]);

void child_proc(int pipefd[2]);

int open_file(int oflag);

void send_block_sig(pid_t pid);

void send_unblock_sig(pid_t pid);

int main() {
  close(open_file(O_WRONLY | O_CREAT | O_TRUNC));

  if (signal(SIGUSR1, sighandler) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGUSR2, sighandler) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("pipe");
    return EXIT_FAILURE;
  }

  pid_t pid;
  if ((pid = fork()) < 0) {
    perror("fork");
    return EXIT_FAILURE;
  }

  if (pid) {
    parent_proc(pid, pipefd);
  } else {
    child_proc(pipefd);
  }

  return 0;
}

void sighandler(int signum) {
  if (signum == SIGUSR1) {
    is_file_blocked = 1;
  }

  if (signum == SIGUSR2) {
    is_file_blocked = 0;
  }
}

void parent_proc(pid_t pid, int pipefd[2]) {
  close(pipefd[1]);

  while (1) {
    send_block_sig(pid);
    sleep(1);

    int fd = open_file(O_WRONLY | O_APPEND);

    srand(time(NULL));
    int rand_value = rand();
    ssize_t write_status = write(fd, &rand_value, sizeof(rand_value));

    printf("parent: write number to file: %d\n", rand_value);

    if (write_status < 0) {
      perror("file write");
      exit(EXIT_FAILURE);
    }
    close(fd);

    send_unblock_sig(pid);
    sleep(1);

    if (read(pipefd[0], &rand_value, sizeof(rand_value)) < 0) {
      perror("pipe read");
      exit(EXIT_FAILURE);
    }

    printf("parent: read number from pipe: %d\n", rand_value);
  }

  close(pipefd[0]);
}

void child_proc(int pipefd[2]) {
  close(pipefd[0]);

  while (1) {
    pause();
    if (is_file_blocked)
      continue;

    int fd = open_file(O_RDONLY);

    int rand_value;
    ssize_t bytes = read(fd, &rand_value, sizeof(rand_value));

    close(fd);

    if (bytes < 0) {
      perror("file read");
      exit(EXIT_FAILURE);
    }

    if (bytes == 0) {
      break;
    }

    printf("child: read number from file: %d\n", rand_value);

    srand(time(NULL));
    rand_value = rand();

    if (write(pipefd[1], &rand_value, sizeof(rand_value)) < 0) {
      perror("pipe write");
      exit(EXIT_FAILURE);
    }

    printf("child: write number to pipe: %d\n", rand_value);
  }

  close(pipefd[1]);
}

int open_file(int oflag) {
  int fd;

  if ((fd = open(FILE_PATH, oflag, S_IWUSR | S_IRUSR)) == -1) {
    perror("file open");
    exit(EXIT_FAILURE);
  }

  return fd;
}

void send_block_sig(pid_t pid) {
  if (kill(pid, SIGUSR1) < 0) {
    perror("kill SIGUSR1");
    exit(EXIT_FAILURE);
  }
}

void send_unblock_sig(pid_t pid) {
  if (kill(pid, SIGUSR2) < 0) {
    perror("kill SIGUSR2");
    exit(EXIT_FAILURE);
  }
}
