#pragma once

#include <string>
#include <cstdint>
#include "parser.tab.hpp"

namespace ParseError
{

struct ErrorParseOptions
{
    bool show_similar_token : 1 = true;
    bool show_bad_token     : 1 = true;
    bool show_error_context : 1 = true;
    bool show_error_place   : 1 = true;
};


void        show_error_context(const yy::location& loc);
size_t      levenshtein_distance(const std::string& s1, const std::string& s2);
std::string find_possible_token(const char* unexpected);
std::string extract_token_at_position(const yy::location& loc);
void        set_error_parse_options(bool show_similar, bool show_bad_token, bool show_error_context);

} /* namespace ParseError */
