#include "shell.h"

/*
void lookInside() {
	char *cwd = (char*)get_current_dir_name();
	DIR* currentDir = opendir(cwd);
	struct dirent *folders = readdir(currentDir);
	while(folders != NULL){
		printf("\t%s", folders->d_name);
		folders = readdir(currentDir);
	}
	rewinddir(currentDir);
	printf("\n");

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


void process_built_in(struct command_linked_list *list, struct alias_table *table) {
	if(list->head == NULL) {
		return;
	}
	if(pipe_amount > 0) {
		printf("\tSyntax error: cannot use pipe with built in commands\n");
		return;
	}
	if(input_redir > 0) {
		printf("\tSyntax error: cannot use input redirection with built in commands\n");
		return;
	}
	if(output_redir > 0) {
		printf("\tSyntax error: cannot use output redirection with built in commands\n");
		return;
	}
	struct linearg *current = list->head;
	struct linearg *prev = NULL;
	/* Check tail for & and fork and execute in background*/
	if(current->id == 999) {
		running = 0;
		printf("\tExiting ... Bye bye\n");
		return;
	}
	/* Check command: built in or executable or others*/
	switch(current->id) {
		case 1: /* cd*/
			if(current->next != NULL) {
				//printf("\tChanged directory to %s\n", current->next->word);
				change_directory(current->next->word);
			}
			else {
				//printf("\tChanged directory to $HOME\n");
				change_directory_home();
			}
			break;
		case 2: /* alias*/
			if(current->next != NULL) {
				if(current->next->next != NULL) {
					printf("\t%s is now an alias of %s\n", current->next->word, current->next->next->word);
					insert_table(table, current->next->word, current->next->next->word);
				}
				else {
					char *value = value_given_key(table, (char *)current->next->word);
					if(value == NULL) {
						printf("\tThere is no alias for %s\n", current->next->word);
					}
					else {
						printf("\tThe alias of %s is %s\n", current->next->word, value);
					}
				}
			}
			else{
				printf("\tAlias table:\n");
				print_table(table);
			}
			break;
		case 3: /* unalias*/
			if(current->next != NULL) {
				char *value = delete_from_table(table, (char *)current->next->word);
				if(value == 0) {
					printf("\tThere is no alias for %s\n", current->next->word);
				}
				else {
					printf("\tUnalias %s : %s\n", current->next->word, value);
				}
			}
			else{
				printf("\tUnalias all\n");
				clear_table(table);
			}
			break;	
		case 4: /* printenv*/
				printf("\tEnvironemnt variables:\n");
				print_environ();
			break;
		case 5: /* setenv*/
			if(current->next != NULL) {
				if(current->next->next != NULL) {
					printf("\tSetted the environment variable %s as %s\n", current->next->word, current->next->next->word);
					set_environ(current->next->word, current->next->next->word, 1);
				}
				else {
					printf("\tSyntax error: setenv variable value, value not included\n");
				}
			}
			else {
				printf("\tSyntax error: setenv variable value, variable and value not included\n");
			}
			break;
		case 6: /* unsetenv*/
			if(current->next != NULL) {
				printf("\tUnset %s\n", current->next->word);
				unset_environ(current->next->word);
			}
			else{
				printf("\tSyntax error: unsetenv variable\n");
			}
			break;
		default :
			;
	}
	/* For each command check for number of arguments throw error if not enough arguments*/
	/* Execute command with argument and check if there is a pipe if there is store output*/
	/* For pipes check online may have to use fork and pipe system calls*/
	/* Check for redirection, execute input redirection first, output redirection last*/
}

