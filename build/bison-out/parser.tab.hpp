/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_ANANASIK_PROJECTS_C_VLADOS_5_PARACL_1_VERSION_BUILD_BISON_OUT_PARSER_TAB_HPP_INCLUDED
# define YY_YY_HOME_ANANASIK_PROJECTS_C_VLADOS_5_PARACL_1_VERSION_BUILD_BISON_OUT_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    OR = 258,                      /* OR  */
    AND = 259,                     /* AND  */
    ISEQ = 260,                    /* ISEQ  */
    ISNE = 261,                    /* ISNE  */
    ISAB = 262,                    /* ISAB  */
    ISABE = 263,                   /* ISABE  */
    ISLS = 264,                    /* ISLS  */
    ISLSE = 265,                   /* ISLSE  */
    ADD = 266,                     /* ADD  */
    SUB = 267,                     /* SUB  */
    MUL = 268,                     /* MUL  */
    DIV = 269,                     /* DIV  */
    REM = 270,                     /* REM  */
    NEG = 271,                     /* NEG  */
    NOT = 272,                     /* NOT  */
    AS = 273,                      /* AS  */
    ADDASGN = 274,                 /* ADDASGN  */
    SUBASGN = 275,                 /* SUBASGN  */
    MULASGN = 276,                 /* MULASGN  */
    DIVASGN = 277,                 /* DIVASGN  */
    NUM = 278,                     /* NUM  */
    VAR = 279,                     /* VAR  */
    LCIB = 280,                    /* LCIB  */
    RCIB = 281,                    /* RCIB  */
    LCUB = 282,                    /* LCUB  */
    RCUB = 283,                    /* RCUB  */
    WH = 284,                      /* WH  */
    IN = 285,                      /* IN  */
    PRINT = 286,                   /* PRINT  */
    IF = 287,                      /* IF  */
    ELIF = 288,                    /* ELIF  */
    ELSE = 289,                    /* ELSE  */
    SC = 290                       /* SC  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"

    int                                      num_value     ;
    std   ::string                         * str_value     ;
    ParaCL::Stmt                           * stmt          ;
    ParaCL::Expr                           * expr          ;
    ParaCL::BlockStmt                      * block         ;
    ParaCL::ConditionStatement             * condition_stmt;
    ParaCL::IfStatement                    * if_stmt       ;
    std   ::vector<ParaCL::ElifStatement*> * elif_stmts    ;
    ParaCL::ElifStatement                  * elif_stmt     ;
    ParaCL::ElseStatement                  * else_stmt     ;
    std   ::vector<ParaCL::Stmt*>          * stmt_vector   ;

#line 113 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (void);


#endif /* !YY_YY_HOME_ANANASIK_PROJECTS_C_VLADOS_5_PARACL_1_VERSION_BUILD_BISON_OUT_PARSER_TAB_HPP_INCLUDED  */
