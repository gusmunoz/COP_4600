#include "command_linked_list.h"

struct command_linked_list *init_list() {
	struct command_linked_list *list = (struct command_linked_list *) malloc( sizeof(struct command_linked_list) );
	list->head = NULL;
	list->tail = NULL;
	list->listsize = 0;
	return list;
}

int insert_list(struct command_linked_list *list, int id, char *word) {
	struct linearg *new_word = (struct linearg *) malloc( sizeof(struct linearg) );
	if(new_word == 0) {
		return -1; /* Returns error if not enough memory to allocate a new linearg*/
	}
	new_word->id = id;
	new_word->word = word;
	new_word->next = NULL;
	++(list->listsize);
	if(list->head == NULL){
		list->head = new_word;
	}
	else {
		list->tail->next = new_word;
	}
	list->tail = new_word;
	return 0;
}

struct linearg *pop_list(struct command_linked_list *list){
	if(list->listsize == 0){
		return NULL;
	}
	struct linearg *temp = list->head;
	list->head = list->head->next;
	--(list->listsize);
	return temp;
}

int clear_list(struct command_linked_list *list) {
	if(list->head == 0) {
		return 1; /* Returns status to tell list is empty*/
	}
	while( list->head != NULL ) {
		free(pop_list(list));
	}
	list->tail = NULL;
	return 0;
}

void print_list(struct command_linked_list *list) {
	struct linearg *current = list->head;
	while( current != NULL ) {
		printf("\t%d\t%s\n", current->id, current->word);
		current = current->next;
	}
	printf("\n");
}