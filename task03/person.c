#include "person.h"

person_t person_init(name_t *name, emails_t *emails, profile_t *profiles,
                     size_t profiles_size) {
  person_t person;
  person_edit(&person, name, emails, profiles, profiles_size);

  return person;
}

int person_compare(person_t *self, person_t *other) {
  return person_compare_by_name(self, &other->name);
}

int person_compare_by_name(person_t *self, name_t *name) {
  int first_name_cmp = strcmp(self->name.first_name, name->first_name);
  int last_name_cmp = strcmp(self->name.last_name, name->last_name);

  return first_name_cmp == 0 ? last_name_cmp : first_name_cmp;
}

void print_tabs(size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    printf("\t");
  }
}

void person_print(person_t *person, size_t additional_tabs_amount) {
  print_tabs(additional_tabs_amount);
  printf("{\n");

  print_tabs(additional_tabs_amount);
  printf("\tfirstName = \"%s\",\n", person->name.first_name);

  print_tabs(additional_tabs_amount);
  printf("\tlastName = \"%s\",\n", person->name.last_name);

  print_tabs(additional_tabs_amount);
  printf("\tworkEmail = \"%s\",\n", person->emails.work);

  print_tabs(additional_tabs_amount);
  printf("\thomeEmail = \"%s\",\n", person->emails.home);

  print_tabs(additional_tabs_amount);
  printf("\tprofiles = [\n");

  for (int j = 0; j < person->profiles_size; j++) {
    print_tabs(additional_tabs_amount);
    printf("\t\t{\n");

    print_tabs(additional_tabs_amount);
    printf("\t\t\taddress = \"%s\",\n", person->profiles[j].address);

    print_tabs(additional_tabs_amount);
    printf("\t\t\tnickname = \"%s\",\n", person->profiles[j].nickname);

    print_tabs(additional_tabs_amount);
    printf("\t\t},\n");
  }

  print_tabs(additional_tabs_amount);
  printf("\t],\n");

  print_tabs(additional_tabs_amount);
  printf("},\n");
}

void person_edit(person_t *self, name_t *name, emails_t *emails,
                 profile_t *profiles, size_t profiles_size) {
  strcpy(self->name.first_name, name->first_name);
  strcpy(self->name.last_name, name->last_name);

  strcpy(self->emails.home, emails->home);
  strcpy(self->emails.work, emails->work);

  memmove(self->profiles, profiles, profiles_size * sizeof(profile_t));
  self->profiles_size = profiles_size;
}
