#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace ParaCL
{

struct NameTable
{
  private:
    std::vector<std::unordered_set<std::string>> scopes_;

  public:
    NameTable() = default;
    void new_scope();
    void leave_scope();
    bool is_not_declare(const std::string &variable) const;
    bool is_declare(const std::string &variable) const;

    void declare_or_do_nothing_if_already_declared(const std::string &variable);
};

} /* namespace ParaCL*/
