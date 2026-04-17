%require "3.2"
%language "c++"
%locations
%define api.value.type variant
%define parse.error detailed
%define api.namespace {yy}
%define api.parser.class {parser}

%code requires {
    #include <cstdio>

    import thelast;

    extern FILE* yyin;
}

%code {
    #include <iostream>
    #include <vector>
    #include <string>

    ParaCL::ast::AST program;
    std::string current_file;

    int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);

    import llvm_ir_translator;
    import frontend_errors;
    import grammar_errors_parsing;
    import nodes_functional;

    #include "create-basic-node.hpp"

    SPECIALIZE_CREATE(ParaCL::ast::node::Print              , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::Scan               , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::Variable           , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::NumberLiteral      , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::StringLiteral      , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::UnaryOperator      , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::BinaryOperator     , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::While              , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement)
    SPECIALIZE_CREATE(ParaCL::ast::node::If                 , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_if_statement)
    SPECIALIZE_CREATE(ParaCL::ast::node::Else               , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement)
    SPECIALIZE_CREATE(ParaCL::ast::node::Condition          , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement)
    SPECIALIZE_CREATE(ParaCL::ast::node::Scope              , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression);
    SPECIALIZE_CREATE(ParaCL::ast::node::FunctionDeclaration, ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::FunctionCall       , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement, ParaCL::ast::node::generatable_expression)
    SPECIALIZE_CREATE(ParaCL::ast::node::Return             , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement)
    SPECIALIZE_CREATE(ParaCL::ast::node::Semicolon          , ParaCL::ast::node::dumpable, ParaCL::ast::node::generatable_statement)

    void parser_show_error(yy::location const & loc, std::string_view msg)
    {
        ParaCL::frontend::grammar::show_error(loc, msg, yyin);
    }

    ParaCL::ast::node::CodeLocation parser_location_cast(yy::location const & loc)
    {
        return ParaCL::frontend::grammar::location_cast(loc, yyin);
    }

    std::string parser_get_token_line(yy::location const & loc)
    {
        return ParaCL::frontend::grammar::get_token_line(loc, yyin);
    }

    ParaCL::ast::node::CodeLocation get_unary_operator_location(yy::location const & operator_location, yy::location const & arg_location)
    {
        return ParaCL::ast::node::CodeLocation
        {
            current_file,
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(operator_location.begin.line),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(arg_location.end.line),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(operator_location.begin.column),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(arg_location.end.column),
            parser_get_token_line(operator_location)
        };
    }

    ParaCL::ast::node::CodeLocation get_binary_operator_location
    (
        yy::location const & left_location,
        yy::location const & operator_location,
        yy::location const & right_location
    )
    {
        return ParaCL::ast::node::CodeLocation
        {
            current_file,
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(left_location.begin.line),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(right_location.end.line),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(left_location.begin.column),
            static_cast<ParaCL::ast::node::CodeLocation::code_place_uint_t>(right_location.end.column),
            parser_get_token_line(operator_location)  // или можно left_location для строки начала выражения
        };
    }
}

%right AS ADDASGN SUBASGN MULASGN DIVASGN REMASGN
%left OR
%left AND
%left ISEQ ISNE
%left ISAB ISABE ISLS ISLSE
%left ADD SUB
%left MUL DIV
%left REM
%right NEG NOT

%token <char> IN
%token <int> NUM
%token <std::string> STRING NAME

%nonassoc IF
%nonassoc ELIF
%nonassoc ELSE

%token WHILE
%token PRINT
%token <char> SC COMMA
%token <char> COLON DECLFUNC RET

%token LCIB RCIB LCUB RCUB


// statements
%type <std::vector<ParaCL::ast::node::BasicNode>> statements
%type <ParaCL::ast::node::BasicNode> statement scope one_statement_scope
%type <ParaCL::ast::node::BasicNode> condition while if else
%type <std::vector<ParaCL::ast::node::BasicNode>> elifs
// expressionss
%type <ParaCL::ast::node::BasicNode> expression binary_operator unary_operator variable number string scan brackets
%type <ParaCL::ast::node::BasicNode> special_expression print
// functions
%type <ParaCL::ast::node::BasicNode> function_assignment function_declaration function_call return
%type <std::vector<ParaCL::ast::node::BasicNode>> function_call_args
%type <std::vector<std::string>> function_decl_args


%start program
%%

program:
    statements {
        auto&& root_scope = ParaCL::ast::node::Scope(std::move($1));
        root_scope.location()
            .set_file(current_file)
            .set_code_excerpt("< global scope >")
            .set_line_begin(1).set_line_end(1)
            .set_column_begin(1).set_column_end(17)
            ;
        
        program = ParaCL::ast::AST(ParaCL::ast::node::create(std::move(root_scope)));
    }
    ;

