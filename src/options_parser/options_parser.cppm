module;

//---------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "global/custom_console_output.hpp"
#include "global/global.hpp"

#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module options_parser;

//---------------------------------------------------------------------------------------------------------------

import paracl_extension;
import paracl_info;

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values();

//---------------------------------------------------------------------------------------------------------------

export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

struct program_options_t
{
    std::string program_name;
    std::vector<std::string> sources;

    std::string executable_file = "a.out";
    bool compile : 1 = false;
    ON_GRAPHVIZ(bool ast_dump : 1 = false; std::string dot_file;)
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

class options_parser
{
  public:
    OptionsParsing::program_options_t get_program_options() const;

    options_parser() = delete;
    options_parser(int argc, char *argv[]);

  private:
    OptionsParsing::program_options_t program_options_;

    void set_program_name(const char *argv0);

    [[noreturn]]
    void parse_flag_help() const;
    [[noreturn]]
    void parse_flag_version() const;

    ON_GRAPHVIZ(void parse_flag_ast_dump();) /* ON_GRAPHVIZ */

    void parse_flag_compile();
    void parse_flag_output();

    void parse_not_a_flag(const char *argument);

    [[noreturn]]
    void undefined_option(const char *argument) const;
};

//---------------------------------------------------------------------------------------------------------------

export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

program_options_t parse_program_options(int argc, char *argv[])
{
    return options_parser(argc, argv).get_program_options();
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

enum flag_key
{
    help = 'h',
    version = 'v',
    compile = 'c',
    output = 'o',
    ON_GRAPHVIZ(ast_dump = 'd', ) undefined_option_key = -1
};

//---------------------------------------------------------------------------------------------------------------

constexpr option long_options[] = {
    {"help", no_argument, 0, help},
    {"version", no_argument, 0, version} ON_GRAPHVIZ(, {"ast-dump", required_argument, 0, ast_dump}),
    {"compile", no_argument, 0, compile},
    {"output", required_argument, 0, output},
    {"", 0, 0, 0}};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// class public methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

options_parser::options_parser(int argc, char *argv[]) : program_options_()
{
    LOGINFO("paracl: options parser: begin parse options");

    set_program_name(argv[0]);

    for (int options_iterator = 1; options_iterator < argc; options_iterator++)
    {
        int option = getopt_long(argc, argv, "hvd:co:", long_options, nullptr);

        switch (option)
        {
        case help:
            parse_flag_help();
            continue;
        case version:
            parse_flag_version();
            continue;

        case compile:
            parse_flag_compile();
            continue;

        case output:
            parse_flag_output();
            continue;

            ON_GRAPHVIZ(case ast_dump : parse_flag_ast_dump(); continue;)
        case undefined_option_key:
            parse_not_a_flag(argv[options_iterator]);
            continue;

        default:
            undefined_option(argv[options_iterator]);
            continue;
        }
    }

    set_getopt_args_default_values();

    LOGINFO("paracl: options parser: parse options completed");
}

//---------------------------------------------------------------------------------------------------------------

OptionsParsing::program_options_t options_parser::get_program_options() const
{
    return program_options_;
}

// class private methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

void options_parser::set_program_name(const char *argv0)
{
    msg_assert(argv0, "argv[0] is nullptr? are you sure, that you give here arg[0]?");
    program_options_.program_name = std::string(argv0);
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::parse_flag_help() const
{
    LOGINFO("paracl: options parser: find '--help' flag");

    std::cout << BOLD "There are all flags and parametrs:" RESET_CONSOLE_OUT << R"(
    -h --help
    -v --version
    -d --ast-dump=
        *.cl
            to give source file, just write *.cl file in command.

        -h, --help
            these flags show information about all flags and their functions.
    
        -v, --version
            use it to show paracl version, that installed on your device

        -d --ast-dump=
            use it, if you want to see ast for your paracl program.
            example: 
            paracl --ast-dump ast.dot a.cl
            paracl -dast.dot a.cl

                will be created ast.dot file in current directrory.
                this flag dont make .png or .svg files, only .dot.
                to compile .dot file in image use graphviz yourself
                example of graphviz usage:
                    dot -Tpng ast.dot -o ast.png

    So, that was all, what I know about flags in this program.
    Good luck, I love you )"
              << HEART << std::endl;

    LOGINFO("paracl: exit success");
    exit(EXIT_SUCCESS); // good exit :)
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::parse_flag_version() const
{
    LOGINFO("paracl: options parser: find '--version' flag");

    std::cout << "ParaCL\n"
                 "Version     : "
              << ParaCL::paracl_info.version
              << "\n"
                 "Build at    : "
              << ParaCL::paracl_info.build_date
              << "\n"
                 "Build type  : "
              << ParaCL::paracl_info.build_type
              << "\n"
                 "Build with  : "
              << ParaCL::paracl_info.compiler
              << "\n"
                 "Commit hash : "
              << ParaCL::paracl_info.git_commit
              << "\n"
                 "Architecture: "
              << ParaCL::paracl_info.architecture << std::endl;

    LOGINFO("paracl: exit success");

    exit(EXIT_SUCCESS); // good exit :)
}

//---------------------------------------------------------------------------------------------------------------

ON_GRAPHVIZ(void options_parser::parse_flag_ast_dump() {
    LOGINFO("paracl: options parser: --ast-dump={}", optarg);

    msg_assert(optarg, "nullptr is no expect here");

    program_options_.ast_dump = true;
    program_options_.dot_file = optarg;
}) /* ON_GRAPHVIZ */

//---------------------------------------------------------------------------------------------------------------

void options_parser::parse_flag_compile()
{
    LOGINFO("paracl: options parser: --compile");
    program_options_.compile = true;
}

//---------------------------------------------------------------------------------------------------------------

void options_parser::parse_flag_output()
{
    msg_assert(optarg, "nullprt is no expect here");
    LOGINFO("paracl: options parser: --output={}", optarg);
    program_options_.executable_file = optarg;
}

//---------------------------------------------------------------------------------------------------------------

void options_parser::parse_not_a_flag(const char *argument)
{
    msg_assert(argument, "nullptr is no expect here");

    LOGINFO("paracl: options parser: parse not a flag: \"{}\"", argument);

    if (not ParaCL::is_paracl_file_name(argument))
    {
        LOGERR("paracl: options parser: unexpected file: \"{}\"", argument);
        throw std::invalid_argument("bad source file: unexpected extentsion: '" WHITE + std::string(argument) +
                                    RESET_CONSOLE_OUT "', expect " + ParaCL::paracl_extension);
    }

    LOGINFO("paracl: options parser: find source file: \"{}\"", argument);

    program_options_.sources.push_back(std::string(argument));
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::undefined_option(const char *argument) const
{
    msg_assert(argument, "nullptr is no expect here");
    LOGERR("paracl: options parser: undefined option \"{}\"", argument);

    throw std::invalid_argument("Undefined option: '" + std::string(argument) + "'");
}

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values()
{
    LOGINFO("paracl: options parser: set getopt arg default values");

    optarg = nullptr;
    optind = 1;
}

//---------------------------------------------------------------------------------------------------------------
