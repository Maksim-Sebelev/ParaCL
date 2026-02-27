module;

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <type_traits>
#include "log/log_api.hpp"

export module interpreter;

import interpreter_name_table;
// export import interpreter_optoins;

export import ast;
import ast_read;

namespace ParaCL::ast::node
{

using executable_expression = int(backend::interpreter::nametable::Nametable&);
using executable_statement = void(backend::interpreter::nametable::Nametable&);
using printable_string = std::string_view();

static_assert(not std::is_same_v<executable_expression, executable_statement>, "visit specializations must be different");
static_assert(not std::is_same_v<executable_expression, printable_string>    , "visit specializations must be different");
static_assert(not std::is_same_v<executable_statement, printable_string>     , "visit specializations must be different");

void 

} /* namespace ParaCL::ast::node */

namespace ParaCL::ast::node::visit_specializations
{

//-----------------------------------------------------------------------------
// PRINT
//-----------------------------------------------------------------------------
template <>
void visit(Print const& print, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute print statement");

    for (auto&& arg : print)
    {
        if (support<printable_string>(arg))
            std::cout << visit<printable_string>(arg);
        else
            std::cout << visit<executable_expression>(arg, nametable);
    }

    std::cout << std::endl;
}

//-----------------------------------------------------------------------------
// SCAN
//-----------------------------------------------------------------------------
template <>
int visit(Scan const& scan, backend::interpreter::nametable::Nametable& nametable)
{
    int value;
    std::cin >> value;
    LOGINFO("paracl: interpreter: scan value: {}", value);
    return value;
}

//-----------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------
template <>
int visit(Variable const& var, backend::interpreter::nametable::Nametable& nametable)
{
    auto&& var_name = var.name();
    auto&& value = nametable.get_variable_value(var_name);
    
    LOGINFO("paracl: interpreter: get variable '{}' = {}", var_name, value.value());
    return value.value();
}

//-----------------------------------------------------------------------------
// NUMBER LITERAL
//-----------------------------------------------------------------------------
template <>
int visit(NumberLiteral const& num, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: number literal: {}", num.value());
    return num.value();
}

//-----------------------------------------------------------------------------
// STRING LITERAL
//-----------------------------------------------------------------------------
template <>
std::string_view visit(StringLiteral const& str)
{
    LOGINFO("paracl: interpreter: string literal: \"{}\"", str.value());
    return str.value();
}

//-----------------------------------------------------------------------------
// UNARY OPERATOR
//-----------------------------------------------------------------------------
template <>
int visit(UnaryOperator const& unary, backend::interpreter::nametable::Nametable& nametable)
{
    auto&& arg_value = visit<executable_expression>(unary.arg(), nametable);
    
    decltype(arg_value) result;
    switch (unary.type())
    {
        case UnaryOperator::MINUS: result = -arg_value; break;
        case UnaryOperator::PLUS:  result = +arg_value; break;
        case UnaryOperator::NOT:   result = !arg_value; break;
        default: __builtin_unreachable();
    }
    
    LOGINFO("paracl: interpreter: unary operation result: {}", result);
    return result;
}

//-----------------------------------------------------------------------------
// BINARY OPERATOR
//-----------------------------------------------------------------------------
template <>
int visit(BinaryOperator const& bin, backend::interpreter::nametable::Nametable& nametable)
{
    auto&& left = visit<executable_expression>(bin.larg(), nametable);
    auto&& right = visit<executable_expression>(bin.rarg(), nametable);

    decltype(left) result;

    switch (bin.type())
    {
        case BinaryOperator::AND:     result = left && right; break;
        case BinaryOperator::OR:      result = left || right; break;
        case BinaryOperator::ADD:     result = left + right; break;
        case BinaryOperator::SUB:     result = left - right; break;
        case BinaryOperator::MUL:     result = left * right; break;
        case BinaryOperator::DIV:     result = left / right; break;
        case BinaryOperator::REM:     result = left % right; break;
        case BinaryOperator::ISAB:    result = left > right; break;
        case BinaryOperator::ISABE:   result = left >= right; break;
        case BinaryOperator::ISLS:    result = left < right; break;
        case BinaryOperator::ISLSE:   result = left <= right; break;
        case BinaryOperator::ISEQ:    result = left == right; break;
        case BinaryOperator::ISNE:    result = left != right; break;
        case BinaryOperator::ASGN:
        case BinaryOperator::ADDASGN:
        case BinaryOperator::SUBASGN:
        case BinaryOperator::MULASGN:
        case BinaryOperator::DIVASGN:
        case BinaryOperator::REMASGN:
            throw std::runtime_error("Assignment operators should be handled by statement nodes");
        default: __builtin_unreachable();
    }

    LOGINFO("paracl: interpreter: binary operation result: {}", result);
    return result;
}

//-----------------------------------------------------------------------------
// WHILE
//-----------------------------------------------------------------------------
template <>
void visit(While const& while_node, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");
    
    while (visit<executable_expression>(while_node.condition(), nametable))
        visit<executable_statement>(while_node.body(), nametable);
}

//-----------------------------------------------------------------------------
// IF
//-----------------------------------------------------------------------------
template <>
void visit(If const& if_node, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute IF statement");
    
    if (visit<executable_expression>(if_node.condition(), nametable)) return;

    visit<executable_statement>(if_node.body(), nametable);
}

//-----------------------------------------------------------------------------
// ELSE
//-----------------------------------------------------------------------------
template <>
void visit(Else const& else_node, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute ELSE statement");
    visit<executable_statement>(else_node.body(), nametable);
}

//-----------------------------------------------------------------------------
// CONDITION
//-----------------------------------------------------------------------------
template <>
void visit(Condition const& condition, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute CONDITION statement");
    
    // Проверяем все if-ы по порядку
    for (const auto& if_node : condition.get_ifs())
    {
        if (not visit<executable_expression>(if_node.condition(), nametable)) continue;

        return visit<executable_statement>(if_node.body(), nametable);
    }

    return visit<executable_statement>(condition.get_else(), nametable);
}

//-----------------------------------------------------------------------------
// SCOPE
//-----------------------------------------------------------------------------
template <>
void visit(Scope const& scope, backend::interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: enter scope");
    nametable.new_scope();
    
    for (const auto& stmt : scope)
        visit<executable_statement>(stmt, nametable);
    
    nametable.leave_scope();
    LOGINFO("paracl: interpreter: leave scope");
}

} /* namespace ParaCL::ast::node::visit_specializations */

