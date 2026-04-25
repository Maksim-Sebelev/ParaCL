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

std::string to_string(ast::node::CodeLocation const & location)
{
    auto&& loc = std::ostringstream{};
    loc << location.file() << ":" << location.line_begin() << ":" << location.column_begin();
    return loc.str();
}

std::string mark_error(ast::node::CodeLocation const & location)
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

std::string note(std::string_view msg)
{
    auto&& note = std::ostringstream{};

    note << BOLD "note: " << msg << RESET;

    return note.str();
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

std::string show_code(ast::node::CodeLocation const & location)
{
    auto&& code = std::ostringstream{};
    auto&& indent = " " + std::to_string(location.line_begin()) + " | ";
    auto&& indent_length = indent.length(); assert(indent_length >= 3);
    code << indent << location.code_excerpt() << "\n" << std::string(indent_length - 2, ' ') << "| " << mark_error(location);
    return code.str();
}

std::string show_code_error(std::string_view msg, ast::node::CodeLocation const & location, ProblemStatus status = ProblemStatus::Error)
{
    auto&& explain = std::ostringstream{};
    explain << BOLD << to_string(location)
        << ": " RESET
        << ((status == ProblemStatus::Error) ? ERROR "error:" : WARNING "warning:")
        << " " RESET BOLD << msg << RESET "\n"
        << show_code(location);

    return explain.str();
}

std::string was_declared_here(std::string_view name, ast::node::CodeLocation const & location, std::string user_msg = "was declared here: ")
{
    auto&& msg = std::ostringstream{};

    msg << "'" << name << "' " << user_msg << to_string(location) << "\n" RESET
        << show_code(location) + RESET;

    return msg.str();    
}

std::string was_declared_here(ast::node::Variable const & node, std::string msg = "was declared here: ")
{
    return was_declared_here(node.name(), node.location(), msg);
}

std::string was_declared_here(ast::node::FunctionDeclaration const & node, std::string msg = "was declared here: ")
{
    return was_declared_here(node.name(), node.location(), msg);
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
    parser_error(ast::node::CodeLocation const & location, std::string_view explain)
    {
        msg_ = show_code_error(explain, location);
    }
};

class using_undeclarated_variable : public error
{
public:
    using_undeclarated_variable(ast::node::Variable const & node)
    {
        auto&& explain = "use of undeclared identifier '" + std::string(node.name()) + "'";
        msg_ = show_code_error(explain, node.location());
    }
};

class function_alias_arguments_mismatch : public error
{
public:
    function_alias_arguments_mismatch(ast::node::FunctionCall const & node, size_t expected_args_size, size_t call_args_size)
    {
        auto&& explain = "function alias signature mismatch, '" + std::string(node.name()) +
                                 "' expects " + std::to_string(expected_args_size) +
                                 " arg" + ((expected_args_size != 1) ? "s" : "") +
                                 ", but called with " + std::to_string(call_args_size) + ":";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_int_as_function : public error
{
public:
    using_int_as_function(ast::node::FunctionCall const & node, ast::node::Variable const & declaration_node)
    {
        auto&& explain = "using '" + std::string(node.name()) + "' as function, but it has `int` type";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node, "was declared as integer here: "));
    }
};

class using_function_as_int : public error
{
public:
    using_function_as_int(ast::node::Variable const & node, ast::node::Variable const & declaration_node)
    {
        auto&& explain = "using '" + std::string(node.name()) + "' as integer, but it has function type.";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node, "was declared as function here: "));
    }
};

class using_undeclarated_function : public error
{
public:
    using_undeclarated_function(ast::node::FunctionCall const & node)
    {
        auto&& explain = "call to undeclared function '" + std::string(node.name()) + "'";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_variable_from_parent_function_scope : public error
{
public:
    using_variable_from_parent_function_scope(ast::node::Variable const & node, ast::node::Variable const & declaration_node)
    {
        auto&& explain = "'" + std::string(node.name()) + "' was declared in parent scope, but isn`t global variable (belong to the parent function scope)";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node));
    }
};

