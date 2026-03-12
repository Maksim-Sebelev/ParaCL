module;

//---------------------------------------------------------------------------------------------------------------

#include <string_view>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstddef>
#include <unordered_set>
#include <type_traits>

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module name_generator_nametable;

//---------------------------------------------------------------------------------------------------------------

namespace test_generator::names_generator
{

//---------------------------------------------------------------------------------------------------------------

export
using unique_name_id_t = unsigned int;

//---------------------------------------------------------------------------------------------------------------

export
class Nametable
{
  private:
    using scopes_t = std::vector<std::unordered_set<unique_name_id_t>>;
    scopes_t scopes_;

    /* here size_t is not a unique_name_id_t */
    size_t unique_names_quant_ = 0;

  public:
    void             new_scope             ();
    void             leave_scope           ();
    void             declare               (unique_name_id_t id); 

    unique_name_id_t get_new_unique_name_id()                    const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>);
    bool             exists                (unique_name_id_t id) const;
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

unique_name_id_t Nametable::get_new_unique_name_id() const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>)
{
    return unique_names_quant_;
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::exists(unique_name_id_t id) const
{
    for (auto&& scope: scopes_ | std::views::reverse)
    {
        if (not scope.contains(id)) continue;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(unique_name_id_t id)
{
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", id);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    if (exists(id))
        throw std::runtime_error("Redeclarion of '" + std::to_string(id) + "'");

    ++unique_names_quant_;
    scopes_.back().insert(id);
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator::names_generator */
//---------------------------------------------------------------------------------------------------------------
