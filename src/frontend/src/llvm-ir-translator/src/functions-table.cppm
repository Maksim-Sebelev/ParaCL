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
#include <iterator>

#if not defined(NDEBUG)
#include <iostream>
#endif /* not defined(NDEBUG) */

export module functions_table;

export import thelast;
import frontend_errors;

namespace ParaCL::frontend::llvm_ir_translator::functions_table
{

using call_args_t = std::vector<llvm::Value*>;

class declaration_args_t
{
private:
    size_t args_quantity_ = 0;
public:
    declaration_args_t() = default;

    template <typename It>
    requires (std::forward_iterator<It>)
    declaration_args_t(It begin, It end) :
        args_quantity_(std::distance(begin, end))
    {}

    declaration_args_t(ast::node::FunctionDeclaration const & funcdecl) :
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

} /* namespace ParaCL::frontend::llvm_ir_translator::functions_table */


/*
я честно говоря не знаю, ужасно ли то, что я делаю дальше
но мне нужно специализировать std::hash для моей таблицы, и вот оно:
*/
namespace std
{

template <>
struct hash<ParaCL::frontend::llvm_ir_translator::functions_table::declaration_args_t>
{
    size_t operator () (ParaCL::frontend::llvm_ir_translator::functions_table::declaration_args_t const & x) const
    {return x.args_quantity(); }
};

} /* namespace std */

namespace ParaCL::frontend::llvm_ir_translator::functions_table
{
export
class FunctionsTable final
{
private:
    struct function_info_t
    {
        llvm::Function* value = nullptr;

        // FIXME: deep copy here. i didnt find way, how to do here reference.
        // also i didnt want to use raw pointer here.
        ast::node::FunctionDeclaration declaration_node;

        bool used : 1 = false;
    };
private:
    using function_overload_set =
    std::unordered_map
    <
      declaration_args_t, /* functoin args */
      function_info_t
    //   llvm::Function* /* function llvm representation */
    >;

    using functions_table =
    std::unordered_map
    <
        std::string_view, /* function name */
        function_overload_set /* all functions with the same name*/
    >;

private:
    functions_table functions_;
    llvm::IRBuilder<>& builder_;

private:
    function_info_t * lookup_(std::string_view name, declaration_args_t const & args)
    {
        auto&& overload_set = functions_.find(name);
        if (overload_set == functions_.end())
            return nullptr;

        auto&& function = (overload_set->second).find(args);

        if (function == (overload_set->second).end())
            return nullptr;

        return std::addressof(function->second);
    }

public:
    void check_unused_call_names() const noexcept
    {
        for (auto&& overload_set: functions_)
        {
            for (auto&& function: overload_set.second)
            {
                auto&& function_info = function.second;
                if (function_info.used or function_info.declaration_node.name().empty()) continue;
                warning::unused_function_call_name(function_info.declaration_node);
            }
        }
    }

public:
    static std::string mangle_name(std::string_view base_name, declaration_args_t const & args)
    {
        if (not base_name.empty())
            return "__" + std::string(base_name) + "_" + std::to_string(args.args_quantity());

        static auto&& no_named_function_counter = 0U;
        return "__" + std::to_string(no_named_function_counter++);
    }

public:
    FunctionsTable(llvm::IRBuilder<>& builder) :
        builder_(builder)
    {}

public:
    void declare(ast::node::FunctionDeclaration const & funcdecl, llvm::Function* funcvalue)
    {
        static auto&& no_name_functions_counter = 0LU;

        auto&& declname = funcdecl.name();

        if (lookup_(declname, funcdecl))
            throw error::redeclaration_of_function(funcdecl);

        auto&& declargs = declaration_args_t{funcdecl};
        auto&& function = functions_[declname][declargs];

        function.value = funcvalue;
        function.declaration_node = funcdecl;
    }

    llvm::Value* call(std::string_view name, call_args_t const & args)
    {
        auto&& function = lookup_(name, args);
        if (not function) return nullptr;

        function->used = true;
        return builder_.CreateCall(function->value, args, mangle_name(name, args));
    }

#if not defined(NDEBUG)
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
#endif /* not defined(NDEBUG) */
};

#if not defined(NDEBUG)
export void dump(FunctionsTable const & t);
#endif /* not defined(NDEBUG) */

} /* namespace ParaCL::frontend::llvm_ir_translator::functions_table */
