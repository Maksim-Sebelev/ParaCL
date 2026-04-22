module;
//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>

#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <optional>

#if not defined(NDEBUG)
#include <iostream>
#endif /* not defined(NDEBUG) */

//---------------------------------------------------------------------------------------------------------------
export module nametable;
//---------------------------------------------------------------------------------------------------------------

import frontend_errors;
import thelast;

//---------------------------------------------------------------------------------------------------------------
namespace ParaCL::frontend::llvm_ir_translator
{
//---------------------------------------------------------------------------------------------------------------

export
enum ValueStatus { global, local };

//---------------------------------------------------------------------------------------------------------------

llvm::Function const* get_function_of_value(llvm::Value const* value)
{
    if (!value) return nullptr;

    if (auto&& inst = llvm::dyn_cast<llvm::Instruction>(value))
        return inst->getFunction();

    if (auto&& arg = llvm::dyn_cast<llvm::Argument>(value))
        return arg->getParent();

    if (auto&& basicBlock = llvm::dyn_cast<llvm::BasicBlock>(value))
        return basicBlock->getParent();

    if (auto&& func = llvm::dyn_cast<llvm::Function>(value))
        return func;

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

export class Nametable final
{
  private:
    llvm::Module& module_;
    llvm::IRBuilder<> &builder_;


private:


    using identifier_t = std::string_view;
    struct identifier_info_t
    {
        ast::node::Variable declaration_node{""};
        llvm::Value* value;
        ValueStatus status;

        bool used : 1 = false;
    };

    std::vector
    <
        std::unordered_map
        <
            identifier_t,
            identifier_info_t
        >
    > scopes_;

private:

    identifier_info_t       * lookup_(identifier_t name);
    identifier_info_t const * lookup_(identifier_t name) const;

    void declare_(identifier_t name, llvm::Value * value, ast::node::Variable const & declaration_node, ValueStatus status);

  private:
    static bool is_function(llvm::Value const* value)
    {
        return llvm::isa<llvm::Function>(value);
    }

  public:
    Nametable(llvm::Module& module, llvm::IRBuilder<> &builder);

    void new_scope();
    void leave_scope();

    llvm::Value *get(std::string_view name);

    void set(std::string_view name, llvm::Value *value, ast::node::Variable const & declaration_node, ValueStatus status = local);
    void force_declare(std::string_view name, llvm::Value *value, ast::node::Variable const & declaration_node);
    ValueStatus status(std::string_view name) const;
    bool from_current_scope(std::string_view name) const;
    bool is_visible_from(std::string_view name, llvm::Function* function) const;
    bool is_function(std::string_view name) const;

#if not defined(NDEBUG)
    friend void dump(Nametable const & nt)
    {
#if 0
        static auto&& dump_counter = 0LU;
        std::cout << "NAMETABLE DUMP[" << dump_counter++ << "]\n{" << std::endl;
        for (auto&& scope: nt.scopes_)
        {
            for (auto&& name: scope)
                std::cout << "\t(" << name.first << ", " << name.second.first->getType()->getTypeID() << ")" << std::endl;
            std::cout << std::endl;;
        }
        std::cout << "}" << std::endl;
        #endif /* 0 */
    }
#endif /* not defined(NDEBUG) */
};

//---------------------------------------------------------------------------------------------------------------
#if not defined(NDEBUG)
export void dump(Nametable const &);
#endif /* not defined(NDEBUG) */
//---------------------------------------------------------------------------------------------------------------

Nametable::Nametable(llvm::Module& module, llvm::IRBuilder<> &builder)
    : module_(module), builder_(builder), scopes_(1 /* global scope */)
{
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
#if not defined(NDEBUG)
    if (scopes_.empty())
        throw std::runtime_error("paracl: try to leave scope, when there are no scopes in nametable");
#endif /* not defined(NDEBUG) */

    auto&& unused_variable_check = [&](auto identifier) -> void
    {
        auto&& info = identifier.second;
        if (info.used) return;
        warning::unused_variable(info.declaration_node);
    };

    auto&& back = scopes_.back();

    std::for_each(back.begin(), back.end(), unused_variable_check);

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *Nametable::get(std::string_view name)
{
    auto&& variable_info_ptr = lookup_(name);
    if (not variable_info_ptr) return nullptr;

    auto&& variable = *variable_info_ptr;
    variable.used = true;

    auto&& variable_value = variable.value;
    if (is_function(variable_value)) return variable_value;
    return builder_.CreateLoad(builder_.getInt32Ty(), variable_value, std::string(name) + "_load");
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set(std::string_view name, llvm::Value *value, ast::node::Variable const & declaration_node, ValueStatus status)
{
#if not defined(NDEBUG)
    if (scopes_.empty())
        throw std::runtime_error("cannot set '" + std::string(name) + "': no active scopes");
#endif /* not defined(NDEBUG) */

    auto&& variable_info_ptr = lookup_(name);
    if (not variable_info_ptr) return declare_(name, value, declaration_node, status);

    auto&& variable = *variable_info_ptr;

    if (is_function(value))
    {
        variable.value = value;
        variable.status = status;
        return;
    }

    builder_.CreateStore(value, variable.value);
}

//---------------------------------------------------------------------------------------------------------------

ValueStatus Nametable::status(std::string_view name) const
{
    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return found->second.status;
    }

#if defined(NDEBUG)
    __builtin_unreachable();
#endif /* defined(NDEBUG) */

    throw std::runtime_error("requests status of undeclarated variable '" + std::string(name) + "'");
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::from_current_scope(std::string_view name) const
{
    if (scopes_.empty()) return false;
    return (scopes_.back().find(name) != scopes_.back().end());
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::is_visible_from(std::string_view name, llvm::Function* function) const
{
    auto&& variable_info_ptr = lookup_(name);
    if (not variable_info_ptr) return false;

    auto&& variable = *variable_info_ptr;

    return
        (status(name) == ValueStatus::global)
        or
        (get_function_of_value(variable.value) == function);
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::is_function(std::string_view name) const
{
    auto&& variable_info_ptr = lookup_(name); assert(variable_info_ptr);
    if (not variable_info_ptr) return false;

    auto&& variable = *variable_info_ptr;

    return is_function(variable.value);
}

//---------------------------------------------------------------------------------------------------------------

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

Nametable::identifier_info_t const* Nametable::lookup_(Nametable::identifier_t name) const
{
    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return std::addressof(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

Nametable::identifier_info_t* Nametable::lookup_(Nametable::identifier_t name)
{
    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return std::addressof(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare_(Nametable::identifier_t name, llvm::Value *value, ast::node::Variable const & declaration_node, ValueStatus status)
{
    assert(scopes_.size() > 0);

    auto&& variable = scopes_.back()[name];
    variable.status = status;
    variable.declaration_node = declaration_node;

    auto&& variable_value = variable.value;
    if (is_function(value))
    {
        variable_value = value;
        return;
    }

    if (status == global)
        variable_value = new llvm::GlobalVariable(module_, value->getType(), false, llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(value->getType()), name);
    else
        variable_value = builder_.CreateAlloca(value->getType(), nullptr, name);

    builder_.CreateStore(value, variable_value);
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::force_declare(Nametable::identifier_t name, llvm::Value *value, ast::node::Variable const & declaration_node)
{
    assert(scopes_.size() > 0);
    
    auto&& back = scopes_.back();

    if (auto&& found = back.find(name); found != back.end())
        throw error::function_arguments_with_same_names_error(found->second.declaration_node, declaration_node);

    declare_(name, value, declaration_node, ValueStatus::local);
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::frontend::llvm_ir_translator */
//---------------------------------------------------------------------------------------------------------------

