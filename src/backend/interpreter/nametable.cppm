module;

//---------------------------------------------------------------------------------------------------------------

#include <concepts>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module interpreter_name_table;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL::backend::interpreter::nametable
{

//---------------------------------------------------------------------------------------------------------------

class Nametable final
{
  private:
    std::vector<std::unordered_map<std::string_view, int>> scopes_;
    int *lookup(std::string_view name);
    void declare(std::string_view name, int value);

  public:
    void new_scope();
    void leave_scope();
    int get_variable_value(std::string_view name) const;
    void set_value(std::string_view name, int value);
};

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    LOGINFO("paracl: interpreter: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    LOGINFO("paracl: interpreter: nametable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

int Nametable::get_variable_value(std::string_view name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (const auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(name);

        if (found == scopes_it.end())
            continue;

        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second);
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    throw std::runtime_error(std::string("requests value of not exists variable: ") + std::string(name));
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set_value(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    int *name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value);

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int *Nametable::lookup(std::string_view name)
{
    for (auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(name);

        if (found == scopes_it.end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", name, value);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[name] = value;
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::backend::interpreter::nametable */
//---------------------------------------------------------------------------------------------------------------
