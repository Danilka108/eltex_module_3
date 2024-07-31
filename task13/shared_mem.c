#include "shared_mem.h"
#include "../common.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void shared_mem_open(shared_mem_t *self, const char *name, size_t size, mode_t mode) {
  self->size = size;

  if (strlen(name) > PATH_MAX) {
    errExit("name too long");
  }

  strcpy(self->name, name);

  self->fd = shm_open(self->name, O_RDWR | O_CREAT | O_TRUNC, mode);
  if (self->fd == -1)
    errExit("shm_open");

  if (ftruncate(self->fd, size) == -1)
    errExit("ftruncate");

  self->p = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, self->fd, 0);
  if (self->p == MAP_FAILED)
    errExit("mmap");
}

void *shared_mem_get(shared_mem_t *self) { return self->p; }

void shared_mem_unlink(shared_mem_t *self) {
  if (shm_unlink(self->name) == -1)
    errExit("shm_unlink");
}

void shared_mem_close(shared_mem_t *self) {
  if (munmap(self->p, self->size) == -1)
    errExit("munmap");

  if (close(self->fd) == -1)
    errExit("close");
}
