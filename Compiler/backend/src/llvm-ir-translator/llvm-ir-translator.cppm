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

// for read 
#include <boost/json.hpp>

#include <cstdlib>
#include <iterator>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <tuple>

#include "create-basic-node.hpp"

// TODO: remove iostream
#include <iostream>

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module llvm_ir_translator;

//---------------------------------------------------------------------------------------------------------------

import nametable;
import functions_table;
import libc_standart_functions;
import thelast;
import compiler_exceptions;

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator
{

//---------------------------------------------------------------------------------------------------------------

struct llvmIrTranslatorData
{
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    Nametable nametable;
    functions_table::FunctionsTable functable;
    llvm::BasicBlock* current_function;
    ValueStatus current_scope_status;

    LibcStandartFunctions libc_standart_functions;

    llvmIrTranslatorData(std::filesystem::path const &source) :
        context(), module(source.string(), context), builder(context),
        nametable(module, builder), functable(builder), libc_standart_functions(module, builder),
        current_function(nullptr), current_scope_status(ValueStatus::global)
    {
        module.setTargetTriple(llvm::sys::getDefaultTargetTriple());
    }

    void set_function(llvm::BasicBlock* function)
    {
        current_function = function;
        builder.SetInsertPoint(function);
    }
};

//---------------------------------------------------------------------------------------------------------------

llvm::Value* create_null_Int32(llvmIrTranslatorData& data)
{
    return llvm::ConstantInt::get(data.builder.getInt32Ty(), 0, true);;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value* convert_to_Int1(llvmIrTranslatorData& data, llvm::Value *value)
{
    assert(value);
    LOGINFO("paracl: ir translator: converting value to i1 type");
    auto&& value_type = value->getType();
    auto&& zero = llvm::ConstantInt::get(value_type, 0, true);
    return data.builder.CreateICmpNE(value, zero, "__toBool");
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value* convert_Int1_to_Int32(llvmIrTranslatorData& data, llvm::Value* value, std::string_view description = "__convertInt1ToInt32")
{
    assert(value);
    return data.builder.CreateZExt(value, data.builder.getInt32Ty(), description);
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace compiler::llvm_ir_translator */
//---------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
namespace last::node
{
//-----------------------------------------------------------------------------

using compiler::llvm_ir_translator::llvmIrTranslatorData;
using compiler::llvm_ir_translator::ValueStatus;

//-----------------------------------------------------------------------------

using generatable_statement  = void (llvmIrTranslatorData&);
using generatable_expression = llvm::Value* (llvmIrTranslatorData&);
using generatable_if_statement = void(llvmIrTranslatorData&, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*);

static_assert(not std::is_same_v<generatable_statement, generatable_expression>   , "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_statement, generatable_if_statement> , "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_if_statement, generatable_expression>, "visit specializations must be diferent");

//-----------------------------------------------------------------------------

decltype(auto) generate_statement(BasicNode const & node, llvmIrTranslatorData& data)
{ return visit<void, llvmIrTranslatorData&>(node, data); }

decltype(auto) generate_expression(BasicNode const & node, llvmIrTranslatorData& data)
{ return visit<llvm::Value*, llvmIrTranslatorData&>(node, data); }

decltype(auto) generate_if_statement(BasicNode const & node, llvmIrTranslatorData& data, llvm::BasicBlock* self_condition, llvm::BasicBlock* self_body, llvm::BasicBlock* next, llvm::BasicBlock* end)
{ return visit<void, llvmIrTranslatorData&, llvm::BasicBlock*>(node, data, self_condition, self_body, next, end); }

//-----------------------------------------------------------------------------
namespace visit_specializations
{
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// NUMBER LITERAL
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(NumberLiteral const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: number literal: {}", node.value());
    auto&& tmp = llvm::ConstantInt::get(data.builder.getInt32Ty(), node.value());
    return tmp;
}

template <>
void visit(NumberLiteral const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<NumberLiteral, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// STRING LITERAL
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(StringLiteral const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: string literal: \"{}\"", node.value());
    return data.builder.CreateGlobalStringPtr(node.value(), "__stringLiteral");
}

template <>
void visit(StringLiteral const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<StringLiteral, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Variable const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: variable access: '{}'", node.name());
    auto&& variable = data.nametable.get(node.name());
    if (not variable) throw compiler::exceptions::using_undeclarated_variable_error(node);

    if (not data.nametable.is_visible_from(node.name(), data.current_function->getParent()))
        throw compiler::exceptions::using_variable_from_parent_function_scope_error(node);

    return variable;
}

template <>
void visit(Variable const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<Variable, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// SCAN
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Scan const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: scan expression");

    auto&& scanf_res_alloca = data.builder.CreateAlloca(data.builder.getInt32Ty(), nullptr, "__scanfResAlloca");
    auto&& fmt = data.builder.CreateGlobalStringPtr("%d", "__scanfFormat");
    auto&& scanf_args = std::vector<llvm::Value*>{fmt, scanf_res_alloca};

    data.builder.CreateCall(data.libc_standart_functions.libc_scanf(), scanf_args, "__scanfExitCode");

    return data.builder.CreateLoad(data.builder.getInt32Ty(), scanf_res_alloca, "__scanfRes");
}

template <>
void visit(Scan const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<Scan, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// UNARY OPERATOR
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(UnaryOperator const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: unary operation");

    auto&& arg = generate_expression(node.arg(), data);

    switch (node.type())
    {
        case UnaryOperator::PLUS:
            return arg;
        case UnaryOperator::MINUS:
        {
            auto&& zero = compiler::llvm_ir_translator::create_null_Int32(data);
            return data.builder.CreateSub(zero, arg, "__unaryMinus");
        }
        case UnaryOperator::NOT:
        {
            auto&& zero = compiler::llvm_ir_translator::create_null_Int32(data);
            auto&& cmp = data.builder.CreateICmpEQ(arg, zero, "__tmpUnaryNotCmpWithZero");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__unaryNot");
        }
        default:
            __builtin_unreachable();
    }
}

template <>
void visit(UnaryOperator const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<UnaryOperator, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// BINARY OPERATOR
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(BinaryOperator const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: binary operation");

    if (node.type() == BinaryOperator::ASGN)
    {
        auto&& variable = static_cast<Variable const &>(node.larg());
        auto&& right = generate_expression(node.rarg(), data);
        data.nametable.set(variable.name(), right, data.current_scope_status);

        return right;
    }

    auto&& left  = generate_expression(node.larg(), data);
    auto&& right = generate_expression(node.rarg(), data);

    switch (node.type())
    {
        case BinaryOperator::ADD: return data.builder.CreateAdd (left, right, "__add");
        case BinaryOperator::SUB: return data.builder.CreateSub (left, right, "__sub");
        case BinaryOperator::MUL: return data.builder.CreateMul (left, right, "__mul");
        case BinaryOperator::DIV: return data.builder.CreateSDiv(left, right, "__div");
        case BinaryOperator::REM: return data.builder.CreateSRem(left, right, "__rem");
        case BinaryOperator::AND:
        {
            auto&&  left_Int1 = compiler::llvm_ir_translator::convert_to_Int1(data, left);
            auto&& right_Int1 = compiler::llvm_ir_translator::convert_to_Int1(data, right);

            auto&& and_result = data.builder.CreateAnd(left_Int1, right_Int1, "__andBool");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, and_result, "__and");
        }
        case BinaryOperator::OR:
        {
            auto&&  left_Int1 = compiler::llvm_ir_translator::convert_to_Int1(data, left);
            auto&& right_Int1 = compiler::llvm_ir_translator::convert_to_Int1(data, right);

            auto&& or_result = data.builder.CreateOr(left_Int1, right_Int1, "__orBool");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, or_result, "__or");
        }
        case BinaryOperator::ISAB:
        {
            auto&& cmp = data.builder.CreateICmpSGT(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_ab");
        }
        case BinaryOperator::ISABE:
        {
            auto&& cmp = data.builder.CreateICmpSGE(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_abe");
        }
        case BinaryOperator::ISLS:
        {
            auto&& cmp = data.builder.CreateICmpSLT(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_ls");
        }
        case BinaryOperator::ISLSE:
        {
            auto&& cmp = data.builder.CreateICmpSLE(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_lse");
        }
        case BinaryOperator::ISEQ:
        {
            auto&& cmp = data.builder.CreateICmpEQ(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_eq");
        }
        case BinaryOperator::ISNE:
        {
            auto&& cmp = data.builder.CreateICmpNE(left, right, "__cmpTmp");
            return compiler::llvm_ir_translator::convert_Int1_to_Int32(data, cmp, "__cmp_ne");
        }
        default: break;
    }

    auto&& variable = static_cast<Variable const &>(node.larg());
    auto&& name     = variable.name();
    auto&& value    = data.nametable.get(name);

    switch(node.type())
    {
        case BinaryOperator::ASGN:    break;
        case BinaryOperator::ADDASGN: right = data.builder.CreateAdd (value, right, "__addAsgnResult"); break;
        case BinaryOperator::SUBASGN: right = data.builder.CreateSub (value, right, "__subAsgnResult"); break;
        case BinaryOperator::MULASGN: right = data.builder.CreateMul (value, right, "__mulAsgnResult"); break;
        case BinaryOperator::DIVASGN: right = data.builder.CreateSDiv(value, right, "__divAsgnResult"); break;
        case BinaryOperator::REMASGN: right = data.builder.CreateSRem(value, right, "__remAsgnResult"); break;
        default: __builtin_unreachable();
    }

    data.nametable.set(name, right, data.current_scope_status);
    return right;
}

template <>
void visit(BinaryOperator const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<BinaryOperator, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// PRINT
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Print const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating print statement");

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
            printf_args.push_back(generate_expression(arg, data));
            fmt << "%d";
        }
    }

    fmt << "\n";

    auto&& fmt_str = data.builder.CreateGlobalStringPtr(fmt.str(), "__printfFormat");
    printf_args.insert(printf_args.begin(), fmt_str);

    return data.builder.CreateCall(data.libc_standart_functions.libc_printf(), printf_args, "__printfExitCode");
}

//-----------------------------------------------------------------------------

template <>
void visit(Print const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<Print, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// WHILE
//-----------------------------------------------------------------------------
template <>
void visit(While const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating while loop");

    auto&& current_func = data.current_function->getParent();

    auto&& cond_block = llvm::BasicBlock::Create(data.context, "while", current_func);
    auto&& body_block = llvm::BasicBlock::Create(data.context, "do", current_func);
    auto&& end_block = llvm::BasicBlock::Create(data.context, "done", current_func);

    data.builder.CreateBr(cond_block);
    data.builder.SetInsertPoint(cond_block);

    auto&& cond_val = generate_expression(node.condition(), data);
    auto&& cond_i1 = data.builder.CreateICmpNE(cond_val, 
        compiler::llvm_ir_translator::create_null_Int32(data), "__whileCondition");

    data.builder.CreateCondBr(cond_i1, body_block, end_block);

    data.builder.SetInsertPoint(body_block);
    generate_statement(node.body(), data);
    data.builder.CreateBr(cond_block);

    data.builder.SetInsertPoint(end_block);
}

//-----------------------------------------------------------------------------
// IF
//-----------------------------------------------------------------------------

template <>
void visit(If const& node, llvmIrTranslatorData& data, llvm::BasicBlock* self_condition, llvm::BasicBlock* self_body, llvm::BasicBlock* next, llvm::BasicBlock* end)
{
    /* expect, thah SetInsertPoint already done */

    LOGINFO("paracl: ir translator: generating if condition with status");

    data.builder.SetInsertPoint(self_condition);
    auto&& if_condition = generate_expression(node.condition(), data);

    auto&& if_condition_bool = data.builder.CreateICmpNE(if_condition, 
        compiler::llvm_ir_translator::create_null_Int32(data), "__condition");

    data.builder.CreateCondBr(if_condition_bool, self_body, next ? next : end);

    data.builder.SetInsertPoint(self_body);
    generate_statement(node.body(), data);
    data.builder.CreateBr(end);
}

//-----------------------------------------------------------------------------
// ELSE
//-----------------------------------------------------------------------------
template <>
void visit(Else const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating else statement");
    generate_statement(node.body(), data);
}

//-----------------------------------------------------------------------------
// CONDITION
//-----------------------------------------------------------------------------

template <>
void visit(Condition const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating condition (if-else if-else)");

    auto&& current_func = data.current_function->getParent();

    auto&& ifs = node.get_ifs();    
    size_t ifs_size = ifs.size();

    auto&& if_condition_blocks = std::vector<llvm::BasicBlock*>{}; if_condition_blocks.reserve(ifs_size);
    auto&& if_body_blocks = std::vector<llvm::BasicBlock*>{}; if_body_blocks.reserve(ifs_size);


    if_condition_blocks.push_back(llvm::BasicBlock::Create(data.context, "if", current_func));
    if_body_blocks.push_back(llvm::BasicBlock::Create(data.context, "then", current_func));

    for (auto&& it = 1LU, ite = ifs_size; it != ite; ++it)
    {
        if_condition_blocks.push_back(llvm::BasicBlock::Create(data.context, "else-if", current_func));
        if_body_blocks.push_back(llvm::BasicBlock::Create(data.context, "then", current_func));
    }

    auto&& else_block = node.has_else() ? llvm::BasicBlock::Create(data.context, "else", current_func) : nullptr;
    auto&& condition_end = llvm::BasicBlock::Create(data.context, "fi", current_func);

    if (ifs_size == 0)
        throw std::logic_error("No if-block in condition");

    data.builder.CreateBr(if_condition_blocks[0]);

    if (ifs_size == 1)
    {
        generate_if_statement(ifs[0], data, if_condition_blocks[0], if_body_blocks[0], else_block, condition_end);
    }
    else
    {
        for (auto&& it = 0LU, ite = ifs_size - 1; it != ite; ++it)
            generate_if_statement(ifs[it], data, if_condition_blocks[it], if_body_blocks[it], if_condition_blocks[it + 1], condition_end);

        generate_if_statement(ifs[ifs_size - 1], data, if_condition_blocks[ifs_size - 1], if_body_blocks[ifs_size - 1], else_block, condition_end);
    }

    /* create else basic block always, cause its simply */
    if (else_block)
    {
        data.builder.SetInsertPoint(else_block);
        generate_statement(node.get_else(), data);
        data.builder.CreateBr(condition_end);
    }

    data.builder.SetInsertPoint(condition_end);
}

//-----------------------------------------------------------------------------
// SCOPE
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Scope const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating scope");

    data.nametable.new_scope();

    auto&& size = node.size();

    if (size == 0)
        return nullptr;

    for (auto&& it = 0LU, ite = size - 1; it != ite; ++it)
        generate_statement(node[it], data);

    auto&& last = node.back();
    auto&& return_value = static_cast<llvm::Value*>(nullptr);

    if (last.supports<generatable_expression>())
    {
        return_value = generate_expression(last, data);
    }
    else
    {
        generate_statement(last, data);
        return_value = nullptr;
    }

    data.nametable.leave_scope();
    return return_value;
}

//-----------------------------------------------------------------------------

template <>
void visit(Scope const& node, llvmIrTranslatorData& data)
{
    std::ignore = visit<Scope, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// RETURN
//-----------------------------------------------------------------------------
template <>
void visit(Return const & node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: return");

    auto&& returning_value = generate_expression(node.expression(), data);
    data.builder.CreateRet(returning_value);
}

//-----------------------------------------------------------------------------
// FUNCTION DECLARATION
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(FunctionDeclaration const & node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: func decl");
    auto&& old_status = ValueStatus{data.current_scope_status};
    data.current_scope_status = ValueStatus::local;

    auto&& args = node.args();
    /* all function return int32 */
    auto&& return_type = data.builder.getInt32Ty();
    /* all args types are int32 */
    auto&& args_types = std::vector<llvm::Type*>(args.size(), data.builder.getInt32Ty());  
    /* all functions return int32 */
    auto&& type = llvm::FunctionType::get(std::move(return_type), std::move(args_types), false);
    /* force_declare function in ir with mangled name, cause function overload must be supported */
    auto&& mangled_name = compiler::llvm_ir_translator::functions_table::FunctionsTable::mangle_name(node.name(), args);

    auto&& function = llvm::Function::Create(std::move(type), llvm::Function::InternalLinkage, mangled_name, data.module);

    auto&& old_function = static_cast<llvm::BasicBlock*>(data.current_function);
    auto&& entry_block = llvm::BasicBlock::Create(data.context,  mangled_name + "-entry", function);
    data.set_function(entry_block);

    /* set  arguments name and put it in nametable */
    auto&& func_args = function->args(); assert(std::distance(func_args.begin(), func_args.end()) == args.size());

    if (not node.name().empty())
        data.functable.declare(node, function);

    /* create scope for args */
    data.nametable.new_scope();

    for (auto&& it = 0LU, ite = args.size(); it != ite; ++it)
    {
        auto&& func_args_it = func_args.begin() + it;
        func_args_it->setName(args[it]);
        data.nametable.force_declare(args[it], func_args_it);
    }

    assert(node.body().is_a<Scope>());
    auto&& body = generate_expression(node.body(), data);

    /*
        if body == nullptr, we expect that the last statement in function scope
        is Return, so we dont need to create with our hands.
        but if scope returned a value, that means last was an expression
        and we need to return it
    */

    if (body)
        data.builder.CreateRet(body);

    /* leave scope for function args */
    data.nametable.leave_scope();

    /* back into the main */
    data.set_function(old_function);

    data.current_scope_status = old_status;

    return function;
}

//-----------------------------------------------------------------------------

template <>
void visit(FunctionDeclaration const & node, llvmIrTranslatorData& data)
{
    std::ignore = visit<FunctionDeclaration, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// FUNCTION CALL
//-----------------------------------------------------------------------------

template <>
llvm::Value* visit(FunctionCall const & node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: func call");

    auto&& args = node.args();
    auto&& name = node.name();
    auto&& completed_args = std::vector<llvm::Value*>{};
    completed_args.reserve(args.size());

    for (auto&& arg: args)
    {
        auto&& arg_value = generate_expression(arg, data);
        completed_args.push_back(std::move(arg_value));
    }

    auto&& funccall = data.functable.call(name, completed_args);
    if (funccall) return funccall;

    auto&& value = data.nametable.get(name);
    if (not value)
        throw compiler::exceptions::using_undeclarated_function(node);

    auto&& function = llvm::dyn_cast<llvm::Function>(value);

    if (not function)
        throw compiler::exceptions::using_int_as_function_error(node);

    if (function->arg_size() != completed_args.size())
        throw compiler::exceptions::function_alias_arguments_mismatch_error(node, function, completed_args);

    return data.builder.CreateCall(function, completed_args, name);;
}

//-----------------------------------------------------------------------------

template <>
void visit(FunctionCall const & node, llvmIrTranslatorData& data)
{
    std::ignore = visit<FunctionCall, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------

} /* namespace visit_specializations */
//-----------------------------------------------------------------------------
} /* namespace last::node */
//-----------------------------------------------------------------------------

SPECIALIZE_CREATE(last::node::Print              , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::Scan               , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::Variable           , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::NumberLiteral      , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::StringLiteral      , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::UnaryOperator      , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::BinaryOperator     , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::While              , last::node::dumpable, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::If                 , last::node::dumpable, last::node::generatable_if_statement)
SPECIALIZE_CREATE(last::node::Else               , last::node::dumpable, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Condition          , last::node::dumpable, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Scope              , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::FunctionDeclaration, last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::FunctionCall       , last::node::dumpable, last::node::generatable_statement, last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::Return             , last::node::dumpable, last::node::generatable_statement)

//---------------------------------------------------------------------------------------------------------------

#define THELAST_READ_AST_NO_INCLUDES
#include "read-ast.hpp"

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator
{

export
void generate_llvm_ir(std::filesystem::path const & ast_text_representation, 
                      std::filesystem::path const & ir_file)
{
    LOGINFO("paracl: ir translator: starting translation from AST to LLVM IR");

    auto&& ast = last::read(ast_text_representation); last::dump(ast, "ast.dot", "ast.svg");
    auto&& data = llvmIrTranslatorData{ast_text_representation};

    LOGINFO("paracl: ir translator: generating main function");

    auto&& main_type = llvm::FunctionType::get(data.builder.getInt32Ty(), false);
    auto&& current_function = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", data.module);

    auto&& entry_block = llvm::BasicBlock::Create(data.context, "entry", current_function);
    data.current_function = entry_block;
    data.builder.SetInsertPoint(entry_block);

    last::node::generate_statement(ast.root(), data);

    data.builder.CreateRet(compiler::llvm_ir_translator::create_null_Int32(data));

    // if (llvm::verifyModule(data.module, &llvm::errs()))
    // {
    //     LOGERR("paracl: ir translator: module verification failed");
    //     throw std::runtime_error("IR module verification failed");
    // }

    LOGINFO("paracl: ir translator: writing IR to file: {}", ir_file.string());

    auto&& ec = std::error_code{};
    llvm::ToolOutputFile out(ir_file.string(), ec, llvm::sys::fs::OF_None);
    if (ec) throw std::runtime_error("failed to open IR file: " + ec.message());

    data.module.print(out.os(), nullptr);
    out.keep();

    LOGINFO("paracl: ir translator: compiling IR to object file: {}", object_file.string());
}

//-----------------------------------------------------------------------------
} /* namespace compiler::llvm_ir_translator */
//-----------------------------------------------------------------------------
