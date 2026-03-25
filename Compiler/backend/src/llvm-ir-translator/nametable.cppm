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
#include <iostream>
// #include "log/log_api.hpp"
#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module nametable;

//---------------------------------------------------------------------------------------------------------------
namespace compiler::llvm_ir_translator
{
//---------------------------------------------------------------------------------------------------------------

export
enum ValueStatus { global, local };

//---------------------------------------------------------------------------------------------------------------

export class Nametable final
{
  private:
    llvm::Module& module_;
    llvm::IRBuilder<> &builder_;

    std::vector<std::unordered_map<std::string_view, std::pair<llvm::Value *, ValueStatus>>> scopes_;

    llvm::Value *lookup_(std::string_view name);
    void declare_(std::string_view name, llvm::Value *, ValueStatus status);

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

    void set(std::string_view name, llvm::Value *value, ValueStatus status = local);
    void force_declare(std::string_view name, llvm::Value *value);
    ValueStatus status(std::string_view name) const;
    bool from_current_scope(std::string_view name) const;

    friend void dump(Nametable const & nt)
    {
        static auto&& dump_counter = 0LU;
        std::cout << "NAMETABLE DUMP[" << dump_counter++ << "]\n{" << std::endl;
        for (auto&& scope: nt.scopes_)
        {
            for (auto&& name: scope)
                std::cout << "\t(" << name.first << ", " << name.second.first->getType()->getTypeID() << ")" << std::endl;
            std::cout << std::endl;;
        }
        std::cout << "}" << std::endl;
    }
};

//---------------------------------------------------------------------------------------------------------------

export void dump(Nametable const &);

//---------------------------------------------------------------------------------------------------------------

Nametable::Nametable(llvm::Module& module, llvm::IRBuilder<> &builder)
    : module_(module), builder_(builder), scopes_(1 /* global scope */)
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

void Nametable::set(std::string_view name, llvm::Value *value, ValueStatus status)
{
    LOGINFO("paracl: compiler: nametable: set \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set '" + std::string(name) + "': no active scopes");

    auto&& var = lookup_(name);
    if (not var) return declare_(name, value, status);

    if (is_function(value))
    {
        // FIXME: here we execute lookup_ action again. need to made somethig smarter
        // we cannont just made: var = value, cause lookup_ return not a pointer reference, just pointer
        for (auto&& scope : scopes_ | std::views::reverse)
        {
            auto&& found = scope.find(name);
            if (found == scope.end()) continue;
            found->second.first = value;
            found->second.second = status;
        }
    
        return;
    }

    builder_.CreateStore(value, var);
}

//---------------------------------------------------------------------------------------------------------------

ValueStatus Nametable::status(std::string_view name) const
{
    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        return found->second.second;
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
        return found->second.first;
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare_(std::string_view name, llvm::Value *value, ValueStatus status)
{
    LOGINFO("paracl: compiler: nametable: declare_ \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare_ variable: no active scopes");

    if (is_function(value))
    {
        scopes_.back()[name].first = value;
        return;
    }

    auto&& variable = scopes_.back()[name];

    if (status == local)
        variable.first = builder_.CreateAlloca(value->getType(), nullptr, name);
    else
        variable.first = new llvm::GlobalVariable(module_, value->getType(), false, llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(value->getType()), name);

    variable.second = status;

    builder_.CreateStore(value, variable.first);
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::force_declare(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: declare_ \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare_ variable: no active scopes");

    if (scopes_.back().find(name) != scopes_.back().end())
        throw std::runtime_error("cannot declare_ '" + std::string(name) + "', cause it already exists in current scope");

    declare_(name, value, ValueStatus::local);
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace compiler::llvm_ir_translator */
//---------------------------------------------------------------------------------------------------------------

