LEX = lex
YACC = yacc -d

CC = cc

shell: y.tab.o lex.yy.o shell.o alias.o built_in.o command_linked_list.o 
		$(CC) -o shell shell.o alias.o built_in.o command_linked_list.o y.tab.o lex.yy.o -ll -lm

shell.o: shell.c shell.h

alias.o: alias.c alias.h

built_in.o: built_in.c built_in.h

command_linked_list.o: command_linked_list.c command_linked_list.h

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: shell.h

y.tab.c y.tab.h: shell_yacc.y
		$(YACC) -v shell_yacc.y
		
lex.yy.c: shell_lex.l
		$(LEX) shell_lex.l
		
clean:
	-rm -f *.o lex.yy.c *.tab.* shell *.output
