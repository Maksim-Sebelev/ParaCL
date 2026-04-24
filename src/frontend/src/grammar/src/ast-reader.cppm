module;

import thelast;

#include "parser.tab.hpp"

#include <cstdio>
#include <memory>
#include <string_view>

extern FILE* yyin;
extern ParaCL::frontend::ast::AST program;
extern std::string current_file;

export module ast_reader;

export import options;

import frontend_errors;

namespace ParaCL::frontend::grammar
{

void set_current_paracl_file(std::string_view file)
{
    current_file = std::string(file);
}

export
ParaCL::frontend::ast::AST&& read_ast(ParaCL::options::Options const & options)
{
    auto&& input_file = options.input_file.string();
    set_current_paracl_file(input_file);

    auto&& input_file_ptr = std::fopen(input_file.c_str(), "r");

    if (not input_file_ptr)
        throw error::no_such_file_error(input_file);

    yyin = input_file_ptr;

    auto&& paracl_parser = yy::parser{};
    
    auto&& result = paracl_parser.parse();

    if (result != 0)
        throw std::runtime_error("Parsing errors occured.");

    std::fclose(input_file_ptr);

    return std::move(program);
}

} /* namespace ParaCL::frontend::grammar */
