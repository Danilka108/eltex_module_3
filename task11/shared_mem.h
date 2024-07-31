#include <limits.h>
#include <linux/limits.h>
#include <stddef.h>

#ifndef TASK_11_SHARED_MEM_H
#define TASK_11_SHARED_MEM_H

typedef struct {
  int fd;
  void *p;
  size_t size;
  char name[PATH_MAX];
} shared_mem_t;

void shared_mem_init(shared_mem_t *self, const char *name, size_t size);

void *shared_mem_get(shared_mem_t *self);

void shared_mem_destroy(shared_mem_t *self);

#endif
