%{
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string.h>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

inline ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void show_error_context(YYLTYPE* loc);

void yyerror(YYLTYPE* loc, const char* msg);

int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);

std::string extract_token_at_position(YYLTYPE* loc);
std::string find_possible_token(const char* unexpected);
size_t levenshtein_distance(const std::string& s1, const std::string& s2);

%}

%define api.pure full
%locations

%precedence OR
%precedence AND
%precedence ISEQ ISNE
%precedence ISAB ISABE ISLS ISLSE
%precedence ADD SUB
%precedence MUL DIV REM
%precedence NEG NOT
%precedence AS
%precedence ADDASGN
%precedence SUBASGN
%precedence MULASGN
%precedence DIVASGN

%union {
    int                                     num_value      ;
    std   ::string*                         str_value      ;
    ParaCL::Stmt*                           stmt           ;
    ParaCL::Expr*                           expr           ;
    ParaCL::BlockStmt*                      block          ;
    ParaCL::ConditionStatement*             condition_stmt ;
    ParaCL::IfStatement*                    if_stmt        ;
    std   ::vector<ParaCL::ElifStatement*>* elif_stmts     ;
    ParaCL::ElifStatement*                  elif_stmt      ;
    ParaCL::ElseStatement*                  else_stmt      ;
    std   ::vector<ParaCL::Stmt*>*          stmt_vector    ;
}

%token <num_value> NUM
%token <str_value> VAR
%token LCIB RCIB LCUB RCUB
%token WH IN PRINT IF ELIF ELSE
%token SC

%type <stmt_vector> program statements
%type <block> block one_stmt_block
%type <stmt> statement assignment combined_assignment print_statement while_statement
%type <expr> expression assignment_expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression factor

%type <condition_stmt> condition_statement
%type <if_stmt>               if_statement
%type <elif_stmts>          elif_statements
%type <else_stmt>           else_statement

%start program

%%

program:
    statements {
        for (auto stmt : *$1)
        {
            program.statements.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        delete $1;
    }
    ;

statements:
    %empty {
        $$ = new std::vector<ParaCL::Stmt*>();
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
    | combined_assignment SC {
        $$ = $1;
    }
    | print_statement SC {
        $$ = $1;
    }
    | while_statement {
        $$ = $1;
    }
    | condition_statement {
        $$ = $1;
    }
    ;

assignment:
    VAR AS expression {
        $$ = new ParaCL::AssignStmt(*$1, std::unique_ptr<ParaCL::Expr>($3));
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
        delete $1;
    }
    | VAR SUBASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR MULASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR DIVASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        $$ = new ParaCL::PrintStmt(std::unique_ptr<ParaCL::Expr>($2));
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB block RCUB {
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        );
    }
    | WH LCIB expression RCIB one_stmt_block {
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>($5)
        );
    }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        auto cond_stmt = new ParaCL::ConditionStatement(
            std::unique_ptr<ParaCL::IfStatement>($1)
        );

        cond_stmt->add_elif_conditions($2);
        delete $2;

        if ($3) cond_stmt->add_else_condition(std::unique_ptr<ParaCL::ElseStatement>($3));

        $$ = cond_stmt;
    }
    ;

if_statement:
    IF LCIB expression RCIB LCUB block RCUB {
        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        );
    }
    | IF LCIB expression RCIB one_stmt_block {
        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($5)
        );
    }
    ;

elif_statements:
    %empty {
        $$ = new std::vector<ParaCL::ElifStatement*>();
    }
    | elif_statements ELIF LCIB expression RCIB LCUB block RCUB {
        $1->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($4),
            std::unique_ptr<ParaCL::BlockStmt>($7)
        ));
        $$ = $1;
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_block {
        $1->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($4),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        ));
        $$ = $1;
    }
    ;

else_statement:
    %empty { $$ = nullptr; }
    | ELSE LCUB block RCUB {
        $$ = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>($3)
        );
    }
    | ELSE one_stmt_block {
        $$ = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>($2)
        );
    }
    ;

expression:
    assignment_expression { $$ = $1; }
    ;

