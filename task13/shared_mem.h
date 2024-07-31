#include <limits.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/types.h>

#ifndef TASK_13_SHARED_MEM_H
#define TASK_13_SHARED_MEM_H

typedef struct {
  int fd;
  void *p;
  size_t size;
  char name[PATH_MAX];
} shared_mem_t;

void shared_mem_open(shared_mem_t *self, const char *name, size_t size, mode_t mode);

void *shared_mem_get(shared_mem_t *self);

void shared_mem_unlink(shared_mem_t *self);

void shared_mem_close(shared_mem_t *self);

#endif
