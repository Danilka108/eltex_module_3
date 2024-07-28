#ifndef TASK_09_RWLOCK_H
#define TASK_09_RWLOCK_H

#include <sys/types.h>

typedef struct {
  int id;
  ushort rmax;
} rwlock_t;

rwlock_t rwlock_init(key_t key, mode_t mode, ushort maxrd);

void rwlock_read_lock(rwlock_t *self);

void rwlock_read_unlock(rwlock_t *self);

void rwlock_write_lock(rwlock_t *self);

void rwlock_write_unlock(rwlock_t *self);

#endif