namespace ParaCL::backend::interpreter
{

export
void interpret(std::filesystem::path const & ast_txt)
{
    LOGINFO("paracl: interpreter: start");

    auto&& ast = ast::read(ast_txt);

    nametable::Nametable nametable;
    nametable.new_scope(); /* global scope */

    ast::node::visit<node::executable_statement>(ast.root(), nametable);    

    LOGINFO("paracl: interpreter: completed");
}

} /* namespace ParaCL::backend::interpreter */

//-----------------------------------------------------------------------------

namespace ParaCL::ast::node
{

template <>
BasicNode create<Print>(Print node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

template <>
BasicNode create<Scan>(Scan node)
{
    return BasicNode::Actions<executable_expression, executable_statement>::create(std::move(node));
}

template <>
BasicNode create<Variable>(Variable node)
{
    return BasicNode::Actions<executable_expression, executable_statement>::create(std::move(node));
}

template <>
BasicNode create<NumberLiteral>(NumberLiteral node)
{
    return BasicNode::Actions<executable_expression, executable_statement>::create(std::move(node));
}

template <>
BasicNode create<StringLiteral>(StringLiteral node)
{
    return BasicNode::Actions<printable_string>::create(std::move(node));
}

template <>
BasicNode create<UnaryOperator>(UnaryOperator node)
{
    return BasicNode::Actions<executable_expression>::create(std::move(node));
}

template <>
BasicNode create<BinaryOperator>(BinaryOperator node)
{
    return BasicNode::Actions<executable_expression>::create(std::move(node));
}

template <>
BasicNode create<While>(While node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

template <>
BasicNode create<If>(If node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

template <>
BasicNode create<Else>(Else node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

template <>
BasicNode create<Condition>(Condition node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

template <>
BasicNode create<Scope>(Scope node)
{
    return BasicNode::Actions<executable_statement>::create(std::move(node));
}

//-----------------------------------------------------------------------------
} /* namespace ParaCL::ast::node */
//-----------------------------------------------------------------------------
