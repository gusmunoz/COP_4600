/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SETENV = 258,
    PRINTENV = 259,
    UNSETENV = 260,
    CD = 261,
    ALIAS = 262,
    UNALIAS = 263,
    EXIT = 264,
    WORD = 265,
    NEWLINE = 266,
    LT = 267,
    GT = 268,
    QUOTED_STRING = 269,
    PIPE = 270,
    ENV_START = 271,
    ENV_END = 272,
    GTGT = 273,
    GTERR = 274,
    GTERRIN = 275,
    AMPER = 276
  };
#endif
/* Tokens.  */
#define SETENV 258
#define PRINTENV 259
#define UNSETENV 260
#define CD 261
#define ALIAS 262
#define UNALIAS 263
#define EXIT 264
#define WORD 265
#define NEWLINE 266
#define LT 267
#define GT 268
#define QUOTED_STRING 269
#define PIPE 270
#define ENV_START 271
#define ENV_END 272
#define GTGT 273
#define GTERR 274
#define GTERRIN 275
#define AMPER 276

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
