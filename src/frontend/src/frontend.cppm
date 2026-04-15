module;

#include <filesystem>

export module frontend;

import thelast;
import options;
import ast_reader;
import llvm_ir_translator;

namespace ParaCL::frontend
{

export
void run(options::Options const & options)
{
    auto&& ast = grammar::read_ast(options);

    if (options.ast_dump)
    {
        auto&& ast_dot = options.input_file.filename();
        ast_dot.replace_extension(".ast.dot");
        auto&& ast_svg = std::filesystem::path{ast_dot};
        ast_svg.replace_extension(".svg");

        ast::dump(ast, ast_dot, ast_svg);
    }

    llvm_ir_translator::generate_llvm_ir(ast, options);
}

} /* namespace ParaCL::frontend */