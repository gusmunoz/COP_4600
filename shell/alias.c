#include "alias.h"

struct alias_table *init_table(int length) {
	int count = 0;
	struct alias_table *table = malloc( sizeof( struct alias_table) + length * sizeof(struct keyvalue) );
	table->tablesize = length;
	while(count != length){
		table->kvtable[count].key = 0;
		table->kvtable[count].value = 0;
		table->kvtable[count].used = 0;
		++count;
	}
	return table;
}

int insert_table(struct alias_table *table, char *key, char *value) {
	if(table->tablesize == 0) {
		return -1; /* Returns an error if table is not created*/
	}
	int count = 0;
	while(table->kvtable[count].key != 0){
		if(strcmp((table->kvtable[count]).key, key) == 0) {
			break;
		}
		++count;
		if(count == table->tablesize) {
			return -1; /* Returns an error if table is full*/
		}
	}
	table->kvtable[count].key = key;
	table->kvtable[count].value = value;
	table->kvtable[count].used = 0;
	return count;
}

char *delete_from_table(struct alias_table *table, char *key) {
	if(table->tablesize == 0) {
		return 0; /* Returns null if table is not created*/
	}
	char *value = 0;
	int count = 0;
	while(count != table->tablesize){
		if(table->kvtable[count].key != 0){
			if(strcmp(table->kvtable[count].key, key) == 0) {
				value = table->kvtable[count].value;
				table->kvtable[count].key = 0;
				table->kvtable[count].value = 0;
				table->kvtable[count].used = 0;
				return value;
			}
		}
		++count;
	}
	return value;
}

void print_table(struct alias_table *table) {
	if(table->tablesize == 0) {
		return; /* Returns nothing if table is not created*/
	}
	int count = 0;
	while(count != table->tablesize) {
		if(table->kvtable[count].key != 0) {
			printf("\t%s\t%s\n",table->kvtable[count].key, table->kvtable[count].value);
		}
		++count;
	}
	printf("\n");
}

char *value_given_key(struct alias_table *table, char *key){
	if(table->tablesize == 0) {
		return 0; /* Returns null if table is not created*/
	}
	char* value = 0;
	int count = 0;
	while(count != table->tablesize){
		if(table->kvtable[count].key != 0){
			if(strcmp(table->kvtable[count].key, key) == 0) {
				if(table->kvtable[count].used == 1) {
					return 0;
				}
				table->kvtable[count].used = 1;
				return table->kvtable[count].value;
			}
		}
		++count;
	}
	return value; /* Null if key is not found*/
}

int index_given_key(struct alias_table *table, char *key) {
	if(table->tablesize == 0) {
		return -1; /* Returns an error if table is not created*/
	}
	int index = -1;
	int count = 0;
	while(count != table->tablesize) {
		if(strcmp(table->kvtable[count].key, key) == 0) {
			return count;
		}
		++count;
	}
	return index; /* -1 if key is not found*/
}

void clear_table(struct alias_table *table) {
	int count = 0;
	while(count != table->tablesize) {
		table->kvtable[count].key = 0;
		table->kvtable[count].value = 0;
		table->kvtable[count].used = 0;
		++count;
	}
}

void reset_used(struct alias_table *table) {
	int count = 0;
	while(count != table->tablesize) {
		table->kvtable[count].used = 0;
		++count;
	}
}