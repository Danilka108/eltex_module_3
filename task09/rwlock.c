#include "rwlock.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

enum {
  READ_SEM = 0,
  WRITE_SEM = 1,
  TURNSTILE_SEM = 2,
  BARRIER_SEM = 3,
};

rwlock_t rwlock_init(key_t key, mode_t mode, ushort rmax) {
  rwlock_t self;
  self.rmax = rmax;

  if ((self.id = semget(key, 4, mode)) == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  ushort values[4];
  values[READ_SEM] = 0;
  values[WRITE_SEM] = 0;
  values[TURNSTILE_SEM] = 1;
  values[BARRIER_SEM] = rmax;

  if (semctl(self.id, 0, SETALL, values) == -1) {
    perror("semctl");
    exit(EXIT_FAILURE);
  }

  return self;
}

void set_op(struct sembuf *buf, int num, int op) {
  buf->sem_num = num;
  buf->sem_op = op;
  buf->sem_flg = 0;
}

void do_ops(rwlock_t *lock, struct sembuf *bufs, int n) {
  if (semop(lock->id, bufs, n) == -1) {
    perror("semop");
    exit(EXIT_FAILURE);
  }
}

void rwlock_read_lock(rwlock_t *self) {
  struct sembuf ops[5] = {
      {BARRIER_SEM, -1, 0},
      {TURNSTILE_SEM, -1, 0},
      {TURNSTILE_SEM, 1, 0},
      {WRITE_SEM, 0, 0},
      {READ_SEM, +1, 0},
  };

  do_ops(self, ops, 5);
}

void rwlock_read_unlock(rwlock_t *self) {
  struct sembuf ops[2] = {
      {BARRIER_SEM, 1, 0},
      {READ_SEM, -1, 0},
  };
  do_ops(self, ops, 2);
}

void rwlock_write_lock(rwlock_t *self) {
  struct sembuf ops[4] = {
      {TURNSTILE_SEM, -1, 0},
      {READ_SEM, 0, 0},
      {WRITE_SEM, 0, 0},
      {WRITE_SEM, +1, 0},
  };
  do_ops(self, ops, 4);
}

void rwlock_write_unlock(rwlock_t *self) {
  struct sembuf ops[2] = {
      {TURNSTILE_SEM, 1, 0},
      {WRITE_SEM, -1, 0},
  };
  do_ops(self, ops, 2);
}
