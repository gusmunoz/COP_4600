#include "built_in.h"

void change_directory(char *dest) {
	char *path = dest;
	int error = chdir(path);
	char *cwd = (char*)get_current_dir_name();
	if(error == 0){
		printf("\tChanged directory to %s\n", cwd);
	}
	else{
		printf("\tError: can't find %s\n", path);
	}
}

void change_directory_home() {
	int error = chdir((char*)getenv("HOME"));
	if(error == 0){
		printf("\tChanged directory to HOME\n");
	}
	else{
		printf("\tError\n");
	}
}

void alias_store(struct alias_table *table, char *key, char *value) {
	int error = insert_table(table, key, value);
	if(error == -1) {
		printf("\tError: alias table is full\n");
	}
	else{
		printf("\tInserted %s as alias %s in index %d\n", key, value, error);
	}
}
	
void alias_print_one(struct alias_table *table, char *key) {
	char *value = value_given_key(table, key);
	if(value == 0) {
		printf("\t%s does not have an alias\n", key);
	}
	else {
		printf("\tThe alias of %s is %s\n", key, value);
	}
}

void alias_print_all(struct alias_table *table) {
	print_table(table);
}

void unalias_all(struct alias_table *table) {
	clear_table(table);
}

void unalias_one(struct alias_table * table, char *key) {
	char *value = delete_from_table(table, key);
	if(value == 0) {
		printf("\tCannot find alias of %s\n", key);
	}
	else {
		printf("\tUnalias key %s alias %s\n", key, value);
	}
}

int set_environ(const char* name, const char* value, int overwrite){
//overwrite == 0 means to NOT overwrite
	char *newEnv;		//ptr to existing env variable or nonexistent env variable
	
	//arg1: no argument for path
	//arg2: first char of string is null --
	//arg3: '=' not found in input string, return NULL --may remove if input format is different
	//arg4: no respective value given
	if(name == NULL || name[0] == '\0' || strchr(name, '=') != NULL || value == NULL){
		printf("Error");
		return -1;
	}
	if(getenv(name) != NULL && overwrite == 0){
		return 0;
	}

	//remove the current value of 'name'
 	unset_environ(name);  

	//allocate space for name & add 2 more spaces for '=' and NULL terminator
	newEnv = malloc(strlen(name) + strlen(value) + 2);
        
	//pointer to memory location NULL -- error
	if(newEnv == NULL){
		return -1;
	}

	//create a string in format ARGPATH=/bin:/usr/bin:/usr/sbin:/usr/local/bin
	strcpy(newEnv, name);
	strcat(newEnv, "=");
	strcat(newEnv, value);

	//adds value to env variable since the env variable already unset; else return error
	if((putenv(newEnv)) != 0){ // 0 == success for putenv() syscall
		return -1;
	}
	else{
		return 0;
	}
}

// unsetenv variable
int unset_environ(const char* name){
	extern char** environ;		//system environment variables and resp value
	char** temp;			//temp ptr points goes through list of names
	char** string_pos;
	size_t len;
	
	if(name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
		printf("Error");
		return -1;
	}

	len = strlen(name);

	//search for 
	for(temp = environ; *temp != NULL; ){
		//does the name exist in the list? may not need '=' ...check!!
		if(strncmp(*temp, name, len) == 0 && (*temp)[len] == '='){
		//shift pointer to end of string in order to remove entry
			for(string_pos = temp; *string_pos != NULL; string_pos++){
				*string_pos = *(string_pos + 1);
			}
		//loop through all entries with 'name'
		}else{
			temp++;
		}
	}
	return 0;
}

// printenv
void print_environ(){
	extern char** environ;
	int index=0;
	while(environ[index] != NULL){
		printf("%d %s\n", index, environ[index]);
		index++;
	}
}
