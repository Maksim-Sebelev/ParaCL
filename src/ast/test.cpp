#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <filesystem>
#include <fstream>

import ast;
import write_ast;

using namespace ::ParaCL::ast::node;
using namespace ::ParaCL::ast;

namespace ParaCL::ast::node::visit_overload_set
{
template <>
void visit([[maybe_unused]] Scan const & scan)
{ std::cout << "Scan" << std::endl; }

template <> 
void visit([[maybe_unused]] Print const & print)
{ std::cout << "Print" << std::endl; }

template <>
void visit([[maybe_unused]] Condition const & print)
{ std::cout << "Condition" << std::endl; }

template <>
void visit(Variable const & v)
{ std::cout << "Variable{" << v.name() << "}" << std::endl; }

template <>
void visit(NumberLiteral const & l)
{ std::cout << "NumberLiteral{" << l.value() << "}" << std::endl; }

template <>
void visit(StringLiteral const & l)
{ std::cout << "NumberLiteral{" << l.value() << "}" << std::endl; }

template <>
void visit([[maybe_unused]] If const & l)
{ std::cout << "If{"  << "}" << std::endl; }

template <>
void visit([[maybe_unused]] Else const & l)
{ std::cout << "Else{"  << "}" << std::endl; }

template <>
void visit(Scope const & s)
{ std::cout << "Scope{\n";  for (auto&& m: s) visit<void>(m); std::cout << "}" << std::endl; }
}

namespace ParaCL::ast::node::visit_overload_set
{
template <>
void visit([[maybe_unused]] Scan const & scan, int& i)
{ std::cout << "Scan " << ++i << std::endl; }

template <> 
void visit([[maybe_unused]] Print const & print, int& i)
{ std::cout << "Print " << ++i << std::endl; }

template <>
void visit([[maybe_unused]] Condition const & print, int& i)
{ std::cout << "Condition " << ++i << std::endl; }

template <>
void visit(Variable const & v, int& i)
{ std::cout << "Variable{" << v.name() << "} " << ++i << std::endl; }


template <>
void visit(If const & v, int& i)
{ std::cout << "If{" << "} " << ++i << std::endl; }


template <>
void visit(Else const & v, int& i)
{ std::cout << "Else{" << "} " << ++i << std::endl; }


template <>
void visit(Scope const & v, int& i)
{ std::cout << "Scope{" << "} " << ++i << std::endl; }



template <>
void visit(NumberLiteral const & v, int& i)
{ std::cout << "NumberLiteral{" << "} " << ++i << std::endl; }


template <>
void visit(StringLiteral const & v, int& i)
{ std::cout << "StringLiteral{" << "} " << ++i << std::endl; }
}


void print(BasicNode const & node)
{ return visit<void>(node); }


void print_and_count(BasicNode const & node, int & i)
{ return visit<void, int&>(node, i); }


int main() try
{
    auto&& n6 = BasicNode::create<NumberLiteral, void(), void(int&), writable>(NumberLiteral{13});
    auto&& n7 = BasicNode::create<StringLiteral, void(), void(int&), writable>(StringLiteral{"fuck me please"});

    int i = 666;
    auto&& n12 = BasicNode::create<Scan, void(), void(int&), writable>(Scan{});
    auto&& n22 = BasicNode::create<Print, void(), void(int&), writable>(Print{});
    auto&& n72 = BasicNode::create<If, void(), void(int&), writable>(If{{n6}, n22});
    auto&& n82 = BasicNode::create<Else, void(), void(int&), writable>(Else{n12});

    auto&& condition = Condition{{n72}, n82};
    auto&& n32 = BasicNode::create<Condition, void(), void(int&), writable>(std::move(condition));

    print_and_count(n12, i);
    print(n12);
    print_and_count(n22, i);
    print(n22);

    print_and_count(n32, i);
    print(n32);

    auto&& n42 = BasicNode{n32};
    print_and_count(n42, i);
    print(n42);

    auto&& n52 = BasicNode{std::move(n12)};
    print_and_count(n52, i);
    print(n52);

    auto&& n62 = BasicNode::create<Variable, void(), void(int&), writable>(Variable{"some name"});
    print_and_count(n62, i);
    print(n62);


    auto&& nast1 = BasicNode::create<Print, void(), void(int&), writable>(Print{n62, n6, n7});
    auto&& nast2 = n6;
    auto&& nast3 = n32;
    auto&& root = BasicNode::create<Scope, void(), void(int&), writable>(Scope{nast1, nast2, nast3});


    auto&& ast = AST{std::move(root)};
    write(ast, "ast.txt");

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
