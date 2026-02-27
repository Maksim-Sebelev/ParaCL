module;

#include <ostream>
#include <fstream>
#include <filesystem>
#include <utility>

export module ast_read;

export import ast;

namespace ParaCL::ast
{

export
template <typename... Signatures>
AST read_ast_txt(std::filesystem::path const & ast_txt)
{
    /* must call create */
    auto&& scope = node::Scope{};

    auto&& root = node::BasicNode::Actions<Signatures...>::template create<node::Scope>(scope);

    return AST(std::move(root));
}

namespace node
{


} /* namespace node */

} /* namespace ParaCL::ast */