statements:
    %empty { $$ = std::vector<ParaCL::ast::node::BasicNode>(); }
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
    | return             SC { $$ = std::move($1); }
    | print              SC { $$ = std::move($1); }
    |                    SC
    {
        $$ = ParaCL::ast::node::create(ParaCL::ast::node::Semicolon{});
        $$.location() = parser_location_cast(@1);
    }
    ;

while:
    WHILE brackets one_statement_scope %prec WHILE {
        auto&& w = ParaCL::ast::node::While(std::move($2), std::move($3));
        w.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(w));
    }
    | WHILE LCIB error RCIB one_statement_scope { parser_show_error(@3, "expected expression as 'while' condition"); YYABORT; }
    | WHILE LCIB expression error one_statement_scope { parser_show_error(@4, "expected ')'"); YYABORT; }
    | WHILE brackets error { parser_show_error(@3, "expected scope or statement"); YYABORT; }
    | WHILE error { parser_show_error(@2, "expected '(' after while"); YYABORT; }
    ;

condition:
    if %prec IF {
        auto&& cond = ParaCL::ast::node::Condition{};
        cond.add_condition(std::move($1));
        cond.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(cond));
    }
    | if elifs %prec IF {
        auto&& cond = ParaCL::ast::node::Condition{};
        cond.add_condition(std::move($1));
        for (auto&& e : $2) cond.add_condition(std::move(e));
        cond.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(cond));
    }
    | if else %prec IF {
        auto&& cond = ParaCL::ast::node::Condition{};
        cond.add_condition(std::move($1));
        cond.set_else(std::move($2));
        cond.location() = parser_location_cast(@1);;
        $$ = ParaCL::ast::node::create(std::move(cond));
    }
    | if elifs else %prec IF {
        auto&& cond = ParaCL::ast::node::Condition{};
        cond.add_condition(std::move($1));
        for (auto&& e : $2) cond.add_condition(std::move(e));
        cond.set_else(std::move($3));
        cond.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(cond));
    }
    ;

if:
    IF brackets one_statement_scope %prec IF {
        auto&& i = ParaCL::ast::node::If(std::move($2), std::move($3));
        i.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(i));
    }
    | IF LCIB error RCIB one_statement_scope %prec IF {
        parser_show_error(@3, "expected expression as 'if' condition");
        YYABORT;
    }
    | IF LCIB expression error one_statement_scope %prec IF {
        parser_show_error(@4, "expected ')' after if");
        YYABORT;
    }
    | IF brackets error %prec IF {
        parser_show_error(@3, "expected scope after if");
        YYABORT;
    }
    | IF error %prec IF {
        parser_show_error(@2, "expected '(' after if");
        YYABORT;
    }
    ;

elifs:
    ELIF brackets one_statement_scope %prec ELIF {
        auto&& vec = std::vector<ParaCL::ast::node::BasicNode>{};
        auto&& node = ParaCL::ast::node::If(std::move($2), std::move($3));
        node.location() = parser_location_cast(@1);
        vec.push_back(ParaCL::ast::node::create(std::move(node)));
        $$ = std::move(vec);
    }
    | elifs ELIF brackets one_statement_scope %prec ELIF {
        auto&& e = ParaCL::ast::node::If(std::move($3), std::move($4));
        e.location() = parser_location_cast(@2);
        $1.push_back(ParaCL::ast::node::create(std::move(e)));
        $$ = std::move($1);
    }
    ;

else:
    ELSE one_statement_scope %prec ELSE {
        auto&& e = ParaCL::ast::node::Else(std::move($2));
        e.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(e));
    }
    | ELSE error %prec ELSE {
        parser_show_error(@2, "expected scope or statement after 'else'"); 
        YYABORT;
    }
    ;

