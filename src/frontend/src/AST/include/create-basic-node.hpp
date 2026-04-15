#pragma once

#define SPECIALIZE_CREATE(NodeT, ...)                                                                                       \
template <>                                                                                                                 \
inline ParaCL::ast::node::BasicNode ParaCL::ast::node::create(NodeT node)                                                                 \
{                                                                                                                           \
    return ParaCL::ast::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node));                                            \
}

#define CREATE_SAME(...)                                                                                                       \
SPECIALIZE_CREATE(ParaCL::ast::node::Scope              , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Print              , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Scan               , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::UnaryOperator      , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::BinaryOperator     , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::If                 , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Else               , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Condition          , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::While              , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::NumberLiteral      , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::StringLiteral      , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Variable           , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::Return             , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::FunctionCall       , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(ParaCL::ast::node::FunctionDeclaration, __VA_ARGS__)
