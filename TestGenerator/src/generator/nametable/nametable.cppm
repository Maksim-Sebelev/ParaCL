module;

//---------------------------------------------------------------------------------------------------------------

#include <string_view>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstddef>

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module nametable;

//---------------------------------------------------------------------------------------------------------------

namespace test_generator::nametable
{

//---------------------------------------------------------------------------------------------------------------

export
class Nametable final
{
  private:
    std::vector<std::unordered_map<std::string_view, int>> scopes_;
    size_t unique_names_quant_ = 0;

  private:
    int      * lookup            (std::string_view name);
    int const* lookup            (std::string_view name) const;
    void       declare           (std::string_view name, int value);

  public:
    void new_scope         ();
    void leave_scope       ();
    void set_value         (std::string_view name, int value);
    bool exists            (std::string_view name)             const;
    int  get_variable_value(std::string_view name)             const;
    int  operator []       (std::string_view name)             const;
    size_t unique_names    ()                                  const;
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

    if (scopes_.empty()) return;

    unique_names_quant_ -= scopes_.back().size();

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

int Nametable::get_variable_value(std::string_view name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second);
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    throw std::runtime_error(std::string("requests value of not exists variable: ") + std::string(name));
}

//---------------------------------------------------------------------------------------------------------------

int Nametable::operator[] (std::string_view name) const
{
    return get_variable_value(name);
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set_value(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    auto&& name_ptr = lookup(name);

    if (not name_ptr) return declare(name, value);

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);
    *name_ptr = value;
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::exists(std::string_view name) const
{
    return lookup(name);
}

//---------------------------------------------------------------------------------------------------------------

size_t Nametable::unique_names() const
{
    return unique_names_quant_;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int * Nametable::lookup(std::string_view name)
{
    for (auto &&scope: scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

int const * Nametable::lookup(std::string_view name) const
{
    for (auto &&scope: scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
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

    ++unique_names_quant_;
    scopes_.back()[name] = value;
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::backend::interpreter::nametable */
//---------------------------------------------------------------------------------------------------------------
