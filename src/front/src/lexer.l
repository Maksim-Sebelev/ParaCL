%{
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <sstream>
#include <source_location>
#include "paraCL_crutch_for_parsery.hpp"
#include "parser.tab.hpp"
#include "global/global.hpp"
#include "global/custom_console_output.hpp"


int current_num_value;
std::string current_var_value;
int eof_reached = 0;

void process_number  (const char* text);
void process_variable(const char* text);

extern FILE* yyin;

extern YYLTYPE yylloc;
#define YY_USER_INIT yylloc.first_line=1;

%}

%option noyywrap
%option nounput
%option noinput


DIGIT       [0-9]
LETTER      [a-zA-Z_]
SPACE       [ ]
TAB         [\t]
CARRIEGE    [\r]
NEWLINE     [\n]
WHITESPACE  {SPACE}|{TAB}|{CARRIEGE}

%%

SPACE          

{SPACE}         { yylloc.first_column++; }
{TAB}           { yylloc.first_column += 4; }
{CARRIEGE}      { }
{WHITESPACE}+   { }
{NEWLINE}       { yylloc.last_line++; yylloc.last_column = 1; }

"#".*          { }

"/*"([^*]|"*"+[^*/])*"*"+"/" {
    const char* p = yytext; assert(p);
    while (*p)
    {
        p++;
    }
}

"and"   { return AND; }
"or"    { return OR; }
"+"     { return ADD; }
"-"     { return SUB; }
"*"     { return MUL; }
"/"     { return DIV; }
">"     { return ISAB; }
">="    { return ISABE; }
"<"     { return ISLS; }
"<="    { return ISLSE; }
"=="    { return ISEQ; }
"!="    { return ISNE; }
"+="    { return ADDASGN; }
"-="    { return SUBASGN; }
"*="    { return MULASGN; }
"/="    { return DIVASGN; }
"("     { return LCIB; }
")"     { return RCIB; }
"{"     { return LCUB; }
"}"     { return RCUB; }
"while" { return WH; }
"?"     { return IN; }
"="     { return AS; }
"print" { return PRINT; }
";"     { return SC; }

[+-]?{DIGIT}+    { 
                process_number(yytext); 
                yylval.num_value = current_num_value;
                return NUM; 
            }

{LETTER}({LETTER}|{DIGIT})* { 
                process_variable(yytext); 
                yylval.str_value = new std::string(current_var_value);
                return VAR; 
            }

<<EOF>>     { 
                return 0;
            }

. { 
    std::cerr << "Error at line " << yylloc.first_line 
              << ", column " << yylloc.first_column 
              << ": unknown character '" << yytext << "'" << std::endl;
    return 0;
}

%%

void process_number(const char* text) {
    current_num_value = atoi(text);
}

void process_variable(const char* text) {
    current_var_value = text;
}

std::string get_current_code_place()
{
    return "no place";
}

std::string get_marked_line_with_error()
{
    return "no line with err";
}

void set_current_paracl_file(const std::string& file)
{
}
