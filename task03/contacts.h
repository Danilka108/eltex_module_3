#include "person.h"
#include <stdbool.h>
#include <stdlib.h>

#ifndef TASK_3_LIST_H
#define TASK_3_LIST_H

typedef struct contacts_node_t {
    person_t person;

    struct contacts_node_t *prev;
    struct contacts_node_t *next;
} contacts_node_t;

typedef struct {
    contacts_node_t fake_node;
} contacts_t;

contacts_t contacts_init();

bool contacts_write(contacts_t *self, const char *path);

bool contacts_read(contacts_t *self, const char *path);

bool contacts_add(contacts_t *self, person_t person);

bool contacts_remove(contacts_t *self, name_t *name);

person_t *contacts_find(contacts_t *self, name_t *name);

void contacts_print(contacts_t *contacts);

void contacts_destroy(contacts_t *self);

#endif