assignment_expression:
    logical_or_expression { $$ = $1; }
    | VAR AS assignment_expression %prec AS {
        $$ = new ParaCL::AssignExpr(*$1, std::unique_ptr<ParaCL::Expr>($3));
        delete $1;
    }
    | VAR ADDASGN assignment_expression %prec ADDASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::ADDASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR SUBASGN assignment_expression %prec SUBASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR MULASGN assignment_expression %prec MULASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR DIVASGN assignment_expression %prec DIVASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression %prec OR {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

logical_and_expression:
    equality_expression { $$ = $1; }
    | logical_and_expression AND equality_expression %prec AND {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

equality_expression:
    relational_expression { $$ = $1; }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

relational_expression:
    additive_expression { $$ = $1; }
    | relational_expression ISAB additive_expression %prec ISAB {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISABE additive_expression %prec ISABE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISLS additive_expression %prec ISLS {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISLSE additive_expression %prec ISLSE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

additive_expression:
    multiplicative_expression { $$ = $1; }
    | additive_expression ADD multiplicative_expression %prec ADD { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ADD,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | additive_expression SUB multiplicative_expression %prec SUB { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

multiplicative_expression:
    unary_expression { $$ = $1; }
    | multiplicative_expression MUL unary_expression %prec MUL { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::DIV, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | multiplicative_expression REM unary_expression %prec REM {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::REM,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

unary_expression:
    factor { $$ = $1; }
    | SUB unary_expression %prec NEG {
        $$ = new ParaCL::UnExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>($2)
        );
    }
    | NOT unary_expression %prec NOT {
        $$ = new ParaCL::UnExpr(
            ParaCL::token_t::NOT,
            std::unique_ptr<ParaCL::Expr>($2)
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
    | IN {
        $$ = new ParaCL::InputExpr();
    }
    ;

block:
    statements {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        for (auto stmt : *$1)
            body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));

        $$ = new ParaCL::BlockStmt(std::move(body_stmts));
        delete $1; /* delete vector of statements, created in 'statements' rule */
    }
    ;

one_stmt_block:
    statement {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>($1));
        $$ = new ParaCL::BlockStmt(std::move(body_stmts));
    }
    ;
%%

void yyerror(YYLTYPE* loc, const char* msg)
{
    std::cerr << current_file << ":"
              << loc->first_line << ":"
              << loc->first_column << ":"
              " ---> " << msg << "\n";
    
    // Извлекаем токен вокруг позиции ошибки
    std::string problematic_token = extract_token_at_position(loc);
    std::string possible_token = find_possible_token(problematic_token.c_str());
    
    if (!problematic_token.empty())
        std::cerr << "Problematic place: '" << problematic_token << "'\n\n";   
    
    show_error_context(loc);

    if (!possible_token.empty())
        std::cerr << "\nDid you mean: '" << possible_token << "'?\n";
}

void show_error_context(YYLTYPE* loc) {
    if (!yyin) return;
    
    long current_file_pos = ftell(yyin);
    
    // Go to start of the file
    rewind(yyin);
    
    char buffer[1024];
    int current_line = 1;

    while (current_line < loc->first_line && fgets(buffer, sizeof(buffer), yyin)) current_line++;

    if (current_line == loc->first_line && fgets(buffer, sizeof(buffer), yyin))
    {
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        
        std::cerr << loc->first_line << " | " << buffer << std::endl;
        
        std::cerr << "  | ";
        int i = 1;
        for (; i < loc->first_column; i++)
        {
            if (i < (int)strlen(buffer) && buffer[i-1] == '\t')
                std::cerr << "~~~~";
            else
                std::cerr << "~";
        }
        std::cerr << "^";

        for (; i < (int)strlen(buffer); i++) std::cerr << "~";

        std::cerr << "\n";
    }
    
    // Restoring the position in the file
    fseek(yyin, current_file_pos, SEEK_SET);
}


size_t levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));
    
    for (size_t i = 1; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 1; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i)
    {
        for (size_t j = 1; j <= len2; ++j)
        {
            size_t cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min(
            {
                dp[i-1][j] + 1,
                dp[i][j-1] + 1,
                dp[i-1][j-1] + cost
            });
        }
    }
    
    return dp[len1][len2];
}

std::string find_possible_token(const char* unexpected) {
    if (strlen(unexpected) == 0) return "";
    static const std::vector<std::string> known_tokens = {
        "if", "else", "while", "print", 
        "and", "or", "not",
        "+", "-", "*", "/", "%",
        "==", "!=", ">", ">=", "<", "<=",
        "=", "+=", "-=", "*=", "/=",
        "(", ")", "{", "}", ";", "?"
    };
    
    std::string best_match;
    size_t min_distance = std::numeric_limits<size_t>::max();
    
    for (const auto& token : known_tokens)
    {
        size_t distance = levenshtein_distance(unexpected, token);
        if (distance < min_distance && distance <= token.length())
        {
            min_distance = distance;
            best_match = token;
        }
    }
    
    return best_match;
}

std::string extract_token_at_position(YYLTYPE* loc) {
    if (!yyin) return "";
    
    long current_file_pos = ftell(yyin);
    
    rewind(yyin);
    
    char buffer[1024];
    int current_line = 1;
    std::string target_line;

    while (current_line < loc->first_line && fgets(buffer, sizeof(buffer), yyin)) {
        current_line++;
    }

    if (current_line == loc->first_line && fgets(buffer, sizeof(buffer), yyin)) {
        target_line = buffer;
    }
    
    fseek(yyin, current_file_pos, SEEK_SET);
    
    if (target_line.empty()) return "";
    
    // try to form token around the given position
    size_t pos = loc->first_column - 1;
    
    if (pos >= target_line.length()) return "";
    
    // go left from problem pos
    size_t start = pos;
    while (start > 0 && !isspace(target_line[start - 1])) {
        start--;
    }
    
    // find end of token (go right until space or end of line)
    size_t end = pos;
    while (end < target_line.length() && !isspace(target_line[end]) && 
           target_line[end] != '\0' && 
           target_line[end] != '\n' && 
           target_line[end] != '\r') {
        end++;
    }
    
    std::string token = target_line.substr(start, end - start);
    
    while (!token.empty() && 
           (token.front() == '(' || token.front() == ')' || 
            token.front() == '{' || token.front() == '}' ||
            token.front() == ';' || token.front() == ',')) {
        token.erase(0, 1);
        start++;
    }
    
    while (!token.empty() && 
           (token.back() == '(' || token.back() == ')' || 
            token.back() == '{' || token.back() == '}' ||
            token.back() == ';' || token.back() == ',')) {
        token.pop_back();
        end--;
    }
    
    return token;
}
