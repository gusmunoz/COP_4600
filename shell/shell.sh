lex shell_lex.l; yacc -d shell_yacc.y; cc -o shell alias.c built_in.c command_linked_list.c shell.c y.tab.c lex.yy.c

