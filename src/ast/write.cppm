module;

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

export module write_ast;

export import ast;

namespace ParaCL::node
{

void write(BasicNode const & node, std::ofstream& os, size_t enclosure)
{
    return visit<void, std::ofstream&, size_t>(node, os, enclosure);
}

export
using writable = void(std::ofstream&, size_t);

namespace write_in_file
{


void n_tab(std::ofstream& os, size_t tabs)
{ os << std::string(tabs, '\t'); }

} /* namespace write_in_file */

}

export
namespace ParaCL::ast
{
void write(AST const & ast, std::filesystem::path const &file)
{
    std::ofstream ofs{file};
    ParaCL::node::write(ast.root(), ofs, 0LU);
}
} /* namespace ParaCL::ast */

namespace ParaCL::node::visit_overload_set
{

template <>
void visit(Print const& print, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Print {\n";

    for (auto&& arg: print)
        write(arg, os, enclosure + 1);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit([[maybe_unused]] Scan const& scan, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Scan {}\n";
}

template <>
void visit(Scope const& scope, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Scope {\n";

    for (auto&& arg: scope)
        write(arg, os, enclosure + 1);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Variable const& variable, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Variable {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "name: \"" << variable.name() << "\"\n";
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(NumberLiteral const& number, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "NumberLiteral {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "value: " << number.value() << "\n";
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(StringLiteral const& string_lit, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "StringLiteral {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "value: \"" << string_lit.value() << "\"\n";
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(UnaryOperator const& unary_op, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "UnaryOperator {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "type: ";
    switch (unary_op.type())
    {
        case UnaryOperator::MINUS: os << "MINUS"; break;
        case UnaryOperator::PLUS:  os << "PLUS"; break;
        case UnaryOperator::NOT:   os << "NOT"; break;
    }
    os << "\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << "arg:\n";
    write(unary_op.arg(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(BinaryOperator const& bin_op, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "BinaryOperator {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << "type: ";
    switch (bin_op.type()) {
        case BinaryOperator::AND:     os << ""; break;
        case BinaryOperator::OR:      os << "OR"; break;
        case BinaryOperator::NOT:     os << "NOT"; break;
        case BinaryOperator::ADD:     os << "ADD"; break;
        case BinaryOperator::SUB:     os << "SUB"; break;
        case BinaryOperator::MUL:     os << "MUL"; break;
        case BinaryOperator::DIV:     os << "DIV"; break;
        case BinaryOperator::REM:     os << "REM"; break;
        case BinaryOperator::ISAB:    os << "ISAB"; break;
        case BinaryOperator::ISABE:   os << "ISABE"; break;
        case BinaryOperator::ISLS:    os << "ISLS"; break;
        case BinaryOperator::ISLSE:   os << "ISLSE"; break;
        case BinaryOperator::ISEQ:    os << "ISEQ"; break;
        case BinaryOperator::ISNE:    os << "ISNE"; break;
        case BinaryOperator::ASGN:    os << "ASGN"; break;
        case BinaryOperator::ADDASGN: os << "ADDASGN"; break;
        case BinaryOperator::SUBASGN: os << "SUBASGN"; break;
        case BinaryOperator::MULASGN: os << "MULASGN"; break;
        case BinaryOperator::DIVASGN: os << "DIVASGN"; break;
        case BinaryOperator::REMASGN: os << "REMASGN"; break;
    }
    os << "\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "left:\n";
    write(bin_op.larg(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "right:\n";
    write(bin_op.rarg(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(While const& while_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "While {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << "condition:\n";
    write(while_node.condition(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure + 1);
    os << "body:\n";
    write(while_node.body(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(If const& if_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "If {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "condition:\n";
    write(if_node.condition(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure + 1);
    os << "body:\n";
    write(if_node.body(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Else const& else_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Else {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << "body:\n";
    write(else_node.boby(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Condition const& condition, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << "Condition {\n";
    
    for (auto&& if_node : condition.get_ifs())
        write(if_node, os, enclosure + 1);

    write(condition.get_else(), os, enclosure + 1);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

} /* namespace ParaCL::node::write_overload_set */
