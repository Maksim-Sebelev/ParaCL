#pragma once

#define SET_NODE_FUNCTIONALITY(NodeT, ...)                                                                                                           \
template <>                                                                                                                                          \
inline ParaCL::frontend::ast::node::BasicNode ParaCL::frontend::ast::node::create(NodeT node)                                                                  \
{                                                                                                                                                    \
    return ParaCL::frontend::ast::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node));                                                    \
}

#define SET_ALL_NODE_SAME_FUNCTIONALITY(...)                                                                                                         \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Scope              , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Print              , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Scan               , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::UnaryOperator      , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::BinaryOperator     , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::If                 , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Else               , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Condition          , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::While              , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::NumberLiteral      , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::StringLiteral      , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Variable           , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::Return             , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::FunctionCall       , __VA_ARGS__)                                                                \
SET_NODE_FUNCTIONALITY(ParaCL::frontend::ast::node::FunctionDeclaration, __VA_ARGS__)