void process_command(struct command_linked_list *list, struct alias_table *table) {
	if(list->head == 0) {
		return;
	}
	if(input_redir > 1) {
		printf("\tSyntax error: multiple input redirections not allowed\n");
		return;
	}
	if(output_redir > 1) {
		printf("\tSyntax error: Maximum of 1 output redirection (stdout)\n");
		return;
	}
	if(output_error > 1) {
		printf("\tSyntax error: Maximum of 1 error redirection (stderr)\n");
		return;
	}
	if(input_ptr != NULL) {
		if(input_ptr->next == NULL) {
			printf("\tSyntax error: no file for input redirection\n");
			return;
		}
	}
	
	char command_line_string[2048] = "";
	
	while(process_alias(list->head, list, table) == 0) {
	}
	reset_used(table);
	//printf("\tExecuting %s\n", list->head->word);
	strcat(command_line_string, list->head->word);
	
	struct linearg *current = list->head->next;
	struct linearg *prev = list->head;
	while(current != NULL) {
		if(current->id == 100 || current->id == 101 || current->id == 103) {
			if(current->id == 100) {
				prev = current->next;
				current = current->next->next;
			}
			break;
		}
		if(current->id == 102) {
			if(current->next == NULL) {
				printf("\tSyntax error: no command after pipe\n");
				return;
			}
			strcat(command_line_string, " ");
			strcat(command_line_string, "|");
			prev = current;
			current = current->next;
			//printf("\n");
			while(process_alias(current, list, table) == 0) {
			}
			reset_used(table);
			//printf("\tExecuting %s\n", current->word);
			strcat(command_line_string, " ");
			strcat(command_line_string, current->word);
		}
		else{
			//printf("\t %s\n", current->word);
			strcat(command_line_string, " ");
			strcat(command_line_string, current->word);
		}
		prev = current;
		current = current->next;
	}
	
	int fd_in = -1;
	int fd_out = -1;
	int fd_error = -1;
	
	if(input_redir == 1) {
		fd_in = open(input_ptr->next->word, O_RDONLY, S_IRUSR | S_IWUSR);
		if(fd_in == -1) {
			printf("\tFile error: < cannot open file\n");
			return;
		}
		//printf("\twith inputs from file %s\n", input_ptr->next->word);
	}
	
	if(output_redir > 0) {
		if(current != NULL) {
			if(current->id == 101) {
				if(current->next != NULL) {
					fd_out = open(current->next->word, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
					if(fd_out == -1) {
						close(fd_out);
						printf("\tFile error: > cannot open or create file\n");
						return;
					}
					//printf("\toutputing to file %s\n", current->next->word);
				}
				else{
					printf("\n");
					printf("\tSyntax error: no file to output to\n");
				}
			}
		}
	}
	else if(output_redir_append > 0) {
		if(current != NULL) {
			if(current->id == 103) {
				if(current->next != NULL) {
					fd_out = open(current->next->word, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
					if(fd_out == -1) {
						close(fd_out);
						printf("\tFile error: >> cannot open or create file\n");
						return;
					}
					//printf("\tappending output to file %s\n", current->next->word);
				}
				else{
					printf("\n");
					printf("\tSyntax error: no file to append output to\n");
				}
			}
		}
	}
	
	else if(output_error > 0) {
		if(current != NULL) {
			if(current->id == 104) {
				if(current->next != NULL) {
					fd_error = open(current->next->word, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
					if(fd_error == -1) {
						close(fd_error);
						printf("\tFile error: >> cannot open or create file\n");
						return;
					}
					//printf("\tappending output to file %s\n", current->next->word);
				}
				else{
					printf("\n");
					printf("\tSyntax error: no file to append output to\n");
				}
			}
			else if(current->id == 105) {
				fd_error = 1;
			}
		}
	}
	
	if(pipe_amount > 0) {
		process_pipe(command_line_string, pipe_amount, list->listsize, fd_in, fd_out, fd_error, background_run);
		
	  
	}
	else {
		printf("\tExecuting command...\n");
		current = list->head;
		char *file = current->word;
		char *args[list->listsize];
		int index = 0;
		while(current != NULL) {
			if(current->id == 100 || current->id == 101 || current->id == 103) {
				break;
			}
			args[index] = current->word;
			++index;
			current = current->next;
		}
		args[index] = NULL;
		exec_io_redir(file, args, fd_in, fd_out, fd_error, background_run);
	}
}

void init_variables() {
	cmd_list  = (struct command_linked_list *) init_list();
	alias = (struct alias_table *) init_table(32);
	input_ptr = 0;
	
	is_built_in = 0;
	pipe_amount = 0;
	output_redir = 0;
	output_redir_append = 0;
	input_redir = 0;
	alias_flag = 0;
	output_error = 0;
	error_to_stdout = 0;
	background_run = 0;
	
}

void free_variables() {
	clear_list(cmd_list);
	free(cmd_list);
	free(alias);
}

void reset_flags() {
	input_ptr = 0;
	
	pipe_amount = 0;
	output_redir = 0;
	output_redir_append = 0;
	input_redir = 0;
	is_built_in = 0;
	alias_flag = 0;
	output_error = 0;
	error_to_stdout = 0;
	background_run = 0;
	
}

void exec_io_redir(char *file, char *args[], int fd_in, int fd_out, int fd_error, int run_in_back) {
	pid_t pid;
	if( (pid = fork()) == 0) {
		char *text_in = NULL;
		int fd;
		int fd_error;
		
		if( fd_out > -1 ) {
			dup2(fd_out, 1);
			close(fd);
		}
		
		if( fd_error > -1 ) {
			dup2(fd_error, 2);
			close(fd_error);
		}
		
		if(fd_in > -1 ) {
			dup2(fd_in, 0);
			close(fd_in);
		}
		
		execvp(file, args);
		printf("\tError: %s\n", strerror(errno));
		exit(1);
	}
	else if(pid == -1) {
		printf("\tError: fork error\n");
	}
	else {
		if(run_in_back == 0) {
			int status;
			wait(&status);
		}
	}
}

int process_pipe(char *commandLine, int number_of_pipes, int max_number_args, int fd_in, int fd_out, int fd_error, int runInBack) {	//max_number_args == size of cmd LL
	char* pipes[number_of_pipes+1]; //array len number_of_pipes of ptr to char
	char* temp[max_number_args];

	int numArgs = 0;
	
	/* Breaks the command line into arguments and stores the arguments in arguments array */
	int numComArgs = getPipes(commandLine,pipes);//numComArgs is the individual strings between every pipe
	
	
	char ***arguments; //ptr to array of ptrs each of which is itself a ptr to array of ptrs to char arrays/ptrs (the actual commands)
	arguments = malloc(numComArgs * sizeof(char **)); // each element in the 1st level is a ptr to array of ptrs which each point to actual cmds
	
	int i, j;
	for(i = 0; i < numComArgs; i++){	
		// pipe string is passed to getArguments, tokenized, then individual words fill temp. temp will then be copied into the arguments array      	
		numArgs = getArguments(pipes[i],temp);
		
		arguments[i] = malloc(numArgs * sizeof(char *)); //each element in the second level pts to a command/arg
		
		for(j = 0; j < numArgs; j++){	
			arguments[i][j] = malloc(strlen(temp[j]) * sizeof(char)); // element in the 3rd level is an actual command/array of chars
			//actual copying of words into argument array happens here
			strcpy(arguments[i][j], temp[j]);
		}
	}

	//conditional run only if a pipe is used in the command line
	int numPipes = numComArgs-1; //number of pipes is 1 less than number of strings: ls | less | grep y.tab.c (2 pipes)
	
	if(numPipes > 0){
		//file descriptor array to be used with sys call pipes -- use array of pipes
		int fd[numPipes][2];
		for(i=0; i< numPipes; i++){
			if(pipe(fd[i]) == -1){ 
				perror("pipe error"); 
				return(1);
			}
		}

		//printf("numComArgs: %d\n", numComArgs);
		int argcounter = 0;
		while(argcounter < numComArgs){
			
			//first child, which does the writing to the opened pipe, then becomes odd child
			if(fork() == 0){
				//if input file redirection used
				if(argcounter == 0 && fd_in > -1){
					dup2(fd_in, STDIN_FILENO);
					close(fd_in);
				}
				//if output redirection after last command exists, redirect STD_OUT to file
				if(argcounter == numComArgs-1 && fd_out > -1){
					dup2(fd_out, STDOUT_FILENO);
					close(fd_out);
				}
			  
				//if error flag set, redirect errors to a file rather to STDOUT
				if(fd_error > -1){
					dup2(fd_error, 2);
					close(fd_error);
				}
				
				//if 1 pipe has already been processed then reroute the output 3rd child process
				if(argcounter > 0){
					//std_in is rerouted to read end of the pipe
					dup2(fd[argcounter-1][0], STDIN_FILENO); 
				}
				
				//if this is the first pipe OR if an argument exists after it, then reroute output to the next argument
				if(argcounter <= numComArgs - 2){
					//std_out is rerouted to pipe write end
					dup2(fd[argcounter][1], STDOUT_FILENO);
				}
				
				//both ends of pipe are now closed. read end because it was never meant to be used and write end because it's
				//re-aliased to std_out
				for(j=0; j<numPipes;j++)
					for(i=0; i<2; i++)
						close(fd[j][i]);
				
				//command is executed using first row of arguments array
				execvp(arguments[argcounter][0], arguments[argcounter]);
				perror("execvp child 1 failed");
				return(1);         
			}

			//second child, which does the reading
			argcounter++;
			
			//bail if child 0 ate the last command
			if(argcounter >= numComArgs) break;
			
			if(fork() == 0){
						
				//std_in is rerouted to read end of the pipe
				dup2(fd[argcounter-1][0], STDIN_FILENO);			

				//if output redirection after last command exsts, redirect STD_OUT
				if(argcounter == numComArgs-1 && fd_out > -1){
					dup2(fd_out, STDOUT_FILENO);
					close(fd_out);
				}

				//if error flag set, redirect errors to a file rather to STDOUT
				if(fd_error > -1){
					dup2(fd_error, 2);
					close(fd_error);
				}
				
				//if a cmd exists after this process then reroute the output of this child
				if(argcounter <= numComArgs - 2){
					//std_out rerouted to 2nd pipe write end
					dup2(fd[argcounter][1], STDOUT_FILENO);
				}
						
				//both ends of pipe are now closed. write end because it was never going to be used and read end 
				//because it's re-aliased to std_in	
				for(j=0; j<numPipes;j++)
					for(i=0; i<2; i++)
						close(fd[j][i]);
					
				//command is executed using second row of arguments array
				execvp(arguments[argcounter][0], arguments[argcounter]);
				perror("execvp child 2 failed");
				return(1);
			}
			argcounter++;
		}
		//parent process closes pipes and waits for childs to finish executing
		for(i = 0; i < numComArgs+1; i++){
			close(fd[i][0]);
			close(fd[i][1]);
		}
		if(runInBack == 0){
			int status;
			for(i = 0; i < numComArgs; i++){
				wait(&status);
			}	  
		}
	}
}

/* Breaks the string pointed by str into string before pipe and after pipe and stores them in the arg array*/
int getPipes(char *str, char *arg[])
{
	char delimeter[] = "|";
	char *temp = NULL;
	int i=0;

	temp = strtok(str,delimeter); //return ptr to last token in string
	
	while (temp != NULL)
	{
		arg[i++] = temp;                   //array "pipes" holds ptr to | delimeter
		temp = strtok(NULL,delimeter);
	}
	arg[i] = NULL;   
	return i;
	
}

/* Breaks the string pointed by str into words and stores them in the arg array*/
int getArguments(char *str, char *arg[])
{
	char delimeter[] = " ";
	char *temp = NULL;
	int i=0;
	temp = strtok(str,delimeter);
	while (temp != NULL)
	{
	      arg[i++] = temp;                   
	      temp = strtok(NULL,delimeter);
	}
	arg[i] = NULL;     
	return i;
}

int process_alias(struct linearg *current, struct command_linked_list *list, struct alias_table *table) {
	if(current == NULL) {
		return -1;
	}
	char *value = value_given_key(table, current->word);
	if(value == NULL) {
		return -1;
	}
	
	struct linearg *save_tail = list->tail;
	
	char *string_to_scan = (char *) malloc(512*sizeof(char));
	strcpy(string_to_scan, value);
	strcat(string_to_scan, "\n\0");
	
	printf("\tString to scan: %s\n", string_to_scan);
	
	YY_BUFFER_STATE tempbuf = yy_scan_string(string_to_scan);
	yyparse();
	yy_flush_buffer(tempbuf);
	yylex_destroy();
	free(string_to_scan);

	current->word = save_tail->next->word;
	current->id = save_tail->next->id;
	struct linearg *temp = save_tail->next;
	save_tail->next = save_tail->next->next;
	free(temp);
	
	if(save_tail->next != NULL) {
		if(current->next == save_tail->next) {
			return 0;
		}
		list->tail->next = current->next;
		current->next = save_tail->next;
		list->tail = save_tail;
		list->tail->next = NULL;
		return 0;
	}
	else {
		list->tail = save_tail;
		return 0;
	}
}

int shell() {
	init_variables();
	running = 1;
// 	insert_table(alias, "kk", "cd xinu-shell\n");
	signal(SIGINT, SIG_IGN);
	while(running == 1) {

		char *curr_dir = (char*)get_current_dir_name();
		printf("Shell %s: ", curr_dir);
		yyparse();		
		//printf("\tCheck alias\n");
		if(is_built_in != 1) {
			while( process_alias(cmd_list->head, cmd_list, alias) == 0 ) {
				
			}
			reset_used(alias);
		}
		//printf("\tCheck built in or commands\n");
		if(is_built_in == 1) {
			process_built_in(cmd_list, alias);
		}
		else {
			process_command(cmd_list, alias);
		}

		//print_list(cmd_list);
		clear_list(cmd_list);
		reset_used(alias);
		reset_flags();

	}
	free_variables();
	return 0;
}

int main() {
	shell();
	exit(0);
	return 0;
}