special_expression:
    scope { $$ = std::move($1); }
    | variable AS scope {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = parser_location_cast(@2);
        $$ = ParaCL::ast::node::create(std::move(node));
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
        auto&& node = ParaCL::ast::node::NumberLiteral(std::move($1));
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

string:
    STRING {
        auto&& node = ParaCL::ast::node::StringLiteral(std::move($1));
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

variable:
    NAME {
        auto&& node = ParaCL::ast::node::Variable(std::move($1));
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

brackets:
    LCIB expression RCIB {
        $$ = std::move($2);
    }
    ;

scan:
    IN {
        auto&& node = ParaCL::ast::node::Scan{};
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

binary_operator:
    variable AS expression %prec AS {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::ASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable ADDASGN expression %prec ADDASGN {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::ADDASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable SUBASGN expression %prec SUBASGN {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::SUBASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable MULASGN expression %prec MULASGN {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::MULASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable DIVASGN expression %prec DIVASGN {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::DIVASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable REMASGN expression %prec REMASGN {
        auto&& node = ParaCL::ast::node::BinaryOperator{
            ParaCL::ast::node::BinaryOperator::BinaryOperatorT::REMASGN,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | variable ADDASGN error      { parser_show_error(@3, "expected expression after '+='"); YYABORT; }
    | variable SUBASGN error      { parser_show_error(@3, "expected expression after '-='"); YYABORT; }
    | variable MULASGN error      { parser_show_error(@3, "expected expression after '*='"); YYABORT; }
    | variable DIVASGN error      { parser_show_error(@3, "expected expression after '/='"); YYABORT; }
    | variable REMASGN error      { parser_show_error(@3, "expected expression after '%='"); YYABORT; }
    | variable AS      error      { parser_show_error(@3, "expected expression after '='"); YYABORT; }
    | expression ADD expression %prec ADD {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ADD,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression SUB expression %prec SUB {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::SUB,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression MUL expression %prec MUL {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::MUL,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression DIV expression %prec DIV {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::DIV,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression REM expression %prec REM {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::REM,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISEQ expression %prec ISEQ {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISEQ,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISNE expression %prec ISNE {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISNE,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISAB expression %prec ISAB {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISAB,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISABE expression %prec ISABE {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISABE,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISLS expression %prec ISLS {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISLS,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression ISLSE expression %prec ISLSE {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::ISLSE,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression OR expression %prec OR {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::OR,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | expression AND expression %prec AND {
        auto&& node = ParaCL::ast::node::BinaryOperator
        {
            ParaCL::ast::node::BinaryOperator::AND,
            std::move($1),
            std::move($3)
        };
        node.location() = get_binary_operator_location(@1, @2, @3);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

print:
    PRINT function_call_args {
        auto&& p = ParaCL::ast::node::Print{std::move($2)};
        p.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(p));
    }
    | PRINT error {
        parser_show_error(@2, "expected expression after 'print'");
        YYABORT;
    }
    ;

unary_operator:
    SUB expression %prec NEG {
        auto&& node = ParaCL::ast::node::UnaryOperator
        {
            ParaCL::ast::node::UnaryOperator::MINUS,
            std::move($2)
        };
        node.location() = get_unary_operator_location(@1, @2);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | NOT expression %prec NOT {
        auto&& node = ParaCL::ast::node::UnaryOperator
        {
            ParaCL::ast::node::UnaryOperator::NOT,
            std::move($2)
        };
        node.location() = get_unary_operator_location(@1, @2);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | ADD expression %prec ADD {
        auto&& node = ParaCL::ast::node::UnaryOperator
        {
            ParaCL::ast::node::UnaryOperator::PLUS,
            std::move($2)
        };
        node.location() = get_unary_operator_location(@1, @2);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

scope:
    LCUB statements RCUB {
        auto&& node = ParaCL::ast::node::Scope(std::move($2));
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

one_statement_scope:
    statement {
        auto&& arg = std::move($1);
        if (arg.is_a<ParaCL::ast::node::Scope>())
        {
            $$ = std::move(arg);
        }
        else if (arg.is_a<ParaCL::ast::node::Semicolon>())
        {
            $$ = std::move(arg);
        }
        else
        {
            auto&& node = ParaCL::ast::node::Scope{};
            node.push_back(arg);
            node.location() = parser_location_cast(@1);
            $$ = ParaCL::ast::node::create(std::move(node));
        }
    }
    ;

function_assignment:
    variable AS function_declaration {
        auto&& asgn = ParaCL::ast::node::BinaryOperator{ParaCL::ast::node::BinaryOperator::ASGN,
            std::move($1),
            std::move($3)
        };
        asgn.location() = parser_location_cast(@2);
        $$ = ParaCL::ast::node::create(std::move(asgn));
    }
    ;

function_declaration:
    DECLFUNC LCIB function_decl_args RCIB scope {
        auto&& node = ParaCL::ast::node::FunctionDeclaration{"", std::move($3), std::move($5)};
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    | DECLFUNC LCIB function_decl_args RCIB COLON NAME scope {
        auto&& node = ParaCL::ast::node::FunctionDeclaration{std::move($6), std::move($3), std::move($7)};
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
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
    %empty { $$ = std::vector<ParaCL::ast::node::BasicNode>{}; }
    | function_call_args COMMA expression { $1.push_back(std::move($3)); $$ = std::move($1); }
    | expression { $$ = std::vector<ParaCL::ast::node::BasicNode>{std::move($1)}; }
    ;

function_call:
    NAME LCIB function_call_args RCIB {
        auto&& node = ParaCL::ast::node::FunctionCall{std::move($1), std::move($3)};
        node.location() = parser_location_cast(@1);
        $$ = ParaCL::ast::node::create(std::move(node));
    }
    ;

return:
    RET expression {
        auto&& node = ParaCL::ast::node::Return{std::move($2)};
        node.location() = parser_location_cast(@1);
        $$ = create(std::move (node));
    }
    | RET error {
        parser_show_error(@1, "expected expression after 'return'");
        YYABORT;
    }
    ;
%%
