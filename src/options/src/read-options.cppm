//---------------------------------------------------------------------------------------------------------------
module;
//---------------------------------------------------------------------------------------------------------------

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>

#include <boost/program_options.hpp>

#define RED "\e[0;31m"
#define VIOLET "\e[0;35m"
#define GREEN "\e[0;32m"

#define BOLD "\033[1m"

#define WARNING VIOLET BOLD
#define ERROR   RED BOLD

#define RESET "\e[0m"

//---------------------------------------------------------------------------------------------------------------
export module read_options;
//---------------------------------------------------------------------------------------------------------------

export import options;

//---------------------------------------------------------------------------------------------------------------
namespace ParaCL::options
{
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
namespace error
{
//---------------------------------------------------------------------------------------------------------------

std::string show_error(std::string_view msg)
{
    auto&& explain = std::ostringstream{};
    explain << BOLD "paracl: " RESET ERROR "error:" RESET " " BOLD << msg << RESET "\n";

    return explain.str();
}

class error : public std::exception
{
protected: std::string msg_;
private:
    const char* what_() const noexcept
    { return msg_.c_str(); }

public:
    const char* what() const noexcept override
    { return what_(); }
};

class unknown_argument_error : public error
{
public:
    unknown_argument_error(std::string_view arg)
    {
        auto&& explain = "unknown argument: '"+std::string(arg)+"'";
        msg_ = show_error(explain);
    }
};

class no_input_files_error : public error
{
public:
    no_input_files_error()
    {
        auto&& explain = "no input files";
        msg_ = show_error(explain);
    }
};

class unsupported_file_format_error : public error
{
public:
    unsupported_file_format_error(std::filesystem::path const & file)
    {
        auto&& explain = "Unsupported file format: "+file.string()+". Expected '.cl' file as input.";
        msg_ = show_error(explain);
    }
};


class unsupported_optimize_level_error : public error
{
public:
    unsupported_optimize_level_error(std::string_view optimize_level)
    {
        auto&& explain = "Unsupported optimize level '"+std::string(optimize_level)+"'. Supported values are 0, 1, 2 or 3";
        msg_ = show_error(explain);
    }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace error */
//---------------------------------------------------------------------------------------------------------------

export
Options read_options(int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description desc(
R"(
 [0;1;31;91mm[0;1;33;93mmm[0;1;32;92mmm[0m                         [0;1;36;96mm[0;1;34;94mmm[0m  [0;1;31;91mm[0m     
 [0;1;33;93m#[0m   [0;1;36;96m"[0;1;34;94m#[0m  [0;1;35;95mm[0;1;31;91mmm[0m    [0;1;36;96mm[0m [0;1;34;94mmm[0m   [0;1;31;91mm[0;1;33;93mmm[0m   [0;1;36;96mm[0;1;34;94m"[0m   [0;1;31;91m"[0m [0;1;33;93m#[0m     
 [0;1;32;92m#[0;1;36;96mmm[0;1;34;94mm#[0;1;35;95m"[0m [0;1;31;91m"[0m   [0;1;32;92m#[0m   [0;1;34;94m#"[0m  [0;1;31;91m"[0m [0;1;33;93m"[0m   [0;1;36;96m#[0m  [0;1;34;94m#[0m      [0;1;32;92m#[0m     
 [0;1;36;96m#[0m      [0;1;33;93mm"[0;1;32;92m""[0;1;36;96m#[0m   [0;1;35;95m#[0m     [0;1;32;92mm"[0;1;36;96m""[0;1;34;94m#[0m  [0;1;35;95m#[0m      [0;1;36;96m#[0m     
 [0;1;34;94m#[0m      [0;1;32;92m"m[0;1;36;96mm"[0;1;34;94m#[0m   [0;1;31;91m#[0m     [0;1;36;96m"m[0;1;34;94mm"[0;1;35;95m#[0m   [0;1;33;93m"m[0;1;32;92mmm[0;1;36;96m"[0m [0;1;34;94m#m[0;1;35;95mmm[0;1;31;91mmm[0m


This is the open-source compiler of the ParaCL from MIPT bachelor C++ course
from Konstantin Vladimirov (https://github.com/tilir), based on LLVM.

Usage:
paracl <source>.cl [options]

Suppoted options)"
    );

    po::options_description visible("General options");

    visible.add_options()
        ("help,h", "produce this message")
        ("version,v", "show version of project")
        ("output,o", po::value<std::string>(), "output file with generated program")
        ("save-temps", "save temporary files generated during compilation")
        ("ast-dump", "dump AST to stdout")
        ("debug,d", "generate debug information")
        ("tmp-dir", po::value<std::string>(), "directory for temporary files (default: /tmp)")
        ("optimize-level,O", po::value<std::string>(), "optimization level (0, 1, 2, 3)");

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input", po::value<std::string>(), "input file");

    po::options_description all;
    all.add(visible).add(hidden);

    po::positional_options_description pos;
    pos.add("input", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
        .options(all)
        .positional(pos)
        .style(po::command_line_style::unix_style | 
               po::command_line_style::allow_sticky)
        .run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    
    if (vm.count("version")) {
        std::cout << "ParaCL-" << PARACL_VERSION << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    
    if (not vm.count("input"))
        throw error::no_input_files_error();
    
    auto&& options = Options{};
    
    options.input_file = vm["input"].as<std::string>();
    if (options.input_file.extension() != ".cl")
        throw error::unsupported_file_format_error(options.input_file);

    if (vm.count("output")) {
        options.output_file = vm["output"].as<std::string>();
        options.tmp_ir_file = options.output_file.filename();
        options.tmp_ir_file = options.tmp_directory / options.tmp_ir_file;
        options.tmp_ir_file.replace_extension(".ll");
    }
    
    if (vm.count("optimize-level"))
    {
        std::string opt_level = vm["optimize-level"].as<std::string>();

        if (not opt_level.empty() and opt_level[0] == 'O')
            opt_level = opt_level.substr(1);

        if (opt_level == "0")
            options.optimize_level = OptimizeLevel::O0;
        else if (opt_level == "1")
            options.optimize_level = OptimizeLevel::O1;
        else if (opt_level == "2")
            options.optimize_level = OptimizeLevel::O2;
        else if (opt_level == "3")
            options.optimize_level = OptimizeLevel::O3;
        else
            throw error::unsupported_optimize_level_error(opt_level);
    }

    if (vm.count("tmp-dir"))
        options.tmp_directory = vm["tmp-dir"].as<std::string>();
    
    options.save_temps = vm.count("save-temps") > 0;
    options.ast_dump = vm.count("ast-dump") > 0;
    options.debug = vm.count("debug") > 0;

    options.correct_configure();
    return options;
}

//---------------------------------------------------------------------------------------------------------------
} // namespace ParaCL::options
//---------------------------------------------------------------------------------------------------------------
