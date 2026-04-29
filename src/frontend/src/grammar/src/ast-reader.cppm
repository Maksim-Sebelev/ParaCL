module;


#include "parser.tab.hpp"

#include <cstdio>
#include <memory>
#include <string_view>
#include <memory>
#include <cassert>

extern FILE* yyin;
extern ParaCL::frontend::ast::AST program;
extern std::string current_file;

export module ast_reader;

export import options;

import thelast;
import frontend_errors;

namespace ParaCL::frontend::grammar
{

void set_current_paracl_file(std::string_view file)
{
    current_file = std::string(file);
}

export
ast::AST&& read_ast(ParaCL::options::Options const & options)
{
    auto&& input_file = options.input_file.string();
    set_current_paracl_file(input_file);

    auto CFileCloser = [](std::FILE* fptr) { assert(fptr && "std::fclose(nullptr)"); std::fclose(fptr); };
    using  CFilePtrRaiiWrapper = std::unique_ptr<std::FILE, decltype(CFileCloser)>;

    auto&& input_file_wrapper = CFilePtrRaiiWrapper{std::fopen(input_file.c_str(), "r")};
    auto&& input_file_ptr = input_file_wrapper.get();

    if (not input_file_ptr)
        throw error::no_such_file_error(input_file);

    yyin = input_file_ptr;

    auto&& paracl_parser = yy::parser{};

    auto&& result = paracl_parser.parse();

    if (result != 0)
        throw std::runtime_error("Parsing errors occured.");

    return std::move(program);
}

} /* namespace ParaCL::frontend::grammar */
