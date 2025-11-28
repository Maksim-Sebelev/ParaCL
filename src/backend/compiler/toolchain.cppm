module;

#include <cstdlib>
#include "parser/parser.hpp"

export module paracl_toolchain;

import options_parser;
import paracl_llvm_ir_translator;
import paracl_linker;

namespace ParaCL
{

void create_objects_files(const OptionsParsing::program_options_t& program_options, const ProgramAST& ast);
void link_objects_to_executable(const OptionsParsing::program_options_t& program_options);

export void compile(const OptionsParsing::program_options_t& program_options, const ProgramAST& ast)
{
    create_objects_files(program_options, ast);
    link_objects_to_executable(program_options);
}

void create_objects_files(const OptionsParsing::program_options_t& program_options, const ProgramAST& ast)
{
    const size_t sources_size = program_options.sources.size();

    for (size_t it = 0, ite = sources_size; it != ite; ++it)
    {
        LLVMIRBuilder llvm_builder(program_options, it);
        llvm_builder.generate_ir(ast);
        llvm_builder.compile_ir();
    }
}

void link_objects_to_executable(const OptionsParsing::program_options_t& program_options)
{
    Linker linker(program_options);
    linker.link_objects_to_executable();
}

} /* namespace ParaCL */
