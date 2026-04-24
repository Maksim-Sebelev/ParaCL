module;

#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LLVMContext.h>

#include <concepts>

export module nodes_functional;

export import llvm_ir_translator_context;

//-----------------------------------------------------------------------------
namespace ParaCL::frontend::ast::node
{
//-----------------------------------------------------------------------------

export using generatable_statement        = void (frontend::llvm_ir_translator::llvmIrTranslatorContext&);
export using generatable_expression       = llvm::Value* (frontend::llvm_ir_translator::llvmIrTranslatorContext&);
export using generatable_if_statement     = void(frontend::llvm_ir_translator::llvmIrTranslatorContext&, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*);

static_assert(not std::is_same_v<generatable_statement, generatable_expression>   , "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_statement, generatable_if_statement> , "visit specializations must be diferent");
static_assert(not std::is_same_v<generatable_if_statement, generatable_expression>, "visit specializations must be diferent");

//-----------------------------------------------------------------------------
} /* namespace ParaCL::frontend::ast::node */
//-----------------------------------------------------------------------------
