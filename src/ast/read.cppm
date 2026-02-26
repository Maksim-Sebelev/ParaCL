module;

#include <fstream>
#include <filesystem>

export module ast_read;

export import ast;

namespace ParaCL::ast
{



export
template <typename... Signatures>
AST read_ast_txt(std::filesystem::path const & ast_txt)
{
    /* must call create_personall */
    auto&& scope = node::Scope{};

    auto&& root = node::BasicNode::create<node::Scope, Signatures...>(scope);
    return AST(std::move(root));
}

} /* namespace ParaCL::ast */
