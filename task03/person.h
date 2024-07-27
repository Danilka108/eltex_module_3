#include <stdio.h>
#include <string.h>

#ifndef TASK_3_PERSON_H
#define TASK_3_PERSON_H

#define CONTACTS_LIST_SIZE 50
#define PROFILES_SIZE 5
#define FIELD_SIZE 256

typedef char field_t[FIELD_SIZE];

typedef struct {
  field_t home;
  field_t work;
} emails_t;

typedef struct {
  field_t address;
  field_t nickname;
} profile_t;

typedef struct {
  field_t first_name;
  field_t last_name;
} name_t;

typedef struct {
  name_t name;
  emails_t emails;

  profile_t profiles[PROFILES_SIZE];
  size_t profiles_size;
} person_t;

person_t person_init(name_t *name, emails_t *emails,
                     profile_t *profiles, size_t profiles_size);

int person_compare(person_t *self, person_t *other);

int person_compare_by_name(person_t *self, name_t *name);

void person_print(person_t *person, size_t additional_tabs_amount);

void person_edit(person_t *self, name_t *name, emails_t *emails,
                 profile_t *profiles, size_t profiles_size);

#endif