class last_function_statement_is_not_return_and_cannot_be_converted_to_expression : public error
{
public:
    last_function_statement_is_not_return_and_cannot_be_converted_to_expression(ast::node::FunctionDeclaration const & node, ast::node::BasicNode const & last)
    {
        auto&& explain = "in function '"+std::string(node.name())+"':\n" RESET + show_code(node.location()) + "\n"
        BOLD "last statement isn`t 'return' and cannot be converted to expression" RESET;
        msg_ = show_code_error(explain, last.location());
    }
};

class using_scope_as_expression_but_last_statement_isnt_expression : public error
{
public:
    using_scope_as_expression_but_last_statement_isnt_expression(ast::node::BasicNode const & node)
    {
        auto&& explain = "last statement in scope isn`t expression, but scope used as expression";
        msg_ = show_code_error(explain, node.location());
    }
};

class using_scope_as_expression_but_its_empty : public error
{
public:
    using_scope_as_expression_but_its_empty(ast::node::Scope const & node)
    {
        auto&& explain = "empty scope is using like expression";
        msg_ = show_code_error(explain, node.location());
    }
};

class redeclaration_of_function : public error
{
public:
    redeclaration_of_function(ast::node::FunctionDeclaration const & node, ast::node::FunctionDeclaration const & declaration_node)
    {
        auto&& explain = "redeclaration of '"+std::string(node.name())+"'";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node));
    }
};

class function_with_empty_body : public error
{
public:
    function_with_empty_body(ast::node::FunctionDeclaration const & node)
    {
        auto&& explain = "empty function body";
        msg_ = show_code_error(explain, node.location());
    }
};

class return_in_not_function_scope : public error
{
public:
    return_in_not_function_scope(ast::node::Return const & node)
    {
        auto&& explain = "using 'return' outside of a function";
        msg_ = show_code_error(explain, node.location());
    }
};

class function_arguments_with_same_names : public error
{
public:
    function_arguments_with_same_names
    (
        ast::node::Variable const & /* FIXME: when show eror will shows more than 1 location, add this argument*/,
        ast::node::Variable const & snd
    )
    {
        auto&& explain = "function arguments must have different names";
        msg_ = show_code_error(explain, snd.location());
    }
};

class set_integer_variable_function_value : public error
{
public:
    set_integer_variable_function_value(ast::node::Variable const & node, ast::node::Variable const & declaration_node)
    {
        auto&& explain = "set '"+std::string(node.name())+"' function value, but it has integer type";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node));
    }
};

class set_function_variale_integer_value : public error
{
public:
    set_function_variale_integer_value(ast::node::Variable const & node, ast::node::Variable const & declaration_node)
    {
        auto&& explain = "set '"+std::string(node.name())+"' integer value, but it has function type";
        msg_ = show_code_error(explain, node.location()) + "\n"
        + note(was_declared_here(declaration_node));
    }
};

} /* namespace error */

export
namespace warning
{

std::ostream& function_return_value_ignored(ast::node::FunctionCall const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "return value of '"+std::string(node.name())+"' is ignored";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& expression_result_unused(ast::node::CodeLocation const & location, std::ostream& os = std::cerr)
{
    auto&& explain = "expression result unused";
    os << show_code_error(explain, location, ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& expression_result_unused(ast::node::BasicNode const & node, std::ostream& os = std::cerr)
{
    return expression_result_unused(node.location(), os);
}

std::ostream& unnamed_function(ast::node::FunctionDeclaration const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "function has no alias and no call-name";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& instructions_after_return(ast::node::BasicNode const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "instructions after this 'return' will never be reached";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& empty_scope(ast::node::Scope const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this scope has no actions";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_semicolon(ast::node::Semicolon const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this semicolon does not match any statement";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_condition(ast::node::If const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this condition has no action";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& useless_else(ast::node::Else const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "this 'else' has no action";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& unused_variable(ast::node::Variable const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "unused variable";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
}

std::ostream& unused_function_call_name(ast::node::FunctionDeclaration const & node, std::ostream& os = std::cerr)
{
    auto&& explain = "useless function call name '"+std::string(node.name())+"'";
    os << show_code_error(explain, node.location(), ProblemStatus::Warning) << "\n";

    return os;
};

} /* namespace warning */

} /* namespace ParaCL::frontend */
