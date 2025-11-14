module;

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>

#include "global/global.hpp"
#include "global/custom_console_output.hpp"
// могли бы import paraCL; но спасибо нашему доблестному bison
#include "paraCL_crutch_for_parsery.hpp"
#include "parser.tab.hpp"
#include "lexer.hpp"


extern FILE* yyin;
extern int yyparse();
extern void set_current_paracl_file(const std::string&);

extern ParaCL::ProgramAST program;

export module run_paracl;

import paracl_extension;
import options_parser;

namespace ParaCL
{
int  no_sources_action      ();
int  one_source_action      (const std::string& source);

[[noreturn]]
void failed_open_source_file(const std::string& source);
} /* namespace ParaCL */

export namespace ParaCL
{

int run_paracl(const OptionsParsing::program_options_t& program_options)
{
    const std::vector<std::string> sources          = program_options.sources  ;
    const size_t                   sources_quantity = sources        .size   ();

    msg_bad_exit(sources_quantity <= 1, "now we work only with 1 input file :(");

    if (sources_quantity == 0)
        return no_sources_action();

    if (sources_quantity == 1)
        return one_source_action(sources[0]);

    builtin_unreachable_wrapper("now we dont parse any situations");
}


} /* export namespace ParaCL */


namespace ParaCL
{

int no_sources_action()
{
    set_current_paracl_file("stdin");
    YYLTYPE loc;
    loc.first_line = loc.last_line = 1;
    loc.first_column = loc.last_column = 0;
    

    yyin = stdin;
    int result =  yyparse();

#if defined(GRAPHVIZ)
    dump(program);
#endif /* defined(GRAPHVIZ) */

    compile(program);

    return result;
}


int one_source_action(const std::string& source)
{
    set_current_paracl_file(source);
    YYLTYPE loc;
    loc.first_line = loc.last_line = 1;
    loc.first_column = loc.last_column = 0;

    FILE* input_file = fopen(source.c_str(), "r");

    if (not input_file)
        failed_open_source_file(source); // exit 1


    yyin = input_file;

    int result = yyparse();

#if defined(GRAPHVIZ)
    dump(program);
#endif /* defined(GRAPHVIZ) */

    compile(program);
    
    fclose(input_file);

    return result;
}

[[noreturn]]
void failed_open_source_file(const std::string& source)
{
    std::cerr << RED BOLD "No such file: " RESET_CONSOLE_OUT WHITE << source << "\n";
    exit(EXIT_FAILURE);
}

} /* namespace ParaCL */
