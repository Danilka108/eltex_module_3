#include "shared_mem.h"
#include "../common.h"
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>

void shared_mem_open(shared_mem_t *self, key_t key, size_t size, mode_t mode) {
  self->size = size;

  self->id = shmget(key, size, IPC_CREAT | mode);
  if (self->id == -1)
    errExit("shmget");

  self->p = shmat(self->id, NULL, 0);
  if (self->p == (void *)-1)
    errExit("shmat");
}

void *shared_mem_get(shared_mem_t *self) {
  return self->p;
}

void shared_mem_unlink(shared_mem_t *self) {
  if (shmctl(self->id, IPC_RMID, NULL) == -1)
    errExit("shmctl");
}

void shared_mem_close(shared_mem_t *self) {
  if (shmdt(self->p) == -1)
    errExit("shmdt");
}
