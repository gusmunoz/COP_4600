%{
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "shell.h"

extern struct command_linked_list *cmd_list;
extern struct linearg *input_ptr;
extern int pipe_amount;
extern int output_redir;
extern int input_redir;
extern int output_redir_append;
extern int is_built_in;
extern int output_error;
extern int error_to_stdin;
extern int background_run;

void yyerror(const char *str){
	fprintf(stderr, "\terror: %s\n", str);
}

int yywrap(){
	return 1;
}

%}

%token SETENV PRINTENV UNSETENV CD ALIAS UNALIAS EXIT WORD NEWLINE LT GT QUOTED_STRING PIPE ENV_START ENV_END GTGT GTERR GTERRIN AMPER

%%
commandline: command_syntax NEWLINE{ /* Command line with executable or command, arguments, pipes, and redirection*/
			YYACCEPT;
		}
		| built_in NEWLINE{ /* Built in without arguments*/
			YYACCEPT;
		}
		| built_in others NEWLINE{ /* Built in with arguments*/
			YYACCEPT;
		}
		| built_in others quoted_string NEWLINE{ /* Built in with arguments and string*/
			YYACCEPT;
		}
		| built_in quoted_string NEWLINE{ /* Built in with string*/
			YYACCEPT;
		}
		| built_in environ_variable NEWLINE{ /* Built in with environment variables*/
			YYACCEPT;
		}
		| environ_variable NEWLINE{
			YYACCEPT;
		}
		| errors NEWLINE{
			YYACCEPT;
		}
		| NEWLINE{
			YYACCEPT;
		}
		| command_syntax AMPER NEWLINE{ /* Command line with executable or command, arguments, pipes, and redirection*/
			background_run = 1;
			YYACCEPT;
		}
		| built_in AMPER NEWLINE{ /* Built in without arguments*/
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		| built_in others AMPER NEWLINE{ /* Built in with arguments*/
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		| built_in others quoted_string AMPER NEWLINE{ /* Built in with arguments and string*/
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		| built_in quoted_string AMPER NEWLINE{ /* Built in with string*/
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		| built_in environ_variable AMPER NEWLINE{ /* Built in with environment variables*/
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		| environ_variable AMPER NEWLINE{
			printf("\tWarning: & ignored\n");
			YYACCEPT;
		}
		;
			
command_syntax: others component_pipe redirection_io{

		}
		| others quoted_string component_pipe redirection_io{
		
		}
		| others environ_variable component_pipe redirection_io{
		
		}
		| others environ_variable others component_pipe redirection_io{
		
		}
		| others quoted_string others component_pipe redirection_io{ 
		
		}
		| others others component_pipe redirection_input others redirection_io{
		
		};

component_pipe: /* No pipe*/ 
		| pipe{ /* Pipe the first command*/
			
		}
		| component_pipe others{ /* Pipe the last command*/
		
		}
		| component_pipe others pipe{ /* Pipe the middle command or commands*/
		
		};

built_in: CD {
			is_built_in = 1;
			insert_list(cmd_list, 1, "cd");
		}
		| SETENV {
			is_built_in = 1;
			insert_list(cmd_list, 5, "setenv");
		}
		| PRINTENV{
			is_built_in = 1;
			insert_list(cmd_list, 4, "printenv");
		}
		| UNSETENV{
			is_built_in = 1;
			insert_list(cmd_list, 6, "unsetenv");
		}
		| ALIAS{
			is_built_in = 1;
			insert_list(cmd_list, 2, "alias");
		}
		| UNALIAS{
			is_built_in = 1;
			insert_list(cmd_list, 3, "unalias");
		}
		| EXIT{
			is_built_in = 1;
			insert_list(cmd_list, 999, "exit");
		};

others: WORD{
			char *word = (char *)$1;
			insert_list(cmd_list, 10, word);
		}
		| others WORD{
			char *word = (char *)$2;
			insert_list(cmd_list, 10, word);
		};
		
redirection_io: /* Empty*/
		| redirection_input others{
		
		}
		| redirection_input others redirection_output others{
		
		}
		| redirection_output others{
		
		}
		| redirection_output others redirection_output others{
		
		}
		| redirection_input others redirection_error others{
		
		}
		| redirection_input others redirection_output others redirection_error others{
		
		}
		| redirection_output others redirection_error others{
		
		}
		| redirection_error others{
		
		}
		| redirection_error{
		
		}
		| redirection_input others redirection_error{
		
		}
		| redirection_input others redirection_output others redirection_error{
		
		}
		| redirection_output others redirection_error{
		
		};
 
redirection_input: LT{
			insert_list(cmd_list, 100, "<");
			++input_redir;
			input_ptr = cmd_list->tail;
		};
		
redirection_output: GT{
			insert_list(cmd_list, 101, ">");
			++output_redir;
		}
		| GTGT {
			insert_list(cmd_list, 103, ">>");
			++output_redir_append;
		};
		
redirection_error: GTERR{
			insert_list(cmd_list, 104, "2>");
			++output_error;
		}
		| GTERRIN{
			insert_list(cmd_list, 105, "2>&1");
			++output_error;
			++error_to_stdout;
		};

pipe: 	PIPE{
			insert_list(cmd_list, 102, "|");
			++pipe_amount;
		};

environ_variable: ENV_START WORD ENV_END {
			extern char **environ;
			const char *env_variable = $2;
			char *value = getenv(env_variable);
			if(value != 0) {
				insert_list(cmd_list, 10, value);
				$$ = value;
			}
			else {
				printf("\tError: cannot find environment variable\n");
			}
		};

quoted_string: QUOTED_STRING {
			char *string_literal = $1;
			insert_list(cmd_list, 10, string_literal);
		};
		
errors: built_in others redirection_io{
			printf("\tError: cannot use IO redirection with built in commands\n");
		}
		| built_in redirection_io{
			printf("\tError: cannot use IO redirection with built in commands\n");	
		};
%%
