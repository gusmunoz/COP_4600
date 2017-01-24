%{
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <pwd.h>
#include "y.tab.h"

char *expand_null_tilde();
void find_string_match();
int single_character_match(char *, char *);
int find_char_match();
%}
%%
setenv 				{return SETENV;}
printenv 			{return PRINTENV;}
unsetenv 			{return UNSETENV;}
cd 				{return CD;}
alias 				{return ALIAS;}
unalias				{return UNALIAS;}
bye|exit|quit 			{return EXIT;}
"${"			{return ENV_START;}
"}"				{return ENV_END;}
\|				{return PIPE;}
\>				{return GT;}
\>\>			{return GTGT;}
\<				{return LT;}
2\>				{return GTERR;}
"2>&1"			{return GTERRIN;}
"&"				{return AMPER;}
"~"				{yylval = strdup(expand_null_tilde()); return WORD;}
\"[^"\n]*\"				{yytext[yyleng-1] = 0; yylval = strdup(yytext+1); return QUOTED_STRING;}
[a-zA-Z\-\.0-9_/:]+ 		{yylval = strdup(yytext); return WORD;}
[a-zA-Z\-\.0-9_/:~][a-zA-Z\-\.0-9_/:~]+	{expand_word_tilde_n(); return WORD;}
[a-zA-Z\-\.0-9/_]*\?[a-zA-Z\-\.0-9/_]*	{find_char_match(); return WORD;}
[a-zA-Z\-\.0-9/_]*\*[a-zA-Z\-\.0-9/_]* {find_string_match(); return WORD;}
\n 				{return NEWLINE;}
. /* ignore all other cases*/
<<EOF>> {return EXIT;}
%%

void find_string_match() {
	char *text = strdup(yytext);
	glob_t glob_res;
	int error = glob(text, 0, NULL, &glob_res);
	if(error == GLOB_ABORTED) {
		printf("\tError: read error\n");
		yylval = strdup(text);
	}
	if(error == GLOB_NOSPACE) {
		printf("\tError: no memory\n");
		yylval = strdup(text);
	}
	if(error == GLOB_NOMATCH) {
		printf("\tError: no matches\n");
		yylval = strdup(text);
	}
	if(error == 0) {
		yylval = strdup(glob_res.gl_pathv[0]);
	}
	globfree(&glob_res);
}

int single_character_match(char *s1, char *s2) {
	int diff = 0;
	int index = 0;
	while(s1[index] != NULL) {
		//printf("\t%c\n", s1[index]);
		if(diff > 1) {
			return diff; // too many mismatch
		}
		if(s2[index] == NULL) {
			return -1; //different length
		}
		if(s1[index] != s2[index]) {
			++diff;
		}
		++index;
	}
	return 0;
}

int find_char_match() {
	char *text = strdup(yytext);
	int index = 0;
	char replace = text[index];
	while(replace != NULL) {
		//printf("\t%c\n", replace);
		if(replace == '?') {
			//printf("\tBREAK\n");
			break;
		}
		++index;
		replace = text[index];
	}
	if(replace == NULL || replace != '?') {
		yylval = strdup(text);
		free(text);
		return 0;
	}
	char *cwd = (char*)get_current_dir_name();
	DIR* currentDir = opendir(cwd);
	struct dirent *folders = readdir(currentDir);
	//printf("\tREADING FILES FROM: %s\n", cwd);
	while(folders != NULL){
		//printf("\t%s\n", folders->d_name);
		if(single_character_match(text, folders->d_name) == 0) {
			yylval = strdup(folders->d_name);
			rewinddir(currentDir);
			return 0;
		}
		folders = readdir(currentDir);
	}
	rewinddir(currentDir);
	printf("\tWildcard error: cannont find match with ? operator\n");
	yylval = strdup(text);
	free(text);
	return 0;
}

char *expand_null_tilde() {
	 //printf("\t111111111111111111111\n");
	char *home = getenv("HOME");
	printf("home: %s\n", home);
	return home;
}

int expand_word_tilde() {
	char *text = strdup(yytext+1);
	struct passwd *user_info = getpwnam(text);
	if(user_info == NULL) {
		char *home = getenv("HOME");
		char path[512];
		strcpy(path, home);
		if(text[0] != '/') {
			strcat(path, "/");
		}
		strcat(path, text);
		yylval = strdup(path);
		return 0;
	}
	yylval = strdup(user_info->pw_dir);
	return 0;
}

int expand_word_tilde_n() {
  //printf("\t22222222222222222222222\n");
  char *text = strdup(yytext);
  struct passwd *user_info = getpwnam(text+1);
  if(user_info != NULL) {
    yylval = strdup(user_info->pw_dir);
    return 0;
  }
  char *home = getenv("HOME");
  int index = 0;
  char path[1024];
  while(text[index] != NULL) {
    if(text[index] != '~') {
      char temp[2];
      temp[0] = text[index];
      temp[1] = '\0';
      strcat(path, temp);
    }
    else {
      if(text[index+1] != NULL) {
	if(text[index+1] != '/') {
	  strcat(path, home);
	  strcat(path, "/");
	}
	else {
	  strcat(path,home);
	}
      }
      else {
	strcat(path, home);
      }
    }
    ++index;
  }
 yylval = strdup(path);
 return 0;
}
