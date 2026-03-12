module;

#include <string>
#include <cassert>
#include <cstddef>
#include <random>

#include "create-basic-node.hpp"

export module name_generator;

//---------------------------------------------------------------------------------------------------------------

export import thelast;
import name_generator_nametable;
import ast_serializer;

//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
namespace test_generator::names_generator
{
//---------------------------------------------------------------------------------------------------------------

/* add node type for ast generator */

struct UninitializedNameDeclaration{};
struct UninitializedNameReUse{};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator::names_generator */
//---------------------------------------------------------------------------------------------------------------

// CREATE_SAME(last::node::writable, last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Variable, last::node::writable, last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(test_generator::UninitializedNameDeclaration)
SPECIALIZE_CREATE(test_generator::UninitializedNameReUse)

//---------------------------------------------------------------------------------------------------------------
namespace test_generator::names_generator
{
//---------------------------------------------------------------------------------------------------------------

class NameGenerator : private Nametable
{
private:
    static const constexpr std::string variables_name_base_ = "var_";
public:
    using Nametable::new_scope;
    using Nametable::leave_scope;
    using Nametable::exists;
    using Nametable::get_new_unique_name_id;
    using Nametable::declare;

private:
public:
    enum : unique_name_id_t  { RandomExistingName = 0LU };

private:


    static std::string generate_name_by_id(unique_name_id_t id)
    { return variables_name_base_ + std::to_string(id); }

    last::node::BasicNode generate_variable(unique_name_id_t id)
    {
        assert(id != RandomExistingName);
        auto&& name = generate_name_by_id(id);
        return last::node::create(last::node::Variable{std::move(name)});
    }

    unique_name_id_t select_random_existing_variable_id() const
    {
        return std::uniform_int_distribution<unique_name_id_t>{1, unique_names()}(rng);   
    }

public:
    static last::node::BasicNode create_new_uninit_name()
    {
        return last::node::create(UninitializedNameDeclaration{});
    }

    static last::node::BasicNode create_existing_uninit_name()
    {
        return last::node::create(UninitializedNameReUse{});
    }

    last::node::BasicNode generate_new_variable()
    {
        return generate_variable(++generated_names_count_);
    }

    last::node::BasicNode generate_existing_variable(unique_name_id_t id = RandomExistingName)
    {
        if (id == RandomExistingName)
        {
            id = select_random_existing_variable_id();
        }
        else if (id < get_new_unique_name_id())
        {
            throw std::runtime("Requests generation of existing variable with id, "
                               "which are not соотвествует to existing variables");
        }
        
        return generate_
    }

    /* replace UninitializedNameDeclaration/UninitializedNameReUse with last::node::Variable  */
    void set_name_to_unnamed_variable(last::node::BasicNode& variable)
    {
        if (variable.is_a<UninitializedNameDeclaration>())
            variable =  generate_new_variable();
        else if (variable.is_a<UninitializedNameReUse>())
            variable = generate_existing_variable();
        else
            throw std::runtime_error("Try to set name to not a variable");
    }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator::names_generator */
//---------------------------------------------------------------------------------------------------------------
