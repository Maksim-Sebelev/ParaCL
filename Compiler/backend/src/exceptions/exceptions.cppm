module;

#include <vector>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <cassert>

#include <llvm/IR/Function.h>

export module compiler_exceptions;

export import thelast;


namespace compiler::exceptions
{

std::string to_string(last::node::CodeLocation const & location)
{
    auto&& loc = std::ostringstream{};
    loc << location.file() << ":" << location.line_begin() << ":" << location.column_begin();
    return loc.str();
}

std::string mark_error(last::node::CodeLocation const & location)
{
    auto&& string_begin_offset = location.column_begin() - 1; assert(location.column_begin() > 0);
    auto&& token_length = (location.line_begin() == location.line_begin()) ?
        (location.column_end() - location.column_begin()) :
        (location.code_excerpt().length() - location.column_begin()); assert(location.code_excerpt().length() >= location.column_begin());

    auto&& mark = std::string(string_begin_offset, ' ');
    mark += "^";
    if (token_length > 1)
        mark += std::string(token_length - 1, '~');

    return mark;
}

std::string show_error(std::string msg, last::node::CodeLocation const & location)
{
    auto&& explain = to_string(location);
    explain += ": ";
    explain += msg;
    explain += "\n";
    auto&& indent = " " + std::to_string(location.line_begin()) + " | ";
    auto&& indent_length = indent.length();
    explain += indent;
    explain += location.code_excerpt();
    explain += "\n";
    explain += std::string(indent_length - 2, ' ') + "| ";
    explain += mark_error(location);

    return explain;
}

class error : public std::exception
{
protected: std::string msg_;
public:
    const char* what() const noexcept override
    { return msg_.c_str(); }
};

export
class using_undeclarated_variable_error : public error
{
public:
    using_undeclarated_variable_error(last::node::Variable const & node)
    {
        auto&& explain = "Using undeclarated variable: '" + std::string(node.name()) + "'";
        msg_ = show_error(explain, node.location());
    }
};

export
class function_alias_arguments_mismatch_error : public error
{
public:
    function_alias_arguments_mismatch_error(last::node::FunctionCall const & node, llvm::Function const * function, std::vector<llvm::Value*> const & completed_args)
    {
        auto&& explain = "Function alias signature mismatch:\n'" + std::string(node.name()) +
                                 "' expects " + std::to_string(function->arg_size()) +
                                 " arg" + ((function->arg_size() != 1) ? "s" : "") +
                                 ", but called with " + std::to_string(completed_args.size()) + ":";
        msg_ = show_error(explain, node.location());
    }
};

export
class using_int_as_function_error : public error
{
public:
    using_int_as_function_error(last::node::FunctionCall const & node)
    {
        auto&& explain = "Using '" + std::string(node.name()) + "' as function, but it has `int` type.";
        msg_ = show_error(explain, node.location());
    }
};

export
class using_undeclarated_function : public error
{
public:
    using_undeclarated_function(last::node::FunctionCall const & node)
    {
        auto&& explain = "Call undeclarated function '" + std::string(node.name()) + "'";
        msg_ = show_error(explain, node.location());
    }
};

export
class using_variable_from_parent_function_scope_error : public error
{
public:
    using_variable_from_parent_function_scope_error(last::node::Variable const & node)
    {
        auto&& explain = "'" + std::string(node.name()) + "' was declared in parent scope, but isn`t global variable (belong to the parent function scope)";
        msg_ = show_error(explain, node.location());
    }
};


} /* namespace compiler::exceptions */
