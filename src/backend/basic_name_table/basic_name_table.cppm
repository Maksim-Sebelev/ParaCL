module;

//---------------------------------------------------------------------------------------------------------------

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "global/global.hpp"
#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module basic_name_table;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
class BasicNameTable
{
  private:
    std::vector<std::unordered_map<std::string, name_info_t>> scopes_;

   protected:
    virtual name_info_t *lookup(const std::string &name) const;
    virtual void declare(const std::string &name, const name_info_t& value);

  public:
    void new_scope();
    void leave_scope();

    virtual std::optional<name_info_t> get_variable_value(const std::string &name) const;
    virtual void set_value(const std::string &name, const name_info_t &value);

    virtual ~BasicNameTable();
};

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
void BasicNameTable<name_info_t>::new_scope()
{
    LOGINFO("paracl: BasicNameTable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
void BasicNameTable<name_info_t>::leave_scope()
{
    LOGINFO("paracl: BasicNameTable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
std::optional<name_info_t> BasicNameTable<name_info_t>::get_variable_value(const std::string &name) const
{
    LOGINFO("paracl: BasicNameTable: searching variable: \"{}\"", name);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(name);

        if (found == it->end())
            continue;

        LOGINFO("paracl: BasicNameTable: variable found: \"{}\"", name);
        return found->second;
    }

    LOGINFO("paracl: BasicNameTable: variable NOT found: \"{}\"", name);
    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
void BasicNameTable<name_info_t>::set_value(const std::string &name, const name_info_t &value)
{
    LOGINFO("paracl: BasicNameTable: set \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    name_info_t *name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value);

    LOGINFO("paracl: BasicNameTable: set to \"{}\"", name);
    // name_ptr->value = value.value;
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
name_info_t *BasicNameTable<name_info_t>::lookup(const std::string &name) const
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(name);

        if (found == it->end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_info_t>
void BasicNameTable<name_info_t>::declare(const std::string &name, const name_info_t& value)
{
    LOGINFO("paracl: BasicNameTable: declare {}", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[name] = value;
}

//---------------------------------------------------------------------------------------------------------------

} // namespace ParaCL

//---------------------------------------------------------------------------------------------------------------