#include <string>
#include "parser/parser_exceptions.hpp"


namespace ErrorHandler
{

parse_grammar_error::parse_grammar_error(const std::string& msg) :
msg_(msg)
{}

const char*
parse_grammar_error::what() const noexcept
{
    return msg_.c_str();
}

undeclarated_variable_error::undeclarated_variable_error(const std::string& msg) :
msg_(msg)
{}

const char*
undeclarated_variable_error::what() const noexcept
{
    return msg_.c_str();
}

} /* namespace ParaCL */
