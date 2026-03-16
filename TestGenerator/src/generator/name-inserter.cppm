#include <cinttypes>
export module name_inserter;

export import thelast;
import name_generator;


namespace test_generator::name_inserter
{
export using names_insertable = void (name_generator::NameGenerator&);

auto insert_names(last::node::BasicNode & node, name_generator::NameGenerator& name_generator)
    -> decltype(last::node::visit<void, name_generator::NameGenerator&>(node, name_generator))
{
    return last::node::visit<void, name_generator::NameGenerator&>(node, name_generator);
}

} /* namespace test_generator::name_inserter */

namespace last::node::visit_specializations
{

template <>
void visit(last::node::Scan& , name_generator::NameGenerator& )
{}

template <>
void visit(last::node::Print& node, name_generator::NameGenerator& name_generator)
{
    for (auto&& arg: node)
        test_generator::name_inserter::insert_names(arg, name_generator);
}

template <>
void visit(test_generator::name_generator::UninitializedNameDeclaration & node, name_generator::NameGenerator& name_generator)
{
    node = name_generator.create_new_uninit_name();
}

template <>
void visit(test_generator::name_generator::UninitializedNameDeclaration & node, name_generator::NameGenerator& name_generator)
{
    node = name_generator.rep();
}

template <>
void visit(test_generator::name_generator::UninitializedNameReUse & node, name_generator::NameGenerator& name_generator)
{
    node = name_generator.create_existing_uninit_name();
}

} /* namespace last::node::visit_specializations */
