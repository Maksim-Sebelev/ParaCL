module;

#include <cstddef>
#include <filesystem>
#include <ostream>
#include <chrono>
#include <fstream>
#include <string>

export module ast_serializer;

import thelast;

namespace test_generator::ast_serialize
{

enum class BuildProgramSetting
{
    DummyValue             ,
    LikeExpression         ,
    LikeStatement          ,
    NotFirstIf             ,
    ScopeLikeConditionBody ,
    GlobalScope            ,
};

void write_n_tab(std::ostream& os, size_t tabs)
{ os << std::string(tabs, '\t'); }

void new_line(std::ostream& os)
{ os << "\n"; }

void begin_scope(std::ostream& os, size_t enclosure)
{
    write_n_tab(os, enclosure);
    os << "{\n";
}

void leave_scope(std::ostream& os, size_t enclosure)
{
    write_n_tab(os, enclosure);
    os << "}\n";
}

void expression_or_statement_begin_action(std::ostream& os, size_t enclosure, BuildProgramSetting setting)
{
    auto&& is_expression = (setting == BuildProgramSetting::LikeExpression);
    if (is_expression)
        os << "(";
    else
        write_n_tab(os, enclosure);
}

void expression_or_statement_and_action(std::ostream& os, BuildProgramSetting setting)
{
    auto&& is_expression = (setting == BuildProgramSetting::LikeExpression);
    if (is_expression)
        os << ")";
    else
        os << ";\n";
}


void statement_end(std::ostream& os)
{ os << ";\n"; }


} /* test_generator::ast_serialize */



