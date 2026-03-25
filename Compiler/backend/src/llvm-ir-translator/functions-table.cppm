module;

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>

#include <unordered_map>
#include <vector>
#include <cstddef>
#include <string_view>
#include <string>
#include <stdexcept>

// TODO: remove iostream
#include <iostream>

export module functions_table;

export import thelast;


namespace compiler::llvm_ir_translator::functions_table
{

using call_args_t = std::vector<llvm::Value*>;

class declaration_args_t
{
private:
    size_t args_quantity_ = 0;
public:
    declaration_args_t() = default;

    declaration_args_t(std::vector<std::string> const & declargs) :
        args_quantity_(declargs.size())
    {}

    declaration_args_t(last::node::FunctionDeclaration const & funcdecl) :
        args_quantity_(funcdecl.args().size())
    {}

    declaration_args_t(call_args_t const & call_args) :
        args_quantity_(call_args.size())
    {}

    size_t args_quantity() const noexcept
    { return args_quantity_; }
};

bool operator == (declaration_args_t const & lhs, declaration_args_t const & rhs)
{
    return
        (lhs.args_quantity() == rhs.args_quantity());
}

/* call_matches_declaration_by_args */
bool operator == (declaration_args_t const & declaration_args, call_args_t const & call_args)
{
    /* we can compare only sizes, cause we have only 1 type in language (int32) */
    return
        (call_args.size() == declaration_args.args_quantity());
}

} /* namespace compiler::llvm_ir_translator::functions_table */


/*
я честно говоря не знаю, ужасно ли то, что я делаю дальше
но мне нужно специализировать std::hash для моей таблицы, и вот оно:
*/
namespace std
{

template <>
struct hash<compiler::llvm_ir_translator::functions_table::declaration_args_t>
{
    size_t operator () (compiler::llvm_ir_translator::functions_table::declaration_args_t const & x) const
    {return x.args_quantity(); }
};

} /* namespace std */

namespace compiler::llvm_ir_translator::functions_table
{
export
class FunctionsTable final
{
private:
    using function_overload_set = std::unordered_map
                                  <
                                    declaration_args_t /* functoin args */,
                                    llvm::Function* /* function llvm representation */
                                  >;

    using functions_table = std::unordered_map
                            <
                                std::string_view /* function name */,
                                function_overload_set /* all functions with the same name*/
                            >;
private:
    functions_table functions_;
    llvm::IRBuilder<>& builder_;
    
private:
    llvm::Function * lookup_(std::string_view name, declaration_args_t const & args)
    {
        auto&& overload_set = functions_.find(name);
        if (overload_set == functions_.end())
            return nullptr;

        auto&& function = (overload_set->second).find(args);

        if (function == (overload_set->second).end())
            return nullptr;

        return function->second;
    }

public:
    static std::string mangle_name(std::string_view base_name, declaration_args_t const & args)
    {
        return "__" + std::string(base_name) + "_" + std::to_string(args.args_quantity());
    }

public:
    FunctionsTable(llvm::IRBuilder<>& builder) :
        builder_(builder)
    {}

public:
    void declare(last::node::FunctionDeclaration const & funcdecl, llvm::Function* funcvalue)
    {
        static auto&& no_name_functions_counter = 0LU;

        auto&& declname = funcdecl.name();

        if (declname.empty())
            throw std::logic_error("declare function without name. So, i actually can help you here");

        if (lookup_(declname, funcdecl))
            throw std::logic_error("Redeclaration of '" + std::string(declname) + "'");

        auto&& declargs = declaration_args_t{funcdecl};
        functions_[declname][declargs] = funcvalue;
    }

    llvm::Function* get(std::string_view name, call_args_t const & args)
    {
        return lookup_(name, args);
    }

    llvm::Value* call(std::string_view name, call_args_t const & args)
    {
        auto&& function = lookup_(name, args);
        if (not function)
            return nullptr;
        return builder_.CreateCall(function, args, mangle_name(name, args));
    }

    friend void dump(FunctionsTable const & t)
    {
        static int n = 0;
        std::cout << "FUNCTABLE DUMP[" << n++ << "]\n{" << std::endl;
        for (auto&& overload_set: t.functions_)
        {
            std::cout << "\t" << overload_set.first << "\n\t{" << std::endl;
            for (auto&& func: overload_set.second)
            {
                std::cout << "\t\t" << mangle_name(overload_set.first, func.first) << std::endl;
            }

            std::cout << "\t}" << std::endl;
        }
        std::cout << "}\n";
    }
};

export void dump(FunctionsTable const & t);

} /* namespace compiler::llvm_ir_translator::functions_table */
