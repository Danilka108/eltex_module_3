#include <stddef.h>
#include <sys/types.h>

#ifndef TASK_12_SHARED_MEM_H
#define TASK_12_SHARED_MEM_H

typedef struct {
  int id;
  size_t size;
  void *p;
} shared_mem_t;

void shared_mem_open(shared_mem_t *self, key_t key, size_t size, mode_t mode);

void *shared_mem_get(shared_mem_t *self);

void shared_mem_unlink(shared_mem_t *self);

void shared_mem_close(shared_mem_t *self);

#endif
