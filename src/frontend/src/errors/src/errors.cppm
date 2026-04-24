module;

#include <cstddef>
#include <ostream>
#include <sstream>
#include <exception>
#include <string>
#include <string_view>
#include <cassert>
#include <iostream>

#define RED "\e[0;31m"
#define VIOLET "\e[0;35m"
#define GREEN "\e[0;32m"

#define BOLD "\033[1m"

#define WARNING VIOLET BOLD
#define ERROR   RED BOLD

#define RESET "\e[0m"

export module frontend_errors;

export import thelast;

namespace ParaCL::frontend
{

std::string to_string(frontend::ast::node::CodeLocation const & location)
{
    auto&& loc = std::ostringstream{};
    loc << location.file() << ":" << location.line_begin() << ":" << location.column_begin();
    return loc.str();
}

std::string mark_error(frontend::ast::node::CodeLocation const & location)
{
    auto&& mark = std::ostringstream{};

    // for (auto&& it = location.line_begin(), ite = location.line_end(); it != ite; ++it)
    // {
    auto&& string_begin_offset = location.column_begin() >= 1 ? location.column_begin() - 1 : 1;
    auto&& token_length = (location.line_begin() == location.line_end()) ?
        (location.column_end() - location.column_begin()) :
        (location.code_excerpt().length() - location.column_begin());

    mark << GREEN BOLD << std::string(string_begin_offset, ' ') << "^";

    if (token_length > 1)
        mark << std::string(token_length - 1, '~');

    mark << RESET;
    // }

    return mark.str();
}

enum class ProblemStatus
{
    Warning, Error
};

std::string show_error(std::string_view msg, ProblemStatus status = ProblemStatus::Error)
{
    auto&& explain = std::ostringstream{};
    explain << BOLD "paracl: " RESET << ((status == ProblemStatus::Error) ? ERROR "error:" : WARNING "warning:") << " " RESET BOLD << msg << RESET "\n";

    return explain.str();
}

std::string show_code(frontend::ast::node::CodeLocation const & location)
{
    auto&& code = std::ostringstream{};
    auto&& indent = " " + std::to_string(location.line_begin()) + " | ";
    auto&& indent_length = indent.length(); assert(indent_length >= 3);
    code << indent << location.code_excerpt() << "\n" << std::string(indent_length - 2, ' ') << "| " << mark_error(location);
    return code.str();
}

std::string show_code_error(std::string_view msg, frontend::ast::node::CodeLocation const & location, ProblemStatus status = ProblemStatus::Error)
{
    auto&& explain = std::ostringstream{};
    explain << BOLD << to_string(location)
        << ": " RESET
        << ((status == ProblemStatus::Error) ? ERROR "error:" : WARNING "warning:")
        << " " RESET BOLD << msg << RESET "\n"
        << show_code(location);

    return explain.str();
}

export
namespace error
{

class error : public std::exception
{
protected: std::string msg_;
private:
    const char* what_() const noexcept
    { return msg_.c_str(); }

public:
    const char* what() const noexcept override
    { return what_(); }
};

std::ostream& operator << (std::ostream& os, error const & e)
{ return os << e.what(); }

class no_such_file_error : public error
{
public:
    no_such_file_error(std::string_view file)
    {
        auto&& explain = "no such file: '"+std::string(file)+"'";
        msg_ = show_error(explain);
    }
};

class parser_error : public error
{
public:
    parser_error(frontend::ast::node::CodeLocation const & location, std::string_view explain)
    {
        msg_ = show_code_error(explain, location);
    }
};

class using_undeclarated_variable_error : public error
{
public:
    using_undeclarated_variable_error(frontend::ast::node::Variable const & node)
    {
        auto&& explain = "use of undeclared identifier '" + std::string(node.name()) + "'";
        msg_ = show_code_error(explain, node.location());
    }
};

class function_alias_arguments_mismatch_error : public error
{
public:
    function_alias_arguments_mismatch_error(frontend::ast::node::FunctionCall const & node, size_t expected_args_size, size_t call_args_size)
    {
        auto&& explain = "function alias signature mismatch, '" + std::string(node.name()) +
                                 "' expects " + std::to_string(expected_args_size) +
                                 " arg" + ((expected_args_size != 1) ? "s" : "") +
                                 ", but called with " + std::to_string(call_args_size) + ":";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_int_as_function_error : public error
{
public:
    using_int_as_function_error(frontend::ast::node::FunctionCall const & node)
    {
        auto&& explain = "using '" + std::string(node.name()) + "' as function, but it has `int` type.";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_function_as_int : public error
{
public:
    using_function_as_int(frontend::ast::node::Variable const & node)
    {
        auto&& explain = "using '" + std::string(node.name()) + "' as integer, but it has function type.";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_undeclarated_function : public error
{
public:
    using_undeclarated_function(frontend::ast::node::FunctionCall const & node)
    {
        auto&& explain = "call to undeclared function '" + std::string(node.name()) + "'";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_variable_from_parent_function_scope_error : public error
{
public:
    using_variable_from_parent_function_scope_error(frontend::ast::node::Variable const & node)
    {
        auto&& explain = "'" + std::string(node.name()) + "' was declared in parent scope, but isn`t global variable (belong to the parent function scope)";
        msg_ = show_code_error(explain, node.location());
    }
};

class last_function_statement_is_not_return_and_cannot_be_converted_to_expression : public error
{
public:
    last_function_statement_is_not_return_and_cannot_be_converted_to_expression(frontend::ast::node::FunctionDeclaration const & node, frontend::ast::node::BasicNode const & last)
    {
        auto&& explain = "in function '"+std::string(node.name())+"':\n" RESET + show_code(node.location()) + "\n"
        BOLD "last statement isn`t 'return' and cannot be converted to expression" RESET;
        msg_ = show_code_error(explain, last.location());
    }
};

class using_scope_as_expression_but_last_statement_isnt_expression : public error
{
public:
    using_scope_as_expression_but_last_statement_isnt_expression(frontend::ast::node::BasicNode const & node)
    {
        auto&& explain = "last statement in scope isn`t expression, but scope used as expression";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_scope_as_expression_but_its_empty : public error
{
public:
    using_scope_as_expression_but_its_empty(frontend::ast::node::Scope const & node)
    {
        auto&& explain = "empty scope is using like expression";
        msg_ = show_code_error(explain, node.location());
    }
};

class redeclaration_of_function : public error
{
public:
    redeclaration_of_function(frontend::ast::node::FunctionDeclaration const & node)
    {
        auto&& explain = "redeclaration of function '"+std::string(node.name())+"'";
        msg_ = show_code_error(explain, node.location());
    }
};

class function_with_empty_body_error : public error
{
public:
    function_with_empty_body_error(frontend::ast::node::FunctionDeclaration const & node)
    {
        auto&& explain = "empty function body";
        msg_ = show_code_error(explain, node.location());
    }
};

class return_in_not_function_scope_error : public error
{
public:
    return_in_not_function_scope_error(frontend::ast::node::Return const & node)
    {
        auto&& explain = "using 'return' outside of a function";
        msg_ = show_code_error(explain, node.location());
    }
};

class function_arguments_with_same_names_error : public error
{
public:
    function_arguments_with_same_names_error
    (
        frontend::ast::node::Variable const & /* FIXME: when show eror will shows more than 1 location, add this argument*/,
        frontend::ast::node::Variable const & snd
    )
    {
        auto&& explain = "function arguments must have different names";
        msg_ = show_code_error(explain, snd.location());
    }
};

} /* namespace error */

export
namespace warning
{

std::ostream& function_return_value_ignored(frontend::ast::node::FunctionCall const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "return value of '"+std::string(node.name())+"' is ignored";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& expression_result_unused(frontend::ast::node::CodeLocation const & location, std::ostream& os = std::cerr)
{
    auto&& explain = "expression result unused";
    os << show_code_error(explain, location, ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& expression_result_unused(frontend::ast::node::BasicNode const & node, std::ostream& os = std::cerr)
{
    return expression_result_unused(node.location(), os);
}

std::ostream& unnamed_function(frontend::ast::node::FunctionDeclaration const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "function has no alias and no call-name";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& instructions_after_return(frontend::ast::node::BasicNode const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "instructions after this 'return' will never be reached";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& empty_scope(frontend::ast::node::Scope const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this scope has no actions";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_semicolon(frontend::ast::node::Semicolon const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this semicolon does not match any statement";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_condition(frontend::ast::node::If const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this condition has no action";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_else(frontend::ast::node::Else const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this 'else' has no action";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& unused_variable(frontend::ast::node::Variable const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "unused variable";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& unused_function_call_name(frontend::ast::node::FunctionDeclaration const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "useless function call name '"+std::string(node.name())+"'";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
};

} /* namespace warning */

} /* namespace ParaCL::frontend */
