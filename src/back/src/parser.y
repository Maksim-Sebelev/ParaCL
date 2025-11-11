%{
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>

#include "paraCL.hpp"
#include "lexer.hpp"

extern FILE* yyin;
extern int yylex();
extern int yyparse();
extern int get_current_line();

void yyerror(const char* s) {
    std::cerr << "Error at line " << get_current_line() << ": " << s << std::endl;
}

#ifdef DEBUG
    #define DEBUG_COUT_M(message)                \
        do {                                     \
            std::cout << (message) << std::endl; \
        } while(0)
#else
    #define DEBUG_COUT_M(message)                \
        do { } while(0)
#endif

ParaCL::Parser::ProgramAST program;

%}

%union {
    int num_value;
    std::string* str_value;
    ParaCL::Parser::Stmt* stmt;
    ParaCL::Parser::Expr* expr;
    std::vector<ParaCL::Parser::Stmt*>* stmt_vector;
}

%token <num_value> NUM
%token <str_value> VAR
%token ADD SUB MUL DIV
%token ISAB ISABE ISLS ISLSE ISEQ
%token LCIB RCIB LCUB RCUB
%token WH IN AS PRINT
%token SC

%type <stmt_vector> program statements
%type <stmt> statement assignment print_statement while_statement input_statement
%type <expr> expression simple_expression term factor assignment_expression

%%

program:
    statements {
        for (auto stmt : *$1) {
            program.statements.push_back(std::unique_ptr<ParaCL::Parser::Stmt>(stmt));
        }
        delete $1;
        DEBUG_COUT_M("AST construction completed!"); 
    }
    ;

statements:
    { 
        $$ = new std::vector<ParaCL::Parser::Stmt*>(); 
    }
    | statements statement {
        $1->push_back($2);
        $$ = $1;
    }
    ;

statement:
    assignment SC { 
        $$ = $1;
    }
    | print_statement SC { 
        $$ = $1;
    }
    | while_statement {
        $$ = $1;
    }
    | input_statement SC { 
        $$ = $1;
    }
    ;

assignment:
    VAR AS expression {
        $$ = new ParaCL::Parser::AssignStmt(*$1, *$3);
        #ifdef DEBUG
            std::cout << "Created assignment for variable " << *$1 << std::endl;
        #endif
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        $$ = new ParaCL::Parser::PrintStmt(*$2);
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB statements RCUB {
        // Конвертируем вектор указателей в вектор unique_ptr для BlockStmt
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Parser::Stmt>>();
        for (auto stmt : *$6) {
            body_stmts.push_back(std::unique_ptr<ParaCL::Parser::Stmt>(stmt));
        }
        auto body = new ParaCL::Parser::BlockStmt(std::move(body_stmts));
        auto condition = new ParaCL::Parser::Expr(std::move(*$3));
        $$ = new ParaCL::Parser::WhileStmt(std::unique_ptr<ParaCL::Parser::Expr>(condition), 
                                           std::unique_ptr<ParaCL::Parser::BlockStmt>(body));
        #ifdef DEBUG
            std::cout << "Created while statement" << std::endl;
        #endif
        delete $6;
    }
    ;

input_statement:
    IN AS VAR {
        // $$ = new ParaCL::Parser::InputExpr(*$3);
        // delete $3;
    }
    ;

expression:
    simple_expression { $$ = $1; }
    | expression ISAB simple_expression { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ISAB, *$1, *$3);
    }
    | expression ISABE simple_expression { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ISABE, *$1, *$3);
    }
    | expression ISLS simple_expression { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ISLS, *$1, *$3);
    }
    | expression ISLSE simple_expression { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ISLSE, *$1, *$3);
    }
    | expression ISEQ simple_expression { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ISEQ, *$1, *$3);
    }
    ;

simple_expression:
    term { $$ = $1; }
    | simple_expression ADD term { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::ADD, *$1, *$3);
    }
    | simple_expression SUB term { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::SUB, *$1, *$3);
    }
    ;

term:
    factor { $$ = $1; }
    | term MUL factor { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::MUL, *$1, *$3);
    }
    | term DIV factor { 
        $$ = new ParaCL::Parser::BinExpr(ParaCL::token_t::DIV, *$1, *$3);
    }
    ;

factor:
    NUM { 
        $$ = new ParaCL::Parser::NumExpr($1);
    }
    | VAR { 
        $$ = new ParaCL::Parser::VarExpr(*$1);
        delete $1;
    }
    | LCIB expression RCIB { 
        $$ = $2;
    }
    | assignment_expression { 
        $$ = $1;
    }
    | IN {
        $$ = new ParaCL::Parser::InputExpr();
    }
    ;

assignment_expression:
    VAR AS expression {
        $$ = new ParaCL::Parser::AssignExpr(*$1, *$3);
        delete $1;
    }
    ;

%%
