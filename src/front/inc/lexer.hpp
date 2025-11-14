#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

extern int current_num_value;
extern std::string current_var_value;

int yylex();

std::string get_current_code_place();
std::string get_marked_line_with_error();


#endif // LEXER_HPP
