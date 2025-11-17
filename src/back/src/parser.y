%{

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include "paraCL_crutch_for_parsery.hpp"
#include "lexer.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

#ifdef DEBUG
    #define DEBUG_COUT_M(message)                \
        do {                                     \
            std::cout << (message) << std::endl; \
        } while(0)
#else
    #define DEBUG_COUT_M(message)                \
        do { } while(0)
#endif

ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void yyerror(YYLTYPE* loc, const char* msg);
int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);

%}

%define api.pure full
%locations

%union {
    int                                num_value  ;
    std   ::string                   * str_value  ;
    std    ::vector<ParaCL::ASTNode*>* ast_nodes  ;
    ParaCL::ASTNode                  * ast_node   ;
    ParaCL::Stmt                     * stmt       ;
    ParaCL::Expr                     * expr       ;
    ParaCL::IfCondition              * if_cond    ;
    ParaCL::ElseCondition            * else_cond  ;
    ParaCL::Condition                * condition  ;
}

%token <num_value> NUM
%token <str_value> VAR
%token AND OR
%token ADDASGN SUBASGN MULASGN DIVASGN
%token ADD SUB MUL DIV
%token ISAB ISABE ISLS ISLSE ISEQ ISNE
%token LCIB RCIB LCUB RCUB
%token WH IN AS PRINT
%token SC
%token <if_cond> IF
%token <else_cond> ELSE

%type <ast_nodes>   program
%type <ast_node>    program_node
%type <stmt>        statement assignment combined_assignment print_statement while_statement input_statement
%type <expr>        expression bool_expression and_or_expression math_comparison_expression add_sub_expression mul_div_expression factor assignment_expression
%type <condition>   condition
%type <if_cond>     if_condition
%type <else_cond>   else_condition

%%

program:
    program_node {
        std::cerr << "add program node\n";
        program.nodes.push_back(std::unique_ptr<ParaCL::ASTNode>($1));
    }
    | program program_node {
        std::cerr << "add program node in helerr rule\n";
        program.nodes.push_back(std::unique_ptr<ParaCL::ASTNode>($2));
    }
    ;

program_node:
    statement {
        std::cerr << "add atatement\n";
        $$ = $1;

    }
    // | condition {
    //     std::cerr << "add condition\n";
    //     $$ = $1;
    // }
    ;

statement:
    assignment SC { 
        $$ = $1;
    }
    | combined_assignment SC {
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
        $$ = new ParaCL::AssignStmt(*$1, std::unique_ptr<ParaCL::Expr>($3));
        ON_DEBUG(std::cout << "Created assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    ;

combined_assignment:
    VAR ADDASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::ADDASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created add assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR SUBASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created sub assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR MULASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created mul assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR DIVASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created div assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        $$ = new ParaCL::PrintStmt(std::unique_ptr<ParaCL::Expr>($2));
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB program RCUB {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        for (auto stmt : *$6) {
            body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>(stmt));
        }
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created while statement" << std::endl;)
        delete $6;
    }
    | WH LCIB expression RCIB program_node {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>($5));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));

        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created while statement" << std::endl;)
    }
    ;

input_statement:
    IN AS VAR {
    }
    ;

condition:
    if_condition { 
        $$ = new ParaCL::Condition(
            std::unique_ptr<ParaCL::IfCondition>($1)
        );
    }
    | else_condition {
        ($$)->add_else_condition(
            std::unique_ptr<ParaCL::ElseCondition>($1)
        );
    }
    ;

if_condition:
    IF LCIB expression RCIB LCUB program RCUB {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        for (auto stmt : *$6) {
            body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>(stmt));
        }
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
        $$ = new ParaCL::IfCondition(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        delete $6;
        ON_DEBUG(std::cout << "Created if statement with {}" << std::endl;)
    }
    | IF LCIB expression RCIB program_node {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>($5));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));

        $$ = new ParaCL::IfCondition(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created if statement whithOUT {}" << std::endl;)
    }
    ;

else_condition:
    ELSE LCUB program RCUB {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        for (auto node : *$3) {
            body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>(node));
        }
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
        $$ = new ParaCL::ElseCondition(
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        delete $3;
        ON_DEBUG(std::cout << "Created else statement with {}" << std::endl;)
    }
    | ELSE program_node {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::ASTNode>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::ASTNode>($2));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));

        $$ = new ParaCL::ElseCondition(
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created else statement withOUT {}" << std::endl;)
    }
    ;

expression:
    bool_expression { $$ = $1; }
    ;

bool_expression:
    and_or_expression { $$ = $1; }
    ;

and_or_expression:
    math_comparison_expression { $$ = $1; }
    | and_or_expression AND math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | and_or_expression OR math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

math_comparison_expression:
    add_sub_expression { $$ = $1; }
    | math_comparison_expression ISAB add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISABE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLS add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLSE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISEQ add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISNE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

add_sub_expression:
    mul_div_expression { $$ = $1; }
    | add_sub_expression ADD mul_div_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ADD, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | add_sub_expression SUB mul_div_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::SUB, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

mul_div_expression:
    factor { $$ = $1; }
    | mul_div_expression MUL factor { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | mul_div_expression DIV factor { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::DIV, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

factor:
    NUM { 
        $$ = new ParaCL::NumExpr($1);
    }
    | VAR { 
        $$ = new ParaCL::VarExpr(*$1);
        delete $1;
    }
    | LCIB expression RCIB { 
        $$ = $2;
    }
    | assignment_expression { 
        $$ = $1;
    }
    | IN {
        $$ = new ParaCL::InputExpr();
    }
    ;

assignment_expression:
    VAR AS expression {
        $$ = new ParaCL::AssignExpr(*$1, std::unique_ptr<ParaCL::Expr>($3));
        delete $1;
    }
    ;

%%

/* int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param); */


void yyerror(YYLTYPE* loc, const char* msg)
{
    std::cerr << current_file     << ":"
              << loc->first_line   << ":"
              << loc->first_column << ":"
              " paracl: error: " << msg << "\n";
}
