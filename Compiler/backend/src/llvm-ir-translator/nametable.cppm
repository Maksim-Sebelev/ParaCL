module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <iostream>
// #include "log/log_api.hpp"
#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module nametable;

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator::nametable
{

//---------------------------------------------------------------------------------------------------------------

export class Nametable final
{
  private:
    llvm::IRBuilder<> &builder_;

    std::vector<std::unordered_map<std::string_view, llvm::Value *>> scopes_;

    llvm::Value *lookup_(std::string_view name);
    void declare_(std::string_view name, llvm::Value * = nullptr);

  private:
    static bool is_function(llvm::Value const* value)
    {
        return llvm::isa<llvm::Function>(value);
    }

  public:
    Nametable(llvm::IRBuilder<> &builder);

    void new_scope();
    void leave_scope();

    llvm::Value *get(std::string_view name);

    void set(std::string_view name, llvm::Value *value);
    void force_declare(std::string_view name, llvm::Value *value);

    friend void dump(Nametable const & nt)
    {
        static auto&& dump_counter = 0LU;
        std::cout << "NAMETABLE DUMP[" << dump_counter++ << "]\n{" << std::endl;
        for (auto&& scope: nt.scopes_)
        {
            for (auto&& name: scope)
                std::cout << "\t(" << name.first << ", " << name.second->getType()->getTypeID() << ")" << std::endl;
            std::cout << std::endl;;
        }
        std::cout << "}" << std::endl;
    }
};

//---------------------------------------------------------------------------------------------------------------

export void dump(Nametable const &);

//---------------------------------------------------------------------------------------------------------------

Nametable::Nametable(llvm::IRBuilder<> &builder)
    : builder_(builder), scopes_(1 /* global scope */)
{
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    LOGINFO("paracl: compiler: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    LOGINFO("paracl: compiler: nametable: exiting scope");
    if (scopes_.empty()) return;
    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *Nametable::get(std::string_view name)
{
    auto&& var = lookup_(name);
    if (not var) return nullptr;
    if (is_function(var)) return var;
    return builder_.CreateLoad(builder_.getInt32Ty(), var, std::string(name) + "_load");
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: set \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set '" + std::string(name) + "': no active scopes");

    auto&& var = lookup_(name);
    if (not var) return declare_(name, value);

    if (is_function(value))
    {
        // FIXME: here we execute lookup_ action again. need to made somethig smarter
        // we cannont just made: var = value, cause lookup_ return not a pointer reference, just pointer
        for (auto&& scope : scopes_ | std::views::reverse)
        {
            auto&& found = scope.find(name);
            if (found == scope.end()) continue;
            found->second = value;
        }
    
        return;
    }

    /* if (not is_function(value))*/

    builder_.CreateStore(value, var);
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

llvm::Value *Nametable::lookup_(std::string_view name)
{
    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return found->second;
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare_(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: declare_ \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare_ variable: no active scopes");

    if (is_function(value))
    {
        scopes_.back()[name] = value;
        return;
    }

    auto&& variable = (scopes_.back()[name] = builder_.CreateAlloca(value->getType(), nullptr, name));

    builder_.CreateStore(value, variable);
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::force_declare(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: declare_ \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare_ variable: no active scopes");

    if (scopes_.back().find(name) != scopes_.back().end())
        throw std::runtime_error("cannot declare_ '" + std::string(name) + "', cause it already exists in current scope");

    declare_(name, value);
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace compiler::llvm_ir_translator::nametable */
//---------------------------------------------------------------------------------------------------------------

