#ifndef SHELL_CMD_LINKED_LIST_H
#define SHELL_CMD_LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct linearg;

struct command_linked_list;

struct linearg{
	int id;
	char *word;
	struct linearg *next;
};

struct command_linked_list{

	struct linearg *head;
	struct linearg *tail;
	int listsize;

};

extern struct command_linked_list *init_list();

extern int insert_list(struct command_linked_list *, int, char *);

extern struct linearg *pop_list(struct command_linked_list *);

extern int clear_list(struct command_linked_list *);

extern void print_list(struct command_linked_list *);

#endif