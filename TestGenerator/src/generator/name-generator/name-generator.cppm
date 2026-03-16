module;

#include <string>
#include <cassert>
#include <cstddef>
#include <random>

#define THELAST_EXPORT_CREATE_SPEZIALIZATIONS
#include "create-basic-node.hpp"

export module name_generator;

//---------------------------------------------------------------------------------------------------------------

export import thelast;
import name_generator_nametable;
import ast_serializer;
import global_create_settings;

//---------------------------------------------------------------------------------------------------------------
namespace test_generator::names_generator
{
//---------------------------------------------------------------------------------------------------------------

/* add node type for ast generator */

export
enum : unique_name_id_t  { RandomExistingName = 0LU };

export
struct UninitializedNameDeclaration{};

export
struct UninitializedNameReUse
{
private:
    unique_name_id_t id_;

public:
    UninitializedNameReUse(unique_name_id_t id = RandomExistingName) :
        id_(id)
    {}

public:
    unique_name_id_t id() const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>)
    { return id_; }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator::names_generator */
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
namespace last::node::visit_specializations
{
//---------------------------------------------------------------------------------------------------------------

template <>
void visit(test_generator::names_generator::UninitializedNameDeclaration const&)
{}

template <>
void visit(test_generator::names_generator::UninitializedNameReUse const&)
{}

//---------------------------------------------------------------------------------------------------------------
} /* namespace last::node::visit_specializations */
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------

SPECIALIZE_CREATE(test_generator::names_generator::UninitializedNameDeclaration)
SPECIALIZE_CREATE(test_generator::names_generator::UninitializedNameReUse)

//--------------------------------------------- ------------------------------------------------------------------
namespace test_generator::names_generator
{
//---------------------------------------------------------------------------------------------------------------

class NameGenerator : private Nametable
{
private:
    static const constexpr std::string variables_name_base_ = "var_";
private:
    std::mt19937& rng_;
public:
    using Nametable::new_scope;
    using Nametable::leave_scope;
    using Nametable::exists;
    using Nametable::get_new_unique_name_id;
    using Nametable::declare;

public:

private:
    static std::string generate_name_by_id(unique_name_id_t id)
    { return variables_name_base_ + std::to_string(id); }

private:
    unique_name_id_t select_random_existing_variable_id() const
    { return std::uniform_int_distribution<unique_name_id_t>{static_cast<unique_name_id_t>(1), get_new_unique_name_id()}(rng_); }

    last::node::BasicNode generate_variable(unique_name_id_t id)
    {
        assert(id != RandomExistingName);
        assert(id <= get_new_unique_name_id());

        if (id == get_new_unique_name_id())
            declare(id);

        auto&& name = generate_name_by_id(id);

        return last::node::create(last::node::Variable{std::move(name)});
    }

public:

    NameGenerator(std::mt19937& randomizer) : rng_(randomizer)
    { new_scope(); /* global scope */ }

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
        return generate_variable(get_new_unique_name_id());
    }

    last::node::BasicNode generate_existing_variable(unique_name_id_t id = RandomExistingName)
    {
        if (id == RandomExistingName)
        {
            id = select_random_existing_variable_id();
        }
        else if (id >= get_new_unique_name_id())
        {
            throw std::runtime_error("Requests generation of existing variable with id, "
                                     "which is соотвествует existing variable");
        }

        return generate_variable(id);
    }

    /* replace UninitializedNameDeclaration/UninitializedNameReUse with last::node::Variable  */
    void set_name_to_unnamed_variable(last::node::BasicNode& variable)
    {
        if (variable.is_a<UninitializedNameDeclaration>())
            variable =  generate_new_variable();
        else if (variable.is_a<UninitializedNameReUse>())
            variable = generate_existing_variable(static_cast<UninitializedNameReUse const &>(variable).id());
        else
            throw std::runtime_error("Try to set name to not a uninit variable with in namegenerator");
    }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator::names_generator */
//---------------------------------------------------------------------------------------------------------------
