#ifndef SHELL_ALIAS_TABLE_H
#define SHELL_ALIAS_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct keyvalue;

struct alias_table;

struct keyvalue {
	char* key;
	char* value;
	int used;
};

struct alias_table{
	
	int tablesize;
	
	struct keyvalue kvtable[];
	
};
	
extern struct alias_table *init_table(int);

extern int insert_table(struct alias_table *, char *, char *);

extern char *delete_from_table(struct alias_table *, char *);

extern void print_table(struct alias_table *);

extern char *value_given_key(struct alias_table *, char *);

extern int index_given_key(struct alias_table *, char *);

extern void clear_table(struct alias_table *);

extern void reset_used(struct alias_table *);
#endif