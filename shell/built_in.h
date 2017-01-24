#ifndef SHELL_BUILT_IN_H
#define SHELL_BUILT_IN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alias.h"
#include "command_linked_list.h"

extern void change_directory(char *);

extern void change_directory_home();

extern void alias_store(struct alias_table *, char *, char *);

extern void alias_print_one(struct alias_table *, char *);

extern void alias_print_all(struct alias_table *);

extern void unalias_all(struct alias_table *);

extern void unalias_one(struct alias_table *, char *);

extern int set_environ(const char *, const char *, int);

extern int unset_environ(const char *);

extern void print_environ();
#endif