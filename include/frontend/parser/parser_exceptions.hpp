#pragma once

#include <stdexcept>
#include <string>

namespace ErrorHandler
{

class parse_grammar_error : public std::exception
{
  private:
    std::string msg_;

  public:
    parse_grammar_error(const std::string &msg);
    const char *what() const noexcept override;
};

class undeclarated_variable_error : public std::exception
{
  private:
    std::string msg_;

  public:
    undeclarated_variable_error(const std::string &msg);
    const char *what() const noexcept override;
};

} // namespace ErrorHandler