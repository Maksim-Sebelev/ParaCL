module;
//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>

#include <cstdlib>
#include <iterator>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <iterator>
#include <set>

#if not defined(NDEBUG)
#include <iostream>
#endif /* not defined(NDEBUG) */

//---------------------------------------------------------------------------------------------------------------
export module llvm_ir_translator;
//---------------------------------------------------------------------------------------------------------------

import thelast;
import frontend_errors;
import nodes_functional;
export import options;
export import llvm_ir_translator_context;

//---------------------------------------------------------------------------------------------------------------
namespace ParaCL::frontend::llvm_ir_translator
{
//---------------------------------------------------------------------------------------------------------------

llvm::Value* create_null_Int32(llvmIrTranslatorContext& context)
{
    return llvm::ConstantInt::get(context.builder.getInt32Ty(), 0, true);;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value* convert_to_Int1(llvmIrTranslatorContext& context, llvm::Value *value)
{
    assert(value);
    auto&& value_type = value->getType();
    auto&& zero = llvm::ConstantInt::get(value_type, 0, true);
    return context.builder.CreateICmpNE(value, zero, "__toBool");
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value* convert_Int1_to_Int32(llvmIrTranslatorContext& context, llvm::Value* value, std::string_view description = "__convertInt1ToInt32")
{
    assert(value);
    return context.builder.CreateZExt(value, context.builder.getInt32Ty(), description);
}

//---------------------------------------------------------------------------------------------------------------

bool is_semicolon_or_empty_scope(ast::node::BasicNode const & node)
{
    return node.is_a<ast::node::Semicolon>() or (node.is_a<ast::node::Scope>() and static_cast<ast::node::Scope const &>(node).empty());
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::frontend::llvm_ir_translator */
//---------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
namespace ParaCL::frontend::ast::node
{
//-----------------------------------------------------------------------------

using frontend::llvm_ir_translator::llvmIrTranslatorContext;
using frontend::llvm_ir_translator::ValueStatus;

//-----------------------------------------------------------------------------

decltype(auto) generate_statement(BasicNode const & node, llvmIrTranslatorContext& context)
{ return visit<void, llvmIrTranslatorContext&>(node, context); }

decltype(auto) generate_expression(BasicNode const & node, llvmIrTranslatorContext& context)
{ return visit<llvm::Value*, llvmIrTranslatorContext&>(node, context); }

decltype(auto) generate_if_statement(BasicNode const & node, llvmIrTranslatorContext& context, llvm::BasicBlock* self_condition, llvm::BasicBlock* self_body, llvm::BasicBlock* next, llvm::BasicBlock* end)
{ return visit<void, llvmIrTranslatorContext&, llvm::BasicBlock*>(node, context, self_condition, self_body, next, end); }

//-----------------------------------------------------------------------------
namespace visit_specializations
{
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// NUMBER LITERAL
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(NumberLiteral const& node, llvmIrTranslatorContext& context)
{
    return llvm::ConstantInt::get(context.builder.getInt32Ty(), node.value());
}

template <>
void visit(NumberLiteral const& node, llvmIrTranslatorContext& context)
{
    warning::expression_result_unused(node.location());
    std::ignore = visit<NumberLiteral, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// STRING LITERAL
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(StringLiteral const& node, llvmIrTranslatorContext& context)
{
    return context.builder.CreateGlobalStringPtr(node.value(), "__stringLiteral");
}

template <>
void visit(StringLiteral const& node, llvmIrTranslatorContext& context)
{
    warning::expression_result_unused(node.location());
    std::ignore = visit<StringLiteral, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Variable const& node, llvmIrTranslatorContext& context)
{
    auto&& variable = context.nametable.get(node.name());
    if (not variable)
        throw error::using_undeclarated_variable(node);

    if (context.nametable.is_function(node.name()))
        throw error::using_function_as_int(node);

    if (not context.nametable.is_visible_from(node.name(), context.current_block->getParent()))
        throw error::using_variable_from_parent_function_scope(node);

    return variable;
}

template <>
void visit(Variable const& node, llvmIrTranslatorContext& context)
{
    warning::expression_result_unused(node.location());
    std::ignore = visit<Variable, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// SCAN
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Scan const& node, llvmIrTranslatorContext& context)
{
    static auto&& fmt = context.builder.CreateGlobalStringPtr("%d", "__scanfFormat");

    auto&& scanf_res_alloca = context.builder.CreateAlloca(context.builder.getInt32Ty(), nullptr, "__scanfResAlloca");
    auto&& scanf_args = std::vector<llvm::Value*>{fmt, scanf_res_alloca};

    context.builder.CreateCall(context.libc_standart_functions.libc_scanf(), scanf_args, "__scanfExitCode");

    return context.builder.CreateLoad(context.builder.getInt32Ty(), scanf_res_alloca, "__scanfRes");
}

template <>
void visit(Scan const& node, llvmIrTranslatorContext& context)
{
    warning::expression_result_unused(node.location());
    std::ignore = visit<Scan, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// UNARY OPERATOR
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(UnaryOperator const& node, llvmIrTranslatorContext& context)
{
    auto&& arg = generate_expression(node.arg(), context);

    switch (node.type())
    {
        case UnaryOperator::PLUS:
            return arg;
        case UnaryOperator::MINUS:
        {
            auto&& zero = frontend::llvm_ir_translator::create_null_Int32(context);
            return context.builder.CreateSub(zero, arg, "__unaryMinus");
        }
        case UnaryOperator::NOT:
        {
            auto&& zero = frontend::llvm_ir_translator::create_null_Int32(context);
            auto&& cmp = context.builder.CreateICmpEQ(arg, zero, "__tmpUnaryNotCmpWithZero");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__unaryNot");
        }
        default: break;
    }
#if defined(NDEBUG)
    __builtin_unreachable();
#endif /* defined(NDEBUG) */
    throw std::runtime_error("Undefined UnaryOperator type");
}

template <>
void visit(UnaryOperator const& node, llvmIrTranslatorContext& context)
{
    warning::expression_result_unused(node.location());
    std::ignore = visit<UnaryOperator, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// BINARY OPERATOR
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(BinaryOperator const& node, llvmIrTranslatorContext& context)
{
    auto&& type = node.type();

    if (type == BinaryOperator::ASGN)
    {
        auto&& variable = static_cast<Variable const &>(node.larg());
        auto&& right = generate_expression(node.rarg(), context);
        context.nametable.set(variable.name(), right, variable, context.current_scope_status);
        return right;
    }

    auto&& left  = generate_expression(node.larg(), context);
    auto&& right = generate_expression(node.rarg(), context);

    switch (type)
    {
        case BinaryOperator::ADD: return context.builder.CreateAdd (left, right, "__add");
        case BinaryOperator::SUB: return context.builder.CreateSub (left, right, "__sub");
        case BinaryOperator::MUL: return context.builder.CreateMul (left, right, "__mul");
        case BinaryOperator::DIV: return context.builder.CreateSDiv(left, right, "__div");
        case BinaryOperator::REM: return context.builder.CreateSRem(left, right, "__rem");
        case BinaryOperator::AND:
        {
            auto&&  left_Int1 = frontend::llvm_ir_translator::convert_to_Int1(context, left);
            auto&& right_Int1 = frontend::llvm_ir_translator::convert_to_Int1(context, right);

            auto&& and_result = context.builder.CreateAnd(left_Int1, right_Int1, "__andBool");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, and_result, "__and");
        }
        case BinaryOperator::OR:
        {
            auto&&  left_Int1 = frontend::llvm_ir_translator::convert_to_Int1(context, left);
            auto&& right_Int1 = frontend::llvm_ir_translator::convert_to_Int1(context, right);

            auto&& or_result = context.builder.CreateOr(left_Int1, right_Int1, "__orBool");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, or_result, "__or");
        }
        case BinaryOperator::ISAB:
        {
            auto&& cmp = context.builder.CreateICmpSGT(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpAb");
        }
        case BinaryOperator::ISABE:
        {
            auto&& cmp = context.builder.CreateICmpSGE(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpAbe");
        }
        case BinaryOperator::ISLS:
        {
            auto&& cmp = context.builder.CreateICmpSLT(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpLs");
        }
        case BinaryOperator::ISLSE:
        {
            auto&& cmp = context.builder.CreateICmpSLE(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpLse");
        }
        case BinaryOperator::ISEQ:
        {
            auto&& cmp = context.builder.CreateICmpEQ(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpEq");
        }
        case BinaryOperator::ISNE:
        {
            auto&& cmp = context.builder.CreateICmpNE(left, right, "__cmpTmp");
            return frontend::llvm_ir_translator::convert_Int1_to_Int32(context, cmp, "__cmpNe");
        }
        default: break;
    }

    auto&& variable = static_cast<Variable const &>(node.larg());
    auto&& name     = variable.name();
    auto&& value    = context.nametable.get(name);

    switch(type)
    {
        case BinaryOperator::ADDASGN: right = context.builder.CreateAdd (value, right, "__addAsgnResult"); break;
        case BinaryOperator::SUBASGN: right = context.builder.CreateSub (value, right, "__subAsgnResult"); break;
        case BinaryOperator::MULASGN: right = context.builder.CreateMul (value, right, "__mulAsgnResult"); break;
        case BinaryOperator::DIVASGN: right = context.builder.CreateSDiv(value, right, "__divAsgnResult"); break;
        case BinaryOperator::REMASGN: right = context.builder.CreateSRem(value, right, "__remAsgnResult"); break;
        default:
#if defined(NDEBUG)
            __builtin_unreachable();
#endif /* defined(NDEBUG) */
            throw std::runtime_error("Undefined UnaryOperator type");
    }

    context.nametable.set(name, right, variable, context.current_scope_status);
    return right;
}

template <>
void visit(BinaryOperator const& node, llvmIrTranslatorContext& context)
{
    switch (node.type())
    {
        case BinaryOperator::ASGN:
        case BinaryOperator::ADDASGN:
        case BinaryOperator::SUBASGN:
        case BinaryOperator::MULASGN:
        case BinaryOperator::DIVASGN:
        case BinaryOperator::REMASGN: break;
        default: warning::expression_result_unused(node.location()); break;
    }

    std::ignore = visit<BinaryOperator, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// PRINT
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Print const& node, llvmIrTranslatorContext& context)
{
    auto&& fmt = std::ostringstream{};
    auto&& printf_args = std::vector<llvm::Value*>{};

    for (auto&& arg : node)
    {
        if (arg.is_a<StringLiteral>())
        {
            auto&& str = static_cast<StringLiteral const &>(arg);
            fmt << str.value();
        }
        else if (arg.is_a<NumberLiteral>())
        {
            auto&& number = static_cast<NumberLiteral const &>(arg);
            fmt << number.value();
        }
        else
        {
            printf_args.push_back(generate_expression(arg, context));
            fmt << "%d";
        }
    }

    fmt << "\n";

    auto&& fmt_str = static_cast<llvm::Value*>(nullptr);

    if (fmt.str() == "%d\n")
    {
        static auto&& fmt_percent_d = context.builder.CreateGlobalStringPtr(fmt.str(), "__printfFormat");
        fmt_str = fmt_percent_d;
    }
    else
    {
        fmt_str = context.builder.CreateGlobalStringPtr(fmt.str(), "__printfFormat");
    }

    printf_args.insert(printf_args.begin(), fmt_str);

    return context.builder.CreateCall(context.libc_standart_functions.libc_printf(), printf_args, "__printfExitCode");
}

//-----------------------------------------------------------------------------

template <>
void visit(Print const& node, llvmIrTranslatorContext& context)
{
    std::ignore = visit<Print, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// WHILE
//-----------------------------------------------------------------------------
template <>
void visit(While const& node, llvmIrTranslatorContext& context)
{
    auto&& current_func = context.current_block->getParent();

    auto&& cond_block = llvm::BasicBlock::Create(context.context, "while", current_func);
    auto&& body_block = llvm::BasicBlock::Create(context.context, "do", current_func);
    auto&& end_block  = llvm::BasicBlock::Create(context.context, "done", current_func);

    context.builder.CreateBr(cond_block);

    context.set_current_block(cond_block);

    auto&& cond_val = generate_expression(node.condition(), context);
    auto&& cond_i1 = context.builder.CreateICmpNE(cond_val, 
        frontend::llvm_ir_translator::create_null_Int32(context), "__whileCondition");

    context.builder.CreateCondBr(cond_i1, body_block, end_block);

    context.set_current_block(body_block);

    auto&& body = node.body();
    if (not frontend::llvm_ir_translator::is_semicolon_or_empty_scope(body))
        generate_statement(body, context);

    context.builder.CreateBr(cond_block);

    context.set_current_block(end_block);
}

//-----------------------------------------------------------------------------
// IF
//-----------------------------------------------------------------------------

template <>
void visit(If const& node, llvmIrTranslatorContext& context, llvm::BasicBlock* self_condition, llvm::BasicBlock* self_body, llvm::BasicBlock* next, llvm::BasicBlock* end)
{
    context.set_current_block(self_condition);
    auto&& if_condition = generate_expression(node.condition(), context);

    auto&& if_condition_bool = context.builder.CreateICmpNE(if_condition, 
        frontend::llvm_ir_translator::create_null_Int32(context), "__condition");

    context.builder.CreateCondBr(if_condition_bool, self_body, next ? next : end);

    context.set_current_block(self_body);

    auto&& body = node.body();
    if (not frontend::llvm_ir_translator::is_semicolon_or_empty_scope(body))
        generate_statement(body, context);
    else
        warning::useless_condition(node);

    context.builder.CreateBr(end);
    context.set_current_block(end);
}

//-----------------------------------------------------------------------------
// ELSE
//-----------------------------------------------------------------------------
template <>
void visit(Else const& node, llvmIrTranslatorContext& context)
{
    auto&& body = node.body();
    if (not frontend::llvm_ir_translator::is_semicolon_or_empty_scope(body))
        generate_statement(body, context);
    else
        warning::useless_else(node);
}

//-----------------------------------------------------------------------------
// CONDITION
//-----------------------------------------------------------------------------

template <>
void visit(Condition const& node, llvmIrTranslatorContext& context)
{
    auto&& current_func = context.current_block->getParent();

    auto&& ifs = node.get_ifs();    
    auto&& ifs_size = ifs.size();

    assert(ifs_size != 0 && "empty condition");

    struct ConditionAndBody
    {
        llvm::BasicBlock* condition;
        llvm::BasicBlock* body;
    };

    auto&& if_blocks = std::vector<ConditionAndBody>{}; if_blocks.reserve(ifs_size);

    auto&& if_condition_blocks = std::vector<llvm::BasicBlock*>{}; if_condition_blocks.reserve(ifs_size);
    auto&& if_body_blocks = std::vector<llvm::BasicBlock*>{}; if_body_blocks.reserve(ifs_size);

    auto&& push_condition = [&](std::string_view name) -> void
    {
        if_blocks.emplace_back
        (
            llvm::BasicBlock::Create(context.context, name, current_func),
            llvm::BasicBlock::Create(context.context, "then", current_func)
        );
    };

    push_condition("if");

    for (auto&& it = 1LU, ite = ifs_size; it != ite; ++it)
        push_condition("else-if");

    auto&& else_block = node.has_else() ? llvm::BasicBlock::Create(context.context, "else", current_func) : nullptr;
    auto&& condition_end = llvm::BasicBlock::Create(context.context, "fi", current_func);

    context.builder.CreateBr(if_blocks[0].condition);

    if (ifs_size == 1)
    {
        generate_if_statement(ifs[0], context, if_blocks[0].condition, if_blocks[0].body, else_block, condition_end);
    }
    else
    {
        for (auto&& it = 0LU, ite = ifs_size - 1; it != ite; ++it)
            generate_if_statement(ifs[it], context, if_blocks[it].condition, if_blocks[it].body, if_blocks[it+1].condition, condition_end);

        generate_if_statement(ifs[ifs_size - 1], context, if_blocks[ifs_size-1].condition, if_blocks[ifs_size - 1].body, else_block, condition_end);
    }

    if (else_block)
    {
        context.set_current_block(else_block);    
        generate_statement(node.get_else(), context);
        context.builder.CreateBr(condition_end);
    }

    context.set_current_block(condition_end);
}

//-----------------------------------------------------------------------------
// SCOPE
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(Scope const& node, llvmIrTranslatorContext& context)
{
    auto&& in_function = (context.current_scope_status == ValueStatus::local);

    context.nametable.new_scope();

    auto&& size = node.size();

    if (size == 0)
        throw error::using_scope_as_expression_but_its_empty(node);

    for (auto&& it = 0LU, ite = size - 1; it != ite; ++it)
    {
        auto&& statement = node[it];

        if (statement.is_a<Return>())
            warning::instructions_after_return(statement);

        generate_statement(statement, context);
    }

    auto&& last = node.back();

    auto&& return_value = static_cast<llvm::Value*>(nullptr);

    if (last.is_a<Return>())
        generate_statement(last, context);
    else if (not last.supports<generatable_expression>())
        throw error::using_scope_as_expression_but_last_statement_isnt_expression(last);
    else
        return_value = generate_expression(last, context);

    context.nametable.leave_scope();

    return return_value;
}

//-----------------------------------------------------------------------------

template <>
void visit(Scope const& node, llvmIrTranslatorContext& context)
{
    auto&& size = node.size();

    if (size == 0)
    {
        warning::empty_scope(node);
        return;
    }

    auto&& in_function = (context.current_scope_status == ValueStatus::local);

    context.nametable.new_scope();

    for (auto&& it = 0LU, ite = size - 1; it != ite; ++it)
    {
        auto&& statement = node[it];

        if (statement.is_a<Return>())
            warning::instructions_after_return(statement);

        generate_statement(statement, context);
    }

    generate_statement(node.back(), context);

    context.nametable.leave_scope();
}

//-----------------------------------------------------------------------------
// SEMICOLON
//-----------------------------------------------------------------------------

template <>
void visit(Semicolon const & node, llvmIrTranslatorContext& context)
{
    warning::useless_semicolon(node);
}

//-----------------------------------------------------------------------------
// RETURN
//-----------------------------------------------------------------------------
template <>
void visit(Return const & node, llvmIrTranslatorContext& context)
{
    auto&& not_in_function = (context.current_scope_status == ValueStatus::global);

    if (not_in_function) throw error::return_in_not_function_scope(node);

    auto&& returning_value = generate_expression(node.expression(), context);
    context.builder.CreateRet(returning_value);
}

//-----------------------------------------------------------------------------
// FUNCTION DECLARATION
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(FunctionDeclaration const & node, llvmIrTranslatorContext& context)
{
    auto&& args = node.args();

    auto&& old_status = ValueStatus{context.current_scope_status};
    context.current_scope_status = ValueStatus::local;

    /* all function return int32 */
    auto&& return_type = context.builder.getInt32Ty();
    /* all args types are int32 */
    auto&& args_types = std::vector<llvm::Type*>(args.size(), context.builder.getInt32Ty());  
    /* all functions return int32 */
    auto&& type = llvm::FunctionType::get(std::move(return_type), std::move(args_types), false);
    /* force_declare function in ir with mangled name, cause function overload must be supported */
    auto&& mangled_name = frontend::llvm_ir_translator::functions_table::FunctionsTable::mangle_name(node.name(), {args.begin(), args.end()});

    auto&& function = llvm::Function::Create(std::move(type), llvm::Function::InternalLinkage, mangled_name, context.module);

    auto&& old_basic_block = static_cast<llvm::BasicBlock*>(context.current_block);
    auto&& entry_block = llvm::BasicBlock::Create(context.context,  mangled_name + "-entry", function);
    context.set_current_block(entry_block);

    /* set  arguments name and put it in nametable */
    auto&& func_args = function->args(); assert(std::distance(func_args.begin(), func_args.end()) == args.size());

    if (not node.name().empty())
        context.functable.declare(node, function);

    /* create scope for args */
    context.nametable.new_scope();

    for (auto&& it = 0LU, ite = args.size(); it != ite; ++it)
    {
        auto&& func_args_it = func_args.begin() + it;
        //std::advance(func_args.begin(), it);

        auto&& args_it = args[it];
        auto&& name = static_cast<Variable const &>(args_it).name();

        func_args_it->setName(name);
        context.nametable.force_declare(name, func_args_it, args_it);
    }

    assert(node.body().is_a<Scope>());

    auto&& body = static_cast<Scope const &>(node.body());

    if (body.empty())
        throw error::function_with_empty_body(node);

    auto&& last_statement = body.back();

    if ((not last_statement.is_a<Return>()) and (not last_statement.supports<generatable_expression>()))
        throw error::last_function_statement_is_not_return_and_cannot_be_converted_to_expression(node, last_statement);

    auto&& body_return = generate_expression(node.body(), context);

    if (body_return) context.builder.CreateRet(body_return);

    /* leave scope for function args */
    context.nametable.leave_scope();

    /* back into the previous settings */
    context.set_current_block(old_basic_block);
    context.current_scope_status = old_status;

    return function;
}

//-----------------------------------------------------------------------------

template <>
void visit(FunctionDeclaration const & node, llvmIrTranslatorContext& context)
{
    if (node.name().empty()) warning::unnamed_function(node);
    std::ignore = visit<FunctionDeclaration, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
// FUNCTION CALL
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(FunctionCall const & node, llvmIrTranslatorContext& context)
{
    auto&& args = node.args();
    auto&& name = node.name();

    auto&& completed_args = std::vector<llvm::Value*>{};
    completed_args.reserve(args.size());

    for (auto&& arg: args)
    {
        auto&& arg_value = generate_expression(arg, context);
        completed_args.push_back(std::move(arg_value));
    }

    auto&& funccall = context.functable.call(name, completed_args);

    if (funccall) return funccall;

    auto&& value = context.nametable.get(name);
    if (not value)
        throw error::using_undeclarated_function(node);

    auto&& function = llvm::dyn_cast<llvm::Function>(value);

    if (not function)
        throw error::using_int_as_function(node);

    if (function->arg_size() != completed_args.size())
        throw error::function_alias_arguments_mismatch(node, function->arg_size(), completed_args.size());

    return context.builder.CreateCall(function, completed_args, name);;
}

//-----------------------------------------------------------------------------

template <>
void visit(FunctionCall const & node, llvmIrTranslatorContext& context)
{
    warning::function_return_value_ignored(node);
    std::ignore = visit<FunctionCall, llvm::Value*, llvmIrTranslatorContext&>(node, context);
}

//-----------------------------------------------------------------------------
} /* namespace visit_specializations */
//-----------------------------------------------------------------------------
} /* namespace ParaCL::frontend::ast::node */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
namespace ParaCL::frontend::llvm_ir_translator
{
//-----------------------------------------------------------------------------

export
void generate_llvm_ir(ast::AST const & ast, options::Options const & options)
{
    auto&& context = llvmIrTranslatorContext{options.input_file};

    auto&& main_type = llvm::FunctionType::get(context.builder.getInt32Ty(), false);
    auto&& main_function = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", context.module);

    auto&& entry_block = llvm::BasicBlock::Create(context.context, "main-entry", main_function);

    context.set_current_block(entry_block);

    ast::node::generate_statement(ast.root(), context);

    context.builder.CreateRet(frontend::llvm_ir_translator::create_null_Int32(context));

    context.functable.check_unused_call_names();

    auto&& ec = std::error_code{};

    auto&& ir_file = options.tmp_ir_file;

    auto&& out = llvm::ToolOutputFile{ir_file.string(), ec, llvm::sys::fs::OF_None};
    if (ec) throw std::runtime_error("failed to open IR file: " + ec.message());

    context.module.print(out.os(), nullptr);
    out.keep();
}

//-----------------------------------------------------------------------------
} /* namespace ParaCL::frontend::llvm_ir_translator */
//-----------------------------------------------------------------------------