namespace last::node
{

export using serializable = void(std::ostream&, size_t, test_generator::ast_serialize::BuildProgramSetting);

auto build(BasicNode const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
    -> decltype(visit<void, std::ostream&, size_t, test_generator::ast_serialize::BuildProgramSetting>(node, os, enclosure, setting))
{
    return visit<void, std::ostream&, size_t, test_generator::ast_serialize::BuildProgramSetting>(node, os, enclosure, setting);
}

namespace visit_specializations
{

template <>
void visit(Print const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::ast_serialize::BuildProgramSetting)
{
    test_generator::ast_serialize::write_n_tab(os, enclosure);

    os << "print ";

    if (node.size() != 0)
    {
        build(node[0], os, 0, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
        for (auto&& it = 1LU, ite = node.size(); it < ite; ++it)
        {
            os << ", ";
            build(node[it], os, enclosure, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
        }
    }

    test_generator::ast_serialize::statement_end(os);
}

template <>
void visit([[maybe_unused]] Scan const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    expression_or_statement_begin_action(os, enclosure, setting);
    os << "?";
    expression_or_statement_and_action(os, setting);
}

template <>
void visit(Scope const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    auto&& size = node.size();

    auto&& is_scope_after_condition = (setting == test_generator::ast_serialize::BuildProgramSetting::ScopeLikeConditionBody);
    auto&& is_scope_global = (setting == test_generator::ast_serialize::BuildProgramSetting::GlobalScope);

    if ((size == 0) and is_scope_after_condition)
    {
        test_generator::ast_serialize::write_n_tab(os, enclosure);
        os << "{}\n";
        return;
    }

    auto&& need_brackets = not (((size == 1) and is_scope_after_condition) or (is_scope_global));
    auto&& offset = static_cast<size_t>(not is_scope_global);

    if (need_brackets)
        test_generator::ast_serialize::begin_scope(os, enclosure);

    for (auto&& arg: node)
        build(arg, os, enclosure + offset, test_generator::ast_serialize::BuildProgramSetting::LikeStatement);

    if (need_brackets)
        test_generator::ast_serialize::leave_scope(os, enclosure);
}

template <>
void visit(While const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::ast_serialize::BuildProgramSetting)
{
    test_generator::ast_serialize::write_n_tab(os, enclosure);
    os << "while (";
    build(node.condition(), os, 0, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
    os << ")\n";
    build(node.body(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(If const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    test_generator::ast_serialize::write_n_tab(os, enclosure);
    if (setting == test_generator::ast_serialize::BuildProgramSetting::NotFirstIf)
        os << "else ";

    os << "if (";
    build(node.condition(), os, 0, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
    os << ")\n";
    build(node.body(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(Else const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::ast_serialize::BuildProgramSetting)
{
    test_generator::ast_serialize::write_n_tab(os, enclosure);
    os << "else\n";
    build(node.body(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(Condition const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::ast_serialize::BuildProgramSetting setting)
{
    if (node.get_ifs().size() != 0)
    {
        auto&& ifs = node.get_ifs();
        build(ifs[0], os, enclosure, test_generator::ast_serialize::BuildProgramSetting::DummyValue);
        for (auto&& it = 1LU, ite = ifs.size(); it < ite; ++it)
            build(ifs[it], os, enclosure, test_generator::ast_serialize::BuildProgramSetting::NotFirstIf);
    }

    if (node.has_else())
        build(node.get_else(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::DummyValue);
}

template <>
void visit(BinaryOperator const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    expression_or_statement_begin_action(os, enclosure, setting);

    build(node.larg(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
    switch (node.type())
    {
        case BinaryOperator::AND    : os << " && "; break;
        case BinaryOperator::OR     : os << " || "; break;
        case BinaryOperator::ADD    : os << " + " ; break;
        case BinaryOperator::SUB    : os << " - " ; break;
        case BinaryOperator::MUL    : os << " * " ; break;
        case BinaryOperator::DIV    : os << " / " ; break;
        case BinaryOperator::REM    : os << " % " ; break;
        case BinaryOperator::ISAB   : os << " > " ; break;
        case BinaryOperator::ISABE  : os << " >= "; break;
        case BinaryOperator::ISLS   : os << " < " ; break;
        case BinaryOperator::ISLSE  : os << " <= "; break;
        case BinaryOperator::ISEQ   : os << " == "; break;
        case BinaryOperator::ISNE   : os << " != "; break;
        case BinaryOperator::ASGN   : os << " = " ; break;
        case BinaryOperator::ADDASGN: os << " += "; break;
        case BinaryOperator::SUBASGN: os << " -= "; break;
        case BinaryOperator::MULASGN: os << " *= "; break;
        case BinaryOperator::DIVASGN: os << " /= "; break;
        case BinaryOperator::REMASGN: os << " %= "; break;
        default: __builtin_unreachable();
    }
    build(node.rarg(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);

    expression_or_statement_and_action(os, setting);
}

template <>
void visit(UnaryOperator const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    expression_or_statement_begin_action(os, enclosure, setting);

    switch (node.type())
    {
        case UnaryOperator::PLUS : os << "+"; break;
        case UnaryOperator::MINUS: os << "-"; break;
        case UnaryOperator::NOT  : os << "!"; break;
        default: __builtin_unreachable();
    }

    build(node.arg(), os, enclosure, test_generator::ast_serialize::BuildProgramSetting::LikeExpression);

    expression_or_statement_and_action(os, setting);
}

template <>
void visit(Variable const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    auto&& is_not_expression = (setting != test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
    if (is_not_expression)
        test_generator::ast_serialize::write_n_tab(os, enclosure);

    os << node.name();

    if (is_not_expression)
        test_generator::ast_serialize::statement_end(os);
}

template <>
void visit(NumberLiteral const & node, std::ostream& os, size_t enclosure, test_generator::ast_serialize::BuildProgramSetting setting)
{
    auto&& is_not_expression = (setting != test_generator::ast_serialize::BuildProgramSetting::LikeExpression);
    if (is_not_expression)
        test_generator::ast_serialize::write_n_tab(os, enclosure);

    os << node.value();

    if (is_not_expression)
        test_generator::ast_serialize::statement_end(os);
}

template <>
void visit(StringLiteral const & node, std::ostream& os, [[maybe_unused]] size_t enclosure, [[maybe_unused]] test_generator::ast_serialize::BuildProgramSetting)
{
    os << "\"" << node.value() << "\"";
}

} /* namespace visit_specializations */
} /* namespace last::node */


namespace test_generator::ast_serialize
{
export
void serialize(last::AST const & ast, std::filesystem::path const & ouput_file)
{
    auto&& ofs = std::ofstream{ouput_file};
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_local = *std::localtime(&t_c);

    ofs << "// Automatic generated with '" << __PRETTY_FUNCTION__ << "'\n"
        << "// " << std::put_time(&tm_local, "%Y-%m-%d %H:%M:%S") << "\n\n";


    last::node::build(ast.root(), ofs, 0, test_generator::ast_serialize::BuildProgramSetting::GlobalScope);
}

} /* namespace test_generator::ast_serialize */
