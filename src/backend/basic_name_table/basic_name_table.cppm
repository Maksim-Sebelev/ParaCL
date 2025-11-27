module;

//---------------------------------------------------------------------------------------------------------------

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "global/global.hpp"
#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module basic_name_table;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
class BasicNameValue
{
    private:
        std::unique_ptr<value_t> value_;

    public:
        BasicNameValue() = default;
        BasicNameValue(const value_t& value);
        BasicNameValue(value_t&& value);
        value_t        get_value          () const;
        const value_t& get_value_const_ref() const &;

        void set_value(const value_t& value);
        void set_value(value_t&& value);
};

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
BasicNameValue<value_t>::BasicNameValue(const value_t& value) :
value_(std::make_unique<value_t>(value))
{}

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
BasicNameValue<value_t>::BasicNameValue(value_t&& value) :
value_(std::make_unique<value_t>(std::move(value)))
{}

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
value_t
BasicNameValue<value_t>::get_value() const
{
    return *value_;
}

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
const value_t&
BasicNameValue<value_t>::get_value_const_ref() const &
{
    return *value_;
}

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
void
BasicNameValue<value_t>::set_value(const value_t& value)
{
    if (value_)
    {
        *value_ = value;
        return;
    }

    value_ = std::make_unique<value_t>(value);
}

//---------------------------------------------------------------------------------------------------------------

template <typename value_t>
void
BasicNameValue<value_t>::set_value(value_t&& value)
{
    if (value_)
    {
        std::swap<value_t>(*value_, value);
        return;
    }

    value_ = std::make_unique<value_t>(std::move(value));
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------

using type_id_t = int;
static constexpr type_id_t default_type_indentifier = -1;

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
class BasicNameInfo : public BasicNameValue<value_t>
{
    private:
        name_id_t id_;
        bool      init_ = false;
        bool      used_ = false;

    public:
        BasicNameInfo() = default;
        BasicNameInfo(const name_id_t& id);
        BasicNameInfo(const name_id_t& id, const value_t& value_);
        BasicNameInfo(const name_id_t& id, value_t&& value);

        name_id_t         get_id   () const;
        type_id_t         get_type () const;

        bool              is_init  () const;
        bool              is_used  () const;

        void              init     ();
        void              used     ();
};

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
BasicNameInfo<name_id_t, type_id, value_t>::BasicNameInfo(const name_id_t& id) :
id_ (id)
{}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
BasicNameInfo<name_id_t, type_id, value_t>::BasicNameInfo(const name_id_t& id, const value_t& value) :
BasicNameValue<value_t>(value),
id_(id),
init_(true)
{}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
BasicNameInfo<name_id_t, type_id, value_t>::BasicNameInfo(const name_id_t& id, value_t&& value) :
BasicNameValue<value_t>(std::move(value)),
id_(id),
init_(true)
{}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
name_id_t
BasicNameInfo<name_id_t, type_id, value_t>::get_id() const
{
    return id_;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
type_id_t
BasicNameInfo<name_id_t, type_id, value_t>::get_type() const
{
    return type_id;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
bool
BasicNameInfo<name_id_t, type_id, value_t>::is_init() const
{
    return init_;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
bool
BasicNameInfo<name_id_t, type_id, value_t>::is_used() const
{
    return used_;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
void
BasicNameInfo<name_id_t, type_id, value_t>::init()
{
    init_ = true;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, type_id_t type_id, typename value_t>
void
BasicNameInfo<name_id_t, type_id, value_t>::used()
{
    used_ = true;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t>
class BasicNameTable
{
  private:
    std::vector<std::unordered_map<BasicNameInfo<>>> scopes_;

  protected:
    virtual name_info_t *lookup(const name_id_t &id);
    virtual void declare(const name_id_t &id, const name_info_t &value);
    void call_method()

  public:
    void new_scope();
    void leave_scope();

    virtual std::optional<name_info_t> get_variable_value(const name_id_t &id) const;
    virtual void set_value(const name_id_t &id, const name_info_t &value);

    virtual ~BasicNameTable() = default;
};

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t> void BasicNameTable<name_info_t>::new_scope()
{
    LOGINFO("paracl: BasicNameTable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t> void BasicNameTable<name_info_t>::leave_scope()
{
    LOGINFO("paracl: BasicNameTable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t>
std::optional<name_info_t> BasicNameTable<name_info_t>::get_variable_value(const name_id_t &id) const
{
    LOGINFO("paracl: BasicNameTable: searching variable: \"{}\"", id);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(id);

        if (found == it->end())
            continue;

        LOGINFO("paracl: BasicNameTable: variable found: \"{}\"", id);
        return found->second;
    }

    LOGINFO("paracl: BasicNameTable: variable NOT found: \"{}\"", id);
    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t>
void BasicNameTable<name_info_t>::set_value(const name_id_t &id, const name_info_t &value)
{
    LOGINFO("paracl: BasicNameTable: set \"{}\"", id);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    name_info_t *name_ptr = lookup(id);

    if (not name_ptr)
        return declare(id, value);

    LOGINFO("paracl: BasicNameTable: set to \"{}\"", id);
    // name_ptr->value = value.value;
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t> name_info_t *BasicNameTable<name_info_t>::lookup(const name_id_t &id)
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(id);

        if (found == it->end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

template <typename name_id_t, typename name_info_t>
void BasicNameTable<name_info_t>::declare(const name_id_t &id, const name_info_t &value)
{
    LOGINFO("paracl: BasicNameTable: declare {}", id);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[id] = value;
}

//---------------------------------------------------------------------------------------------------------------

} // namespace ParaCL

//---------------------------------------------------------------------------------------------------------------
