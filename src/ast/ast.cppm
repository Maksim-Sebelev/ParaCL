module;

//--------------------------------------------------------------------------------------------------------------------------------------

#include <concepts>
#include <stdexcept>
#include <memory>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

//--------------------------------------------------------------------------------------------------------------------------------------

export module ast;

//--------------------------------------------------------------------------------------------------------------------------------------

export import node_type_erasure;
export import ast_nodes;

//--------------------------------------------------------------------------------------------------------------------------------------

namespace ParaCL::ast
{

//--------------------------------------------------------------------------------------------------------------------------------------

export
class AST
{
private:
    node::BasicNode root_;

public:
    AST(node::BasicNode const & root) : /* too long, because all tree will copy */
        root_(root)
    {}

    AST(node::BasicNode&& root) :
        root_(std::move(root))
    {}

public:
    node::BasicNode const &root() const noexcept
    { return root_; }

};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::ast */
//--------------------------------------------------------------------------------------------------------------------------------------
