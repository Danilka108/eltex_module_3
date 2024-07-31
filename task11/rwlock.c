#include "rwlock.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

void wait_sem(sem_t *sem);
void post_sem(sem_t *sem);

void rwlock_init(rwlock_t *self, int pshared) {
  self->rdcount = 0;

  if (sem_init(&self->read_sem, pshared, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&self->write_sem, pshared, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&self->rdcount_sem, pshared, 1) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }
}

void rwlock_read_lock(rwlock_t *self) {
  wait_sem(&self->read_sem);
  post_sem(&self->read_sem);

  wait_sem(&self->rdcount_sem);

  if (++self->rdcount == 1) {
    wait_sem(&self->write_sem);
  }

  post_sem(&self->rdcount_sem);
}

void rwlock_read_unlock(rwlock_t *self) {
  wait_sem(&self->rdcount_sem);

  if (--self->rdcount == 0) {
    post_sem(&self->write_sem);
  }

  post_sem(&self->rdcount_sem);
}

void rwlock_write_lock(rwlock_t *self) {
  wait_sem(&self->read_sem);
  wait_sem(&self->write_sem);
}

void rwlock_write_unlock(rwlock_t *self) {
  post_sem(&self->read_sem);
  post_sem(&self->write_sem);
}

void rwlock_destroy(rwlock_t *self) {
  sem_destroy(&self->read_sem);
  sem_destroy(&self->write_sem);
  sem_destroy(&self->rdcount_sem);
}

void wait_sem(sem_t *sem) {
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
}

void post_sem(sem_t *sem) {
  if (sem_post(sem) == -1) {
    perror("sem_post");
    exit(EXIT_FAILURE);
  }
}
