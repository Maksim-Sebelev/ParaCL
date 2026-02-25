module;

#include <fstream>
#include <filesystem>

export module ast_read;

export import ast;

namespace ParaCL::ast
{

void check_file_signature()

export
template <typename... Signatures>
AST read(std::filesystem::path const & ast_txt)
{
    auto&& scope = node::Scope{};

    auto&& root = node::BasicNode::create<node::Scope, Signatures...>(scope);
    return AST(std::move(root));
}



} /* namespace ParaCL::ast */
