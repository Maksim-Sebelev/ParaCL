module;

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/TargetParser/Host.h>

#include <filesystem>

export module llvm_ir_translator_context;

export import nametable;
export import functions_table;
export import libc_standart_functions;

//---------------------------------------------------------------------------------------------------------------
namespace ParaCL::frontend::llvm_ir_translator
{
//---------------------------------------------------------------------------------------------------------------

export struct llvmIrTranslatorContext
{
  llvm::LLVMContext               context;
  llvm::Module                    module;
  llvm::IRBuilder<>               builder;
  Nametable                       nametable;
  functions_table::FunctionsTable functable;
  LibcStandartFunctions           libc_standart_functions;
  llvm::BasicBlock*               current_block        = nullptr;
  ValueStatus                     current_scope_status = ValueStatus::global;
  llvm::BasicBlock*               current_loop         = nullptr;
  llvm::BasicBlock*               current_loop_end     = nullptr;

  llvmIrTranslatorContext(
      std::filesystem::path const& source
  ) :
      context(),
      module(source.string(), context),
      builder(context),
      nametable(module, builder),
      functable(builder),
      libc_standart_functions(module, builder)
  {
    // llvm, what's wrong with your back compability?
    auto&& target_triple = llvm::Triple(llvm::sys::getDefaultTargetTriple());
    module.setTargetTriple(target_triple);
  }

  void
  set_current_block(
      llvm::BasicBlock* function
  )
  {
    current_block = function;
    builder.SetInsertPoint(function);
  }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::frontend::llvm_ir_translator */

//---------------------------------------------------------------------------------------------------------------
