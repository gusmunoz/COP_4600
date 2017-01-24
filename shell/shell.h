#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "y.tab.h"
#include "command_linked_list.h"
#include "built_in.h"
#include "alias.h"
#include "built_in.h"

#define TABLESIZE 32

typedef struct yy_buffer_state * YY_BUFFER_STATE;

// Data structures
struct command_linked_list *cmd_list;
struct alias_table *alias;
struct linearg *input_ptr;

// Flags
int pipe_amount;
int output_redir;
int input_redir;
int output_redir_append;
int output_error;
int error_to_stdout;
int is_built_in;
int running;
int alias_flag;
char *string_value;
int background_run;

/*void reset_global_variables() {
	input_redir = 0;
	output_redir = 0;
	pipe_amount = 0;
	input_ptr = NULL;
}*/

/*
all built in command ids are below 10 ... 
besides exit
id
1	cd
2	alias
3	unalias
4	printenv
5	setenv
6	unsetenv
7	
8
9
10	argument or others
11	ls
999 exit
*/

/* Breaks the string pointed by str into string before pipe and after pipe and stores them in the arg array*/
int getPipes(char *str, char *arg[]);

/* Breaks the string pointed by str into words and stores them in the arg array*/
int getArguments(char *str, char *arg[]);

int process_pipe(char *, int, int, int, int, int, int);

void process_built_in(struct command_linked_list *, struct alias_table *);

void process_command(struct command_linked_list *, struct alias_table *);

void free_variables();

void reset_flags();

void exec_io_redir(char *file, char *args[], int fd_in, int fd_out, int fd_error, int run_in_back);