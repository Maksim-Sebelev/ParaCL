module;

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer/lexer.hpp"
#include "parser.tab.hpp"
#include "parser/parser.hpp"

#include "global/custom_console_output.hpp"
#include "global/global.hpp"

#if defined(GRAPHVIZ)
#include <iostream>
import ast_graph_dump;
#endif /* defined(GRAPHIVZ)*/

extern int yyparse();
extern void set_current_paracl_file(const std::string &);

extern FILE *yyin;
extern ParaCL::ProgramAST program;

export module run_paracl;

import options_parser;
import paracl_interpreter;

import paracl_toolchain;

namespace ParaCL
{
void no_sources_action(const OptionsParsing::program_options_t &program_options);
void one_source_action(const OptionsParsing::program_options_t &program_options);

export void run_paracl(const OptionsParsing::program_options_t &program_options)
{
    const std::vector<std::filesystem::path> sources = program_options.sources;
    const size_t sources_quantity = sources.size();

    msg_bad_exit(sources_quantity <= 1, "now we work only with 1 input file :(");

    if (sources_quantity == 0)
        return no_sources_action(program_options);

    else if (sources_quantity == 1)
        return one_source_action(program_options);

    builtin_unreachable_wrapper("now we dont parse any situations");
}

void no_sources_action(const OptionsParsing::program_options_t &program_options)
{
    set_current_paracl_file("stdin");
    yyin = stdin;

    yy::parser parser;
    int result = parser.parse();

    if (result != EXIT_SUCCESS)
        throw std::runtime_error("Parsing failed");
    ON_GRAPHVIZ(if (program_options.ast_dump) {
        try
        {
            ast_dump(program, program_options.dot_file);
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << ERROR_MSG("graphviz ast dump failed:\n") << e.what() << "\n";
        }
        catch (...)
        {
            std::cerr << ERROR_MSG("graphviz ast dump failed with unknow exception!\n");
        }
    }) /* ON_GRAPHVIZ */

    if (not program_options.compile)
       interpret(program);
    else
        compile(program_options, program);
}

void one_source_action(const OptionsParsing::program_options_t &program_options)
{
    const std::filesystem::path &source = program_options.sources[0];
    set_current_paracl_file(source.string());

    FILE *input_file = fopen(source.c_str(), "rb");

    if (not input_file)
        throw std::invalid_argument(RED BOLD "no such file: " RESET_CONSOLE_OUT WHITE + source.string());

    yyin = input_file;

    yy::parser parser;
    int result = parser.parse();

    fclose(input_file);

    if (result != EXIT_SUCCESS)
        throw std::runtime_error("parsing failed");

ON_GRAPHVIZ(
    if (program_options.ast_dump) {
    try
    {
        ast_dump(program, program_options.dot_file);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << ERROR_MSG("graphviz ast dump failed:\n") << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << ERROR_MSG("graphviz ast dump failed with unknow exception!\n");
    }
}) /* ON_GRAPHVIZ */

    if (not program_options.compile)
       interpret(program);
    else
        compile(program_options, program);
}

} /* namespace ParaCL */
