%require "3.2"
%language "c++"
%locations
%define api.value.type variant
%define parse.error detailed
%define api.namespace {yy}
%define api.parser.class {parser}

%code requires {
    #include <iostream>
    #include <cstdio>
    #include <vector>
    #include <string>
    
    #include <boost/json.hpp>
    #include <boost/json/object.hpp>

    import thelast;
    #include "create-basic-node.hpp"
    CREATE_SAME(boost::json::value(), last::node::dumpable)

    extern FILE* yyin;
    extern std::string current_file;
    extern int current_num_value;
    extern std::string current_var_value;
}

%code {
    #include "parse_error.hpp"
    #include "lexer.hpp"
    #include "check_variables.hpp"


    last::AST program;
    ParaCL::ParserNameTable name_table;

    int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);
}

%precedence OR
%precedence AND
%precedence ISEQ ISNE
%precedence ISAB ISABE ISLS ISLSE
%precedence ADD SUB
%precedence MUL DIV REM
%precedence NEG NOT
%right AS
%right ADDASGN SUBASGN MULASGN DIVASGN
%precedence THEN
%precedence ELIF
%precedence ELSE

%token <int> NUM
%token <std::string> NAME
%token LCIB RCIB LCUB RCUB
%token WH IN PRINT IF
%token SC COMMA
%token <std::string> STRING
%token DECLFUNC RET
%token COLON

%type <std::vector<last::node::BasicNode>> statements function_call_args
%type <last::node::BasicNode> statement assignment
%type <last::node::BasicNode> print_expression while_statement condition_statement
%type <last::node::BasicNode> expression assignment_expression logical_or_expression primary_expression
%type <last::node::BasicNode> logical_and_expression equality_expression relational_expression
%type <last::node::BasicNode> additive_expression multiplicative_expression unary_expression return_statement
%type <last::node::BasicNode> scope one_stmt_scope if_statement else_statement
%type <std::vector<last::node::BasicNode>> elif_statements
%type <last::node::BasicNode> function_declaration function_call
%type <std::vector<std::string>> function_decl_args
%type <last::node::BasicNode> special_expression function_assignment scope_assignment

%start program
%%

program:
    create_global_scope statements leave_global_scope {
        auto&& root_scope = last::node::Scope(std::move($2));
        program = last::AST(last::node::create(std::move(root_scope)));
    }
    ;

create_global_scope:
    %empty { name_table.new_scope(); }
    ;

leave_global_scope:
    %empty { name_table.leave_scope(); }
    ;

