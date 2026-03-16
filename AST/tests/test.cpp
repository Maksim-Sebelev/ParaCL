#include <ios>
#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <filesystem>
#include <fstream>
#include <utility>
#include <boost/json.hpp>
#include <boost/json/object.hpp>

import thelast;


#include "create-basic-node.hpp"


using namespace ::last::node;
using namespace ::last;

CREATE_SAME(writable, dumpable, serializable)

#include "read-ast.hpp"


int main() try
{
    auto&& n_scan = create(Scan{});
    auto&& n_string = create(StringLiteral("fuck"));
    auto&& n_number = create(NumberLiteral(666));
    auto&& n_variable = create(Variable("pretty_name"));
    auto&& n_print = create(Print{n_string, n_number, n_scan, n_scan});
    auto&& n_unop = create(UnaryOperator{UnaryOperator::NOT, n_variable});
    auto&& n_binop3 = create(BinaryOperator{BinaryOperator::SUB, n_number, n_unop});
    auto&& n_binop2 = create(BinaryOperator{BinaryOperator::AND, n_binop3, n_variable});
    auto&& n_binop = create(BinaryOperator{BinaryOperator::ASGN, n_variable, n_binop2});

    auto&& n_scope2 = create(Scope{n_print, n_unop, n_number, n_binop});
    auto&& n_scope3 = create(Scope{n_print, n_scan, n_print});
    auto&& n_scope4 = create(Scope{});

    auto&& n_return2 = create(Return{n_binop});
    auto&& n_scope6 = create(Scope{n_return2});
    auto&& n_declfunc = create(FunctionDeclaration{"foo", {}, n_scope6});
    auto&& n_var1 = create(Variable{"x"});
    auto&& n_var2 = create(Variable{"y"});
    auto&& n_var3 = create(Variable{"z"});
    auto&& n_call_func = create(FunctionCall("foo", {n_var1, n_var2, n_var3}));
    auto&& n_if = create(If{n_scan, n_scope2});
    auto&& n_if2 = create(If{n_scan, n_scope3});
    auto&& n_else = create(Else{n_scope4});
    auto&& n_condition = create(Condition{{n_if, n_if2}, n_else});
    auto&& n_scope5 = create(Scope{n_number});
    auto&& n_while2 = create(While{n_scan, n_scope5});
    auto&& n_return = create(Return{n_scan});
    auto&& n_scope = create(Scope{n_scan, n_condition, n_print, n_while2, n_return});
    auto&& n_while = create(While{n_call_func, n_scope});
    auto&& n_global_scope = create(Scope{n_declfunc, n_while});
    auto&& ast = AST{std::move(n_global_scope)};
    write(ast, "serializable-ast.ast.json");
    build_program(ast, "serializable-ast.cl");
    dump(ast, "ast.dot", "ast.svg");

    auto&& ast2 = read("serializable-ast.ast.json");
    build_program(ast2, "2.cl");

    return 0;
}
catch (const std::runtime_error& e)
{
    std::cerr << "runtime error: " << e.what() << "\n";
    return 1;
}
catch (const std::bad_cast& e)
{
    std::cerr << "bad cast error: " << e.what() << "\n";
    return 1;
}
catch (...)
{
    std::cerr << "excepption catched\n";
    return 1;
}
