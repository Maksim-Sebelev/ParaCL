module;

#include <memory>

export module ast;

export import node_type_erasure;
export import ast_nodes;

namespace last
{


export
class AST
{
    node::BasicNode root_;

public:
    AST(node::BasicNode const & root) : root_(root) {}
    
    AST() = default;

    AST(node::BasicNode&& root) :
        root_(std::move(root))
    {}

    node::BasicNode const &root() const noexcept { return root_; }
};

} /* namespace last */
