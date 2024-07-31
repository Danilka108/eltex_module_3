#include <semaphore.h>

#ifndef TASK_11_RWLOCK_H
#define TASK_11_RWLOCK_H

typedef struct {
  size_t rdcount;
  sem_t read_sem, write_sem, rdcount_sem;
} rwlock_t;

void rwlock_init(rwlock_t *self, int pshared);

void rwlock_read_lock(rwlock_t *self);

void rwlock_read_unlock(rwlock_t *self);

void rwlock_write_lock(rwlock_t *self);

void rwlock_write_unlock(rwlock_t *self);

void rwlock_destroy(rwlock_t *self);

#endif
