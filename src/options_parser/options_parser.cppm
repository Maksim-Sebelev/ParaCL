module;

//---------------------------------------------------------------------------------------------------------------

#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "global/global.hpp"
#include "global/custom_console_output.hpp"

import paracl_extension;

#if defined(USE_LOGGER)
import pineaplog;
#endif /* defined(USE_LOGGER) */

//---------------------------------------------------------------------------------------------------------------

export module options_parser;

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values();

//---------------------------------------------------------------------------------------------------------------


export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

struct program_options_t
{
    bool                     help    : 1 = false;
    std::vector<std::string> sources;
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

class options_parser
{
    public:
        OptionsParsing::program_options_t get_program_options() const;

        options_parser() = delete;
        options_parser(int argc, char* argv[]);

    private:
        OptionsParsing::program_options_t program_options_;

        [[noreturn]]        
        void parse_flag_help          () const;
        void parse_not_a_flag         (const char* argument);

        [[noreturn]]
        void undefined_option         (const char* argument) const;
};

//---------------------------------------------------------------------------------------------------------------

export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

program_options_t parse_program_options(int argc, char* argv[])
{
    options_parser result = options_parser(argc, argv);
    return options_parser(argc, argv).get_program_options();
}

//---------------------------------------------------------------------------------------------------------------
} /* export namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

constexpr option long_options[] =
{
    {"help"   , no_argument, 0, 'h'},
    {""       , 0          , 0,  0 }, /* just for safety */
};

//---------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// class public methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

options_parser::options_parser(int argc, char* argv[]) :
program_options_()
{
    static constexpr int undefined_option_key = -1;

    for (int options_iterator = 1; options_iterator < argc; options_iterator++)
    {
        int option = getopt_long(argc, argv, "h", long_options, nullptr);
    
        switch (option)
        {
            case 'h'                 : parse_flag_help ();                       continue;
            case undefined_option_key: parse_not_a_flag(argv[options_iterator]); continue;
            default                  : undefined_option(argv[options_iterator]); continue;
        }
    }

    set_getopt_args_default_values();
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

[[noreturn]]
void options_parser::parse_flag_help() const
{
    std::cout <<
    BOLD
    "There are all flags and parametrs:"
    RESET_CONSOLE_OUT
    << R"(
    -h --help
    -v --verbose

        if you want to give source file, just write *.pcl file in command.

        -h, --help
            these flags show information about all flags and their functions.
    
        -v, --verbose
            use it to see, what paracl made undef the hood.
    
    So, that was all, what I know about flags in this program.
    Good luck, I love you )"
    << HEART;

    exit(EXIT_SUCCESS); // good exit :)
}

//---------------------------------------------------------------------------------------------------------------

void options_parser::parse_not_a_flag(const char* argument)
{
    if (not ParaCL::is_paracl_file_name(argument))
        undefined_option(argument); // exit 1

    program_options_.sources.push_back(std::string(argument));   
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::undefined_option(const char* argument) const
{
    assert(argument);
    std::cerr << "Undefined option '" << argument << "'" << std::endl
              << "I don`t know, what i need to do :("  << std::endl;

    exit(EXIT_FAILURE);
}

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values()
{
    optarg = nullptr;
    optind = 1;
}

//---------------------------------------------------------------------------------------------------------------