statements:
    %empty { $$ = std::vector<last::node::BasicNode>(); }
    | statements statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | statements LCUB scope RCUB {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

statement:
    assignment SC { $$ = std::move($1); }
    | while_statement { $$ = std::move($1); }
    | condition_statement { $$ = std::move($1); }
    | SC { $$ = last::node::create(last::node::Scope{}); }
    | return_statement SC { $$ = std::move($1); }
    | special_expression { $$ = std::move($1); }
    | expression SC { $$ = std::move($1); }
    ;

assignment:
    NAME AS expression {
        name_table.declare_or_do_nothing_if_already_declared($1);

        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );

        $$ = last::node::create(std::move(binop));
    }
    | NAME AS error {
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    ;

print_expression:
    PRINT function_call_args {
        auto&& p = last::node::Print(std::move($2));
        $$ = last::node::create(std::move(p));
    }
    | PRINT error {
        ErrorHandler::throwError(@2, "expected expressions after print");
        YYABORT;
    }
    ;

function_call_args:
    %empty { $$ = std::vector<last::node::BasicNode>(); }
    | function_call_args expression { $1.push_back(std::move($2)); $$ = std::move($1); }
    | function_call_args COMMA expression { $1.push_back(std::move($3)); $$ = std::move($1); }
    ;


function_decl_args:
    %empty { 
        $$ = std::vector<std::string>(); 
        name_table.new_scope(); 
    }
    | function_decl_args NAME {
        name_table.declare_or_do_nothing_if_already_declared($2);
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | function_decl_args COMMA NAME {
        name_table.declare_or_do_nothing_if_already_declared($3);
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB scope RCUB {
        auto&& w = last::node::While(std::move($3), std::move($6));
        $$ = last::node::create(std::move(w));
    }
    | WH LCIB expression RCIB one_stmt_scope {
        auto&& w = last::node::While(std::move($3), std::move($5));
        $$ = last::node::create(std::move(w));
    }
    | WH LCIB error RCIB LCUB scope RCUB { ErrorHandler::throwError(@3, "expected condition in while"); YYABORT; }
    | WH LCIB expression error LCUB scope RCUB { ErrorHandler::throwError(@4, "expected ')' after while condition"); YYABORT; }
    | WH LCIB expression RCIB error { ErrorHandler::throwError(@5, "expected scope after while"); YYABORT; }
    | WH error { ErrorHandler::throwError(@2, "expected '(' after while"); YYABORT; }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        last::node::Condition cond;
        cond.add_condition(std::move($1));
        for (auto&& e : $2) cond.add_condition(std::move(e));
        if ($3) cond.set_else(std::move($3));
        $$ = last::node::create(std::move(cond));
    }
    ;

if_statement:
    IF LCIB expression RCIB LCUB scope RCUB {
        auto&& i = last::node::If(std::move($3), std::move($6));
        $$ = last::node::create(std::move(i));
    }
    | IF LCIB expression RCIB one_stmt_scope %prec THEN {
        auto&& i = last::node::If(std::move($3), std::move($5));
        $$ = last::node::create(std::move(i));
    }
    | IF LCIB error RCIB LCUB scope RCUB { ErrorHandler::throwError(@3, "expected condition in if"); YYABORT; }
    | IF LCIB expression error LCUB scope RCUB { ErrorHandler::throwError(@4, "expected ')' after if"); YYABORT; }
    | IF LCIB expression RCIB error { ErrorHandler::throwError(@5, "expected scope after if"); YYABORT; }
    | IF error { ErrorHandler::throwError(@2, "expected '(' after if"); YYABORT; }
    ;

elif_statements:
    %empty { $$ = std::vector<last::node::BasicNode>(); }
    | elif_statements ELIF LCIB expression RCIB LCUB scope RCUB %prec ELIF {
        auto&& e = last::node::If(std::move($4), std::move($7));
        $1.push_back(last::node::create(std::move(e)));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_scope %prec ELIF {
        auto&& e = last::node::If(std::move($4), std::move($6));
        $1.push_back(last::node::create(std::move(e)));
        $$ = std::move($1);
    }
    ;

else_statement:
    %empty { $$ = last::node::BasicNode{}; }
    | ELSE LCUB scope RCUB %prec ELSE {
        auto&& e = last::node::Else(std::move($3));
        $$ = last::node::create(std::move(e));
    }
    | ELSE one_stmt_scope %prec ELSE {
        auto&& e = last::node::Else(std::move($2));
        $$ = last::node::create(std::move(e));
    }
    | ELSE error { ErrorHandler::throwError(@2, "expected scope after else"); YYABORT; }
    ;
primary_expression:
    NUM { $$ = last::node::create(last::node::NumberLiteral($1)); }
    | NAME {
        if (name_table.is_not_force_declare($1)) {
            ErrorHandler::throwError(@1, "using unforce_declared variable: " + $1);
            YYABORT;
        }
        $$ = last::node::create(last::node::Variable(std::move($1)));
    }
    | LCIB expression RCIB { $$ = std::move($2); }
    | IN { $$ = last::node::create(last::node::Scan{}); }
    | STRING { $$ = last::node::create(last::node::StringLiteral(std::move($1))); }
    | function_call { $$ = std::move($1); }
    ;

unary_expression:
    primary_expression { $$ = std::move($1); }
    | SUB unary_expression %prec NEG {
        auto&& unop = last::node::UnaryOperator(
            last::node::UnaryOperator::UnaryOperatorT::MINUS,
            std::move($2)
        );
        $$ = last::node::create(std::move(unop));
    }
    | NOT unary_expression %prec NOT {
        auto&& unop = last::node::UnaryOperator(
            last::node::UnaryOperator::UnaryOperatorT::NOT,
            std::move($2)
        );
        $$ = last::node::create(std::move(unop));
    }
    | ADD unary_expression %prec NEG { $$ = std::move($2); }
    ;

multiplicative_expression:
    unary_expression { $$ = std::move($1); }
    | multiplicative_expression MUL unary_expression %prec MUL {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::MUL,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::DIV,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | multiplicative_expression REM unary_expression %prec REM {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::REM,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

additive_expression:
    multiplicative_expression { $$ = std::move($1); }
    | additive_expression ADD multiplicative_expression %prec ADD {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ADD,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | additive_expression SUB multiplicative_expression %prec SUB {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::SUB,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

relational_expression:
    additive_expression { $$ = std::move($1); }
    | relational_expression ISAB additive_expression %prec ISAB {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISAB,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | relational_expression ISABE additive_expression %prec ISABE {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISABE,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | relational_expression ISLS additive_expression %prec ISLS {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISLS,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | relational_expression ISLSE additive_expression %prec ISLSE {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISLSE,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

equality_expression:
    relational_expression { $$ = std::move($1); }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISEQ,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ISNE,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

logical_and_expression:
    equality_expression { $$ = std::move($1); }
    | logical_and_expression AND equality_expression %prec AND {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::AND,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = std::move($1); }
    | logical_or_expression OR logical_and_expression %prec OR {
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::OR,
            std::move($1), std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

assignment_expression:
    logical_or_expression { $$ = std::move($1); }
    | NAME AS assignment_expression %prec AS {
        name_table.declare_or_do_nothing_if_already_declared($1);
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | NAME ADDASGN assignment_expression %prec ADDASGN {
        if (name_table.is_not_force_declare($1)) {
            ErrorHandler::throwError(@1, "using unforce_declared variable: " + $1);
            YYABORT;
        }
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::ADDASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | NAME SUBASGN assignment_expression %prec SUBASGN {
        if (name_table.is_not_force_declare($1)) {
            ErrorHandler::throwError(@1, "using unforce_declared variable: " + $1);
            YYABORT;
        }
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::SUBASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | NAME MULASGN assignment_expression %prec MULASGN {
        if (name_table.is_not_force_declare($1)) {
            ErrorHandler::throwError(@1, "using unforce_declared variable: " + $1);
            YYABORT;
        }
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::MULASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    | NAME DIVASGN assignment_expression %prec DIVASGN {
        if (name_table.is_not_force_declare($1)) {
            ErrorHandler::throwError(@1, "using unforce_declared variable: " + $1);
            YYABORT;
        }
        auto&& binop = last::node::BinaryOperator(
            last::node::BinaryOperator::BinaryOperatorT::DIVASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        );
        $$ = last::node::create(std::move(binop));
    }
    ;

expression:
    assignment_expression { $$ = std::move($1); } |
    LCUB scope RCUB { $$ = std::move($2); } |
    print_expression { $$ = std::move($1); }
    ;

scope:
    scope_enter_action statements scope_leave_action {
        auto&& s = last::node::Scope(std::move($2));
        $$ = last::node::create(std::move(s));
    }
    ;

one_stmt_scope:
    scope_enter_action statement scope_leave_action {
        std::vector<last::node::BasicNode> vec{std::move($2)};
        auto&& s = last::node::Scope(std::move(vec));
        $$ = last::node::create(std::move(s));
    }
    ;

return_statement:
    RET expression {
        $$ = create(last::node::Return{std::move($2)});
    }
    ;

function_declaration:
    DECLFUNC LCIB function_decl_args RCIB LCUB scope RCUB {
        $$ = last::node::create(last::node::FunctionDeclaration{"", std::move($3), std::move($6)});
    } |
    DECLFUNC LCIB function_decl_args RCIB COLON NAME LCUB scope RCUB {
        $$ = last::node::create(last::node::FunctionDeclaration{std::move($6), std::move($3), std::move($8)});
    }
    ;

function_call:
    NAME LCIB function_call_args RCIB {
        $$ = last::node::create(last::node::FunctionCall{std::move($1), std::move($3)});
    }
    ;

scope_assignment:
    NAME AS LCUB scope RCUB {
        name_table.declare_or_do_nothing_if_already_declared($1);
        auto&& asgn = last::node::BinaryOperator{last::node::BinaryOperator::ASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($4)
        };

        $$ = last::node::create(std::move(asgn));
    }
    ;

function_assignment:
    NAME AS function_declaration {
        name_table.declare_or_do_nothing_if_already_declared($1);
        auto&& asgn = last::node::BinaryOperator{last::node::BinaryOperator::ASGN,
            last::node::create(last::node::Variable(std::move($1))),
            std::move($3)
        };
    
        $$ = last::node::create(std::move(asgn));
    }
    ;

special_expression:
    function_declaration
    { $$ = std::move($1); }
    | function_assignment
    { $$ = std::move($1); }
    | scope_assignment
    { $$ = std::move($1); }
    ;

scope_enter_action: %empty { name_table.new_scope(); } ;
scope_leave_action: %empty { name_table.leave_scope(); } ;

%%
