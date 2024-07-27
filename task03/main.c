#include "contacts.h"
#include "person.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "contacts.data"

bool enter_name(name_t *name);

bool enter_emails(emails_t *emails);

bool enter_profiles(profile_t *profiles, size_t *profiles_size);

int main() {
  contacts_t contacts = contacts_init();
  if (!contacts_read(&contacts, FILE_PATH)) {
    return EXIT_FAILURE;
  }

  while (true) {
    printf("menu options:\n");
    printf("1 - add contact\n");
    printf("2 - edit contact\n");
    printf("3 - remove contact\n");
    printf("4 - find contact\n");
    printf("5 - print all contacts\n");
    printf("6 - exit\n");
    printf("> ");

    int option;
    scanf("%d", &option);

    switch (option) {
    case 1: {
      name_t name;
      emails_t emails;
      profile_t profiles[PROFILES_SIZE];
      size_t profiles_size;

      enter_name(&name);
      enter_emails(&emails);
      enter_profiles(profiles, &profiles_size);

      person_t person = person_init(&name, &emails, profiles, profiles_size);
      contacts_add(&contacts, person);

      continue;
    }
    case 2: {
      name_t name;
      emails_t emails;
      profile_t profiles[PROFILES_SIZE];
      size_t profiles_size;

      enter_name(&name);
      enter_emails(&emails);
      enter_profiles(profiles, &profiles_size);

      person_t *person = contacts_find(&contacts, &name);
      if (person) {
        person_edit(person, &name, &emails, profiles, profiles_size);
      } else {
        printf("person not found\n");
      }

      continue;
    }
    case 3: {
      name_t name;
      enter_name(&name);

      if (contacts_remove(&contacts, &name)) {
        printf("contact removed\n");
      } else {
        printf("contact not found\n");
      }

      continue;
    }
    case 4: {
      name_t name;
      enter_name(&name);

      person_t *person = person = contacts_find(&contacts, &name);
      if (person) {
        person_print(person, 0);
      } else {
        printf("contact not found\n");
      }

      continue;
    }
    case 5: {
      contacts_print(&contacts);
      continue;
    }
    case 6:
      contacts_write(&contacts, FILE_PATH);
      contacts_destroy(&contacts);

      return 0;
    default:
      printf("invalid menu option\n");
      continue;
    }
  }
}

bool enter_name(name_t *name) {
  printf("enter contact firstname: ");
  scanf("%s", name->first_name);

  printf("enter contact lastname: ");
  scanf("%s", name->last_name);

  return true;
}

bool enter_emails(emails_t *emails) {
  printf("enter home email: ");
  scanf("%s", emails->home);

  printf("enter work email: ");
  scanf("%s", emails->work);

  return true;
}

bool enter_profiles(profile_t *profiles, size_t *profiles_size) {
  printf("enter profiles amount: ");
  scanf("%lu", profiles_size);

  if (*profiles_size > PROFILES_SIZE) {
    return false;
  }

  printf("enter profiles:\n");

  for (size_t i = 0; i < *profiles_size; i++) {
    printf("enter profile address: ");
    scanf("%s", profiles[i].address);

    printf("enter profile nickname: ");
    scanf("%s", profiles[i].nickname);
  }

  return true;
}
