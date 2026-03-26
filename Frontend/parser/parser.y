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

    last::AST program;

    int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);

std::string get_token_line(const yy::location &loc)
{
    if (!yyin)
        return "";

    long current_file_pos = ftell(yyin);

    rewind(yyin);

    char buffer[1024];
    int current_line = 1;
    std::string target_line;

    while (current_line < loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
    {
        ++current_line;
    }

    if (current_line == loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
        target_line = buffer;

    target_line.pop_back();

    fseek(yyin, current_file_pos, SEEK_SET);

    return target_line;
}

    template <>
    last::node::CodeLocation::CodeLocation(yy::location loc) :
        file_(current_file),
        line_begin_(loc.begin.line),
        line_end_(loc.end.line),
        column_begin_(loc.begin.column),
        column_end_(loc.end.column),
        code_excerpt_(get_token_line(loc))
    {}
}


%right AS ADDASGN SUBASGN MULASGN DIVASGN
%left OR
%left AND
%left ISEQ ISNE
%left ISAB ISABE ISLS ISLSE
%left ADD SUB
%left MUL DIV
%left REM
%right NEG NOT

%precedence IN
%precedence <int> NUM
%token <std::string> STRING
%precedence <std::string> NAME

%nonassoc IF
%nonassoc ELIF
%nonassoc ELSE

%token WHILE
%token PRINT
%token SC COMMA
%token COLON DECLFUNC RET

%token LCIB RCIB LCUB RCUB


// statements
%type <std::vector<last::node::BasicNode>> statements
%type <last::node::BasicNode> statement scope one_statement_scope
%type <last::node::BasicNode> condition while if else
%type <std::vector<last::node::BasicNode>> elifs
// expressionss
%type <last::node::BasicNode> expression binary_operator unary_operator variable number string scan brackets
%type <last::node::BasicNode> special_expression print
// functions
%type <last::node::BasicNode> function_assignment function_declaration function_call return
%type <std::vector<last::node::BasicNode>> function_call_args
%type <std::vector<std::string>> function_decl_args


%start program
%%

program:
    statements {
        auto&& root_scope = last::node::Scope(std::move($1));
        program = last::AST(last::node::create(std::move(root_scope)));
    }
    ;

statements:
    %empty { $$ = std::vector<last::node::BasicNode>(); }
    | statements statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    ;

statement:
    condition               { $$ = std::move($1); }
    | while                 { $$ = std::move($1); }
    | special_expression    { $$ = std::move($1); }
    | expression         SC { $$ = std::move($1); }
    | print              SC { $$ = std::move($1); }
    | return             SC { $$ = std::move($1); }
    |                    SC { $$ = last::node::create(last::node::Scope{}); }
    ;

while:
    WHILE brackets one_statement_scope {
        auto&& w = last::node::While(std::move($2), std::move($3));
        w.location() = @1;
        $$ = last::node::create(std::move(w));
    }
    | WHILE LCIB error RCIB scope { ErrorHandler::showError(@3, "expected expression"); YYABORT; }
    | WHILE LCIB expression error scope { ErrorHandler::showError(@4, "expected ')'"); YYABORT; }
    | WHILE brackets error { ErrorHandler::showError(@3, "expected scope or statement"); YYABORT; }
    | WHILE error { ErrorHandler::showError(@2, "expected '(' after while"); YYABORT; }
    ;

condition:
    if %prec IF {
        auto&& cond = last::node::Condition{};
        cond.add_condition(std::move($1));
        cond.location() = @1;
        $$ = last::node::create(std::move(cond));
    }
    | if elifs %prec IF {
        auto&& cond = last::node::Condition{};
        cond.add_condition(std::move($1));
        for (auto&& e : $2) cond.add_condition(std::move(e));
        cond.location() = @1;
        $$ = last::node::create(std::move(cond));
    }
    | if else %prec IF {
        auto&& cond = last::node::Condition{};
        cond.add_condition(std::move($1));
        cond.set_else(std::move($2));
        cond.location() = @1;
        $$ = last::node::create(std::move(cond));
    }
    | if elifs else %prec IF {
        auto&& cond = last::node::Condition{};
        cond.add_condition(std::move($1));
        for (auto&& e : $2) cond.add_condition(std::move(e));
        cond.set_else(std::move($3));
        cond.location() = @1;
        $$ = last::node::create(std::move(cond));
    }
    ;

if:
    IF brackets one_statement_scope %prec IF {
        auto&& i = last::node::If(std::move($2), std::move($3));
        i.location() = @1;
        $$ = last::node::create(std::move(i));
    }
    | IF LCIB error RCIB one_statement_scope %prec IF {
        ErrorHandler::showError(@3, "expected condition in if");
        YYABORT;
    }
    | IF LCIB expression error one_statement_scope %prec IF {
        ErrorHandler::showError(@4, "expected ')' after if");
        YYABORT;
    }
    | IF brackets error %prec IF {
        ErrorHandler::showError(@3, "expected scope after if");
        YYABORT;
    }
    | IF error %prec IF {
        ErrorHandler::showError(@2, "expected '(' after if");
        YYABORT;
    }
    ;

elifs:
    ELIF brackets one_statement_scope %prec ELIF {
        auto&& vec = std::vector<last::node::BasicNode>{};
        auto&& node = last::node::If(std::move($2), std::move($3));
        node.location() = @1;
        vec.push_back(last::node::create(std::move(node)));
        $$ = std::move(vec);
    }
    | elifs ELIF brackets one_statement_scope %prec ELIF {
        auto&& e = last::node::If(std::move($3), std::move($4));
        e.location() = @2;
        $1.push_back(last::node::create(std::move(e)));
        $$ = std::move($1);
    }
    ;

else:
    ELSE one_statement_scope %prec ELSE {
        auto&& e = last::node::Else(std::move($2));
        e.location() = @1;
        $$ = last::node::create(std::move(e));
    }
    | ELSE error %prec ELSE {
        ErrorHandler::showError(@2, "expected scope or statement after 'else'"); 
        YYABORT;
    }
    ;

special_expression:
    scope { $$ = std::move($1); }
    | variable AS scope {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::ASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | function_declaration { $$ = std::move($1); }
    | function_assignment { $$ = std::move($1); }
    ;

expression:
    brackets          { $$ = std::move($1); }
    | binary_operator { $$ = std::move($1); }
    | unary_operator  { $$ = std::move($1); }
    | variable        { $$ = std::move($1); }
    | number          { $$ = std::move($1); }
    | string          { $$ = std::move($1); }
    | scan            { $$ = std::move($1); }
    | function_call   { $$ = std::move($1); }
    ;

number:
    NUM {
        auto&& node = last::node::NumberLiteral(std::move($1));
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

string:
    STRING {
        auto&& node = last::node::StringLiteral(std::move($1));
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

variable:
    NAME {
        auto&& node = last::node::Variable(std::move($1));
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

brackets:
    LCIB expression RCIB {
        $$ = std::move($2);
    }
    ;

scan:
    IN {
        auto&& node = last::node::Scan{};
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

binary_operator:
    variable AS expression %prec AS {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::BinaryOperatorT::ASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | variable ADDASGN expression %prec ADDASGN {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::BinaryOperatorT::ADDASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | variable SUBASGN expression %prec SUBASGN {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::BinaryOperatorT::SUBASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | variable MULASGN expression %prec MULASGN {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::BinaryOperatorT::MULASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | variable DIVASGN expression %prec DIVASGN {
        auto&& node = last::node::BinaryOperator{
            last::node::BinaryOperator::BinaryOperatorT::DIVASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | variable ADDASGN error { ErrorHandler::showError(@3, "expected expression after '+='"); YYABORT; }
    | variable SUBASGN error { ErrorHandler::showError(@3, "expected expression after '-='"); YYABORT; }
    | variable MULASGN error { ErrorHandler::showError(@3, "expected expression after '*='"); YYABORT; }
    | variable DIVASGN error { ErrorHandler::showError(@3, "expected expression after '/='"); YYABORT; }
    | variable AS      error { ErrorHandler::showError(@3, "expected expression after ='"); YYABORT; }
    | expression ADD expression %prec ADD {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ADD,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression SUB expression %prec SUB {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::SUB,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression MUL expression %prec MUL {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::MUL,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression DIV expression %prec DIV {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::DIV,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression REM expression %prec REM {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::REM,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISEQ expression %prec ISEQ {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISEQ,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISNE expression %prec ISNE {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISNE,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISAB expression %prec ISAB {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISAB,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISABE expression %prec ISABE {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISABE,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISLS expression %prec ISLS {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISLS,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression ISLSE expression %prec ISLSE {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISLSE,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression OR expression %prec OR {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::OR,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    | expression AND expression %prec AND {
        auto&& node = last::node::BinaryOperator
        {
            last::node::BinaryOperator::AND,
            std::move($1),
            std::move($3)
        };
        node.location() = @2;
        $$ = last::node::create(std::move(node));
    }
    ;

print:
    PRINT function_call_args {
        auto&& p = last::node::Print(std::move($2));
        p.location() = @1;
        $$ = last::node::create(std::move(p));
    }
    | PRINT error {
        ErrorHandler::showError(@2, "expected expression after 'variable'");
        YYABORT;
    }
    ;

unary_operator:
    SUB expression %prec NEG {
        auto&& node = last::node::UnaryOperator
        {
            last::node::UnaryOperator::MINUS,
            std::move($2)
        };
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    | NOT expression %prec NOT {
        auto&& node = last::node::UnaryOperator
        {
            last::node::UnaryOperator::NOT,
            std::move($2)
        };
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    | ADD expression %prec ADD {
        auto&& node = last::node::UnaryOperator
        {
            last::node::UnaryOperator::PLUS,
            std::move($2)
        };
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

scope:
    LCUB statements RCUB {
        auto&& node = last::node::Scope(std::move($2));
        auto&& location = node.location();
        location.set_all(current_file, @1.begin.line, @3.end.line, @1.begin.column, @3.end.column, "");
        $$ = last::node::create(std::move(node));
    }
    ;

one_statement_scope:
    statement {
        auto&& vec = std::vector<last::node::BasicNode>{std::move($1)};
        if (vec.size() == 1 and vec[0].is_a<last::node::Scope>())
            $$ = std::move(vec[0]);
        else
        {
            auto&& node = last::node::Scope(std::move(vec));
            node.location() = @1;
            $$ = last::node::create(std::move(node));
        }
    }
    ;

function_assignment:
    variable AS function_declaration {
        auto&& asgn = last::node::BinaryOperator{last::node::BinaryOperator::ASGN,
            std::move($1),
            std::move($3)
        };
        asgn.location() = @2;
        $$ = last::node::create(std::move(asgn));
    }
    ;

function_declaration:
    DECLFUNC LCIB function_decl_args RCIB scope {
        auto&& node = last::node::FunctionDeclaration{"", std::move($3), std::move($5)};
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    | DECLFUNC LCIB function_decl_args RCIB COLON NAME scope {
        auto&& node = last::node::FunctionDeclaration{std::move($6), std::move($3), std::move($7)};
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

function_decl_args:
    %empty { 
        $$ = std::vector<std::string>(); 
    }
    | function_decl_args NAME {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | function_decl_args COMMA NAME {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

function_call_args:
    %empty { $$ = std::vector<last::node::BasicNode>{}; }
    | function_call_args COMMA expression { $1.push_back(std::move($3)); $$ = std::move($1); }
    | expression { $$ = std::vector<last::node::BasicNode>{std::move($1)}; }
    ;

function_call:
    NAME LCIB function_call_args RCIB {
        auto&& node = last::node::FunctionCall{std::move($1), std::move($3)};
        node.location() = @1;
        $$ = last::node::create(std::move(node));
    }
    ;

return:
    RET expression {
        auto&& node = last::node::Return{std::move($2)};
        node.location() = @1;
        $$ = create(std::move(node));
    }
    ;
%%
