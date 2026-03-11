module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>
#include <boost/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>

#include "create-basic-node.hpp"

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module llvm_ir_translator;

//---------------------------------------------------------------------------------------------------------------

import nametable;
import libc_standart_functions;
import thelast;

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator
{

//---------------------------------------------------------------------------------------------------------------

struct llvmIrTranslatorData
{
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    nametable::Nametable nametable;
    LibcStandartFunctions libc_standart_functions;

    llvmIrTranslatorData(std::filesystem::path const &source) :
        context(), module(source.string(), context), builder(context),
        nametable(module, builder), libc_standart_functions(module, builder)
    {}
};

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler::llvm_ir_translator */
//---------------------------------------------------------------------------------------------------------------

namespace last::node
{

//-----------------------------------------------------------------------------

using compiler::llvm_ir_translator::llvmIrTranslatorData;

//-----------------------------------------------------------------------------

using generatable_statement  = void (llvmIrTranslatorData&);
using generatable_expression = llvm::Value* (llvmIrTranslatorData&);
using generatable_if_statement = void(llvmIrTranslatorData&, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*);

static_assert(not std::is_same_v<generatable_statement, generatable_expression>, "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_statement, generatable_if_statement>, "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_if_statement, generatable_expression>, "visit specializations must be diferent");

//-----------------------------------------------------------------------------

decltype(auto) generate_statement(BasicNode const & node, llvmIrTranslatorData& data)
{ return visit<void, llvmIrTranslatorData&>(node, data); }

decltype(auto) generate_expression(BasicNode const & node, llvmIrTranslatorData& data)
{ return visit<llvm::Value*, llvmIrTranslatorData&>(node, data); }

decltype(auto) generate_if_statement(BasicNode const & node, llvmIrTranslatorData& data, llvm::BasicBlock* self_condition, llvm::BasicBlock* self_body, llvm::BasicBlock* next, llvm::BasicBlock* end)
{ return visit<void, llvmIrTranslatorData&, llvm::BasicBlock*>(node, data, self_condition, self_body, next, end); }

namespace visit_specializations
{

//-----------------------------------------------------------------------------
// NUMBER LITERAL
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(NumberLiteral const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: number literal: {}", node.value());
    return llvm::ConstantInt::get(data.builder.getInt32Ty(), node.value());
}

template <>
void visit(NumberLiteral const& node, llvmIrTranslatorData& data)
{
    (void) visit<NumberLiteral, llvm::Value*, llvmIrTranslatorData&>(node, data);
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

//-----------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Variable const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: variable access: '{}'", node.name());
    return data.nametable.get_variable_value(node.name());
}

template <>
void visit(Variable const& node, llvmIrTranslatorData& data)
{
    (void) visit<Variable, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// SCAN
//-----------------------------------------------------------------------------
template <>
llvm::Value* visit(Scan const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: scan expression");

    auto&& temp_var = data.builder.CreateAlloca(data.builder.getInt32Ty(), nullptr, "__scan_tmp");
    auto&& fmt = data.builder.CreateGlobalStringPtr("%d", "__scanfFormat");
    auto&& scanf_args = std::vector<llvm::Value*>{fmt, temp_var};

    data.builder.CreateCall(data.libc_standart_functions.libc_scanf(), scanf_args, "__scanf_exit_code");

    return data.builder.CreateLoad(data.builder.getInt32Ty(), temp_var, "__scan_result");
}

template <>
void visit(Scan const& node, llvmIrTranslatorData& data)
{
    (void) visit<Scan, llvm::Value*, llvmIrTranslatorData&>(node, data);
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
            auto&& zero = llvm::ConstantInt::get(data.builder.getInt32Ty(), 0);
            return data.builder.CreateSub(zero, arg, "__unaryMinus");
        }
        case UnaryOperator::NOT:
        {
            auto&& zero = llvm::ConstantInt::get(data.builder.getInt32Ty(), 0);
            auto&& cmp = data.builder.CreateICmpEQ(arg, zero);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__unaryNot");
        }
        default:
            __builtin_unreachable();
    }
}

template <>
void visit(UnaryOperator const& node, llvmIrTranslatorData& data)
{
    (void) visit<UnaryOperator, llvm::Value*, llvmIrTranslatorData&>(node, data);
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
        data.nametable.set_value(variable.name(), right);
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
        case BinaryOperator::AND: return data.builder.CreateAnd (left, right, "__and");
        case BinaryOperator::OR:  return data.builder.CreateOr  (left, right, "__or" );
        case BinaryOperator::ISAB:
        {
            auto&& cmp = data.builder.CreateICmpSGT(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_ab");
        }
        case BinaryOperator::ISABE:
        {
            auto&& cmp = data.builder.CreateICmpSGE(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_abe");
        }
        case BinaryOperator::ISLS:
        {
            auto&& cmp = data.builder.CreateICmpSLT(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_ls");
        }
        case BinaryOperator::ISLSE:
        {
            auto&& cmp = data.builder.CreateICmpSLE(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_lse");
        }
        case BinaryOperator::ISEQ:
        {
            auto&& cmp = data.builder.CreateICmpEQ(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_eq");
        }
        case BinaryOperator::ISNE:
        {
            auto&& cmp = data.builder.CreateICmpNE(left, right);
            return data.builder.CreateZExt(cmp, data.builder.getInt32Ty(), "__cmp_ne");
        }
        default: break;
    }

    auto&& variable = static_cast<Variable const &>(node.larg());
    auto&& name  = variable.name();
    auto&& value = data.nametable.get_variable_value(name);

    switch(node.type())
    {
        case BinaryOperator::ASGN:    break;
        case BinaryOperator::ADDASGN: right = data.builder.CreateAdd (value, right, "__addAsgnResult"); break;
        case BinaryOperator::SUBASGN: right = data.builder.CreateSub (value, right, "__subAsgnResult"); break;
        case BinaryOperator::MULASGN: right = data.builder.CreateMul (value, right, "__mulAsgnResult"); break;
        case BinaryOperator::DIVASGN: right = data.builder.CreateSDiv(value, right, "__divAsgnResult"); break;
        case BinaryOperator::REMASGN: right = data.builder.CreateSRem(value, right, "__remAsgnResult"); break;
        default:
            throw std::runtime_error("Assignment operators should be handled by statement nodes");
    }

    data.nametable.set_value(name, right);
    return right;
}

template <>
void visit(BinaryOperator const& node, llvmIrTranslatorData& data)
{
    (void) visit<BinaryOperator, llvm::Value*, llvmIrTranslatorData&>(node, data);
}

//-----------------------------------------------------------------------------
// PRINT
//-----------------------------------------------------------------------------
template <>
void visit(Print const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating print statement");

    auto&& fmt = std::ostringstream{};
    auto&& printf_args = std::vector<llvm::Value*>{};

    for (auto&& arg : node)
    {
        if (arg.is_a<StringLiteral>())
            fmt << "%s";
        else /* print expect only string literals, variables and number literals, so if not %s, using %d */
            fmt << "%d";

        printf_args.push_back(generate_expression(arg, data));
    }

    fmt << "\n";

    auto&& fmt_str = data.builder.CreateGlobalStringPtr(fmt.str(), "__printfFormat");
    printf_args.insert(printf_args.begin(), fmt_str);

    data.builder.CreateCall(data.libc_standart_functions.libc_printf(), printf_args, "__printf_exit_code");
}

//-----------------------------------------------------------------------------
// WHILE
//-----------------------------------------------------------------------------
template <>
void visit(While const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating while loop");

    auto&& current_func = data.builder.GetInsertBlock()->getParent();

    auto&& cond_block = llvm::BasicBlock::Create(data.context, "while_cond", current_func);
    auto&& body_block = llvm::BasicBlock::Create(data.context, "while_body", current_func);
    auto&& end_block = llvm::BasicBlock::Create(data.context, "while_end", current_func);

    data.builder.CreateBr(cond_block);
    data.builder.SetInsertPoint(cond_block);

    auto&& cond_val = generate_expression(node.condition(), data);
    auto&& cond_i1 = data.builder.CreateICmpNE(cond_val, 
        llvm::ConstantInt::get(data.builder.getInt32Ty(), 0), "while_cond");

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
        llvm::ConstantInt::get(data.builder.getInt32Ty(), 0), "__condition");

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

    auto&& current_func = data.builder.GetInsertBlock()->getParent();

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
void visit(Scope const& node, llvmIrTranslatorData& data)
{
    LOGINFO("paracl: ir translator: generating scope");

    data.nametable.new_scope();

    for (auto&& stmt : node)
        generate_statement(stmt, data);

    data.nametable.leave_scope();
}

//-----------------------------------------------------------------------------
} /* namespace visit_specializations */
//-----------------------------------------------------------------------------
} /* namespace last::node */
//-----------------------------------------------------------------------------

SPECIALIZE_CREATE(last::node::Print         , last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Scan          , last::node::generatable_expression, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Variable      , last::node::generatable_expression, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::NumberLiteral , last::node::generatable_expression, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::StringLiteral , last::node::generatable_expression)
SPECIALIZE_CREATE(last::node::UnaryOperator , last::node::generatable_expression, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::BinaryOperator, last::node::generatable_expression, last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::While         , last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::If            , last::node::generatable_if_statement)
SPECIALIZE_CREATE(last::node::Else          , last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Condition     , last::node::generatable_statement)
SPECIALIZE_CREATE(last::node::Scope         , last::node::generatable_statement)

//---------------------------------------------------------------------------------------------------------------

#define THELAST_READ_AST_NO_INCLUDES
#include "read-ast.hpp"
#undef THELAST_READ_AST_NO_INCLUDES

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator
{

export
void generate_llvm_ir(std::filesystem::path const & ast_text_representation, 
                      std::filesystem::path const & ir_file)
{
    LOGINFO("paracl: ir translator: starting translation from AST to LLVM IR");

    auto&& ast = last::read(ast_text_representation);
    auto&& data = llvmIrTranslatorData{ast_text_representation};

    LOGINFO("paracl: ir translator: generating main function");

    auto&& main_type = llvm::FunctionType::get(data.builder.getInt32Ty(), false);
    auto&& main_function = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", data.module);

    auto&& entry_block = llvm::BasicBlock::Create(data.context, "entry", main_function);
    data.builder.SetInsertPoint(entry_block);

    data.nametable.new_scope();
    last::node::generate_statement(ast.root(), data);
    data.nametable.leave_scope();

    data.builder.CreateRet(llvm::ConstantInt::get(data.builder.getInt32Ty(), 0));

    if (llvm::verifyModule(data.module, &llvm::errs()))
    {
        LOGERR("paracl: ir translator: module verification failed");
        throw std::runtime_error("IR module verification failed");
    }

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
