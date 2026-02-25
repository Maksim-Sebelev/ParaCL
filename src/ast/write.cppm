module;

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

export module ast_write;

export import ast;
import paracl_info;
import node_traits;

namespace ParaCL::ast::node
{

export
using writable = void(std::ofstream&, size_t);

void write(BasicNode const & node, std::ofstream& os, size_t enclosure)
{ return visit<void, std::ofstream&, size_t>(node, os, enclosure); }

namespace write_in_file
{

void n_tab(std::ofstream& os, size_t tabs)
{ os << std::string(tabs, '\t'); }

} /* namespace write_in_file */
} /* namespace ParaCL::ast::node */

namespace ParaCL::ast
{

export
void write(AST const & ast, std::filesystem::path const &file)
{
    std::ofstream ofs{file};
    ofs << ParaCLInfo::queryParaCLInfo().ast_text_representation_signature;
    ParaCL::ast::node::write(ast.root(), ofs, 0LU);
}
} /* namespace ParaCL::ast */

namespace ParaCL::ast::node::visit_overload_set
{

template <>
void visit(Print const& print, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Print, traits::NAME>() << " {\n";

    for (auto&& arg: print)
        write(arg, os, enclosure + 1);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit([[maybe_unused]] Scan const& scan, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Scan, traits::NAME>() << " {}\n";
}

template <>
void visit(Scope const& scope, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Scope, traits::NAME>() << " {\n";

    for (auto&& arg: scope)
        write(arg, os, enclosure + 1);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Variable const& variable, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Variable, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<Variable, traits::FIELD, 0>() << ": \"" << variable.name() << "\"\n";

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(NumberLiteral const& number, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<NumberLiteral, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<NumberLiteral, traits::FIELD, 0>() << ": " << number.value() << "\n";

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(StringLiteral const& string_lit, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<StringLiteral, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<StringLiteral, traits::FIELD, 0>() << ": \"" << string_lit.value() << "\"\n";

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(UnaryOperator const& unary_op, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<UnaryOperator, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<UnaryOperator, traits::FIELD, 0>() << ": ";

    switch (unary_op.type())
    {
        case UnaryOperator::MINUS: os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::MINUS>(); break;
        case UnaryOperator::PLUS:  os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::PLUS>();  break;
        case UnaryOperator::NOT:   os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::NOT>();   break;
        default: __builtin_unreachable();
    }
    os << "\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<UnaryOperator, traits::FIELD, 1>() << ":\n";
    write(unary_op.arg(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(BinaryOperator const& bin_op, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<BinaryOperator, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<BinaryOperator, traits::FIELD, 0>() << ": ";

    switch (bin_op.type())
    {
        case BinaryOperator::AND:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::AND>(); break;
        case BinaryOperator::OR:      os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::OR>(); break;
        case BinaryOperator::ADD:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ADD>(); break;
        case BinaryOperator::SUB:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::SUB>(); break;
        case BinaryOperator::MUL:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::MUL>(); break;
        case BinaryOperator::DIV:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::DIV>(); break;
        case BinaryOperator::REM:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::REM>(); break;
        case BinaryOperator::ISAB:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISAB>(); break;
        case BinaryOperator::ISABE:   os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISABE>(); break;
        case BinaryOperator::ISLS:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISLS>(); break;
        case BinaryOperator::ISLSE:   os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISLSE>(); break;
        case BinaryOperator::ISEQ:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISEQ>(); break;
        case BinaryOperator::ISNE:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISNE>(); break;
        case BinaryOperator::ASGN:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ASGN>(); break;
        case BinaryOperator::ADDASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ADDASGN>(); break;
        case BinaryOperator::SUBASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::SUBASGN>(); break;
        case BinaryOperator::MULASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::MULASGN>(); break;
        case BinaryOperator::DIVASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::DIVASGN>(); break;
        case BinaryOperator::REMASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::REMASGN>(); break;
        default: __builtin_unreachable();
    }
    os << "\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<BinaryOperator, traits::FIELD, 1>() << ":\n";
    write(bin_op.larg(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<BinaryOperator, traits::FIELD, 2>() << ":\n";
    write(bin_op.rarg(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(While const& while_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<While, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<While, traits::FIELD, 0>() << ":\n";
    write(while_node.condition(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<While, traits::FIELD, 1>() << ":\n";
    write(while_node.body(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(If const& if_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<If, traits::NAME>() << " {\n";
    
    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<If, traits::FIELD, 0>() << ":\n";
    write(if_node.condition(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<If, traits::FIELD, 1>() << ":\n";
    write(if_node.body(), os, enclosure + 2);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Else const& else_node, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Else, traits::NAME>() << " {\n";

    write_in_file::n_tab(os, enclosure + 1);
    os << traits::get_node_info<Else, traits::FIELD, 0>() << ":\n";
    write(else_node.body(), os, enclosure + 2);

    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

template <>
void visit(Condition const& condition, std::ofstream& os, size_t enclosure)
{
    write_in_file::n_tab(os, enclosure);
    os << traits::get_node_info<Condition, traits::NAME>() << " {\n";
    
    for (auto&& if_node : condition.get_ifs())
        write(if_node, os, enclosure + 1);

    write(condition.get_else(), os, enclosure + 1);
    
    write_in_file::n_tab(os, enclosure);
    os << "}\n";
}

} /* namespace ParaCL::ast::node::write_overload_set */
