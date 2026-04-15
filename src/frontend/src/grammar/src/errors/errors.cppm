module;

#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <fstream>

#include "parser.tab.hpp" // for yy::location

extern std::string current_file;

export module grammar_errors_parsing;

export import thelast;

import frontend_errors;

namespace ParaCL::frontend::grammar
{

export
std::string get_token_line(const yy::location &loc, FILE* in)
{
    assert(in);

    auto&& current_file_pos = ftell(in);

    rewind(in);

    char buffer[1024];

    size_t current_line = 1;
    std::string target_line;

    while (current_line < loc.begin.line && fgets(buffer, sizeof(buffer), in))
    {
        ++current_line;
    }

    if (current_line == loc.begin.line && fgets(buffer, sizeof(buffer), in))
        target_line = buffer;

    target_line.pop_back();

    fseek(in, current_file_pos, SEEK_SET);

    return target_line;
}

export
ast::node::CodeLocation location_cast(yy::location const & loc, FILE* in)
{
    return ast::node::CodeLocation
    {
        current_file,
        static_cast<ast::node::CodeLocation::code_place_uint_t>(loc.begin.line),
        static_cast<ast::node::CodeLocation::code_place_uint_t>(loc.end.line),
        static_cast<ast::node::CodeLocation::code_place_uint_t>(loc.begin.column),
        static_cast<ast::node::CodeLocation::code_place_uint_t>(loc.end.column),
        get_token_line(loc, in)
    };
}

export
void show_error(yy::location const & loc, std::string_view msg, FILE* in, std::ostream& os = std::cerr)
{
    os << error::parser_error(location_cast(loc, in), msg) << "\n";
}

} /* namespace ParaCL::frontend::grammar */
