cmake_minimum_required(VERSION 3.28)

project(ParaCL-options
    LANGUAGES CXX
    VERSION 1.0
)

# =================================================================================================

if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
else()
  if (CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD requires to be greater or equal than 20, but given ${CMAKE_CXX_STANDARD}")
  endif(CMAKE_CXX_STANDARD LESS 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# =================================================================================================
# find LLVM

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# =================================================================================================

function(add_llvm_to_target target)
    target_compile_definitions(${target}
    PUBLIC
        ${LLVM_DEFINITIONS}
    )
    target_include_directories(${target}
    PUBLIC
        ${LLVM_INCLUDE_DIRS}
    )
    target_link_libraries(${target}
    PUBLIC
        LLVM
    )
endfunction(add_llvm_to_target )

# =================================================================================================

set(PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# =================================================================================================
# nametable library for ir_translator

set(PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB paracl-frontend-llvmirtranslator-nametable)
add_library(${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB})

set(PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_SRC_DIR ${PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR})
set(PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_SRC
    ${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_SRC_DIR}/nametable.cppm
    ${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_SRC_DIR}/functions-table.cppm
)

target_sources(${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_SRC}
)

target_link_libraries(${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB}
  PUBLIC
    TheLast::TheLast
  PRIVATE
    paracl-frontend-errors
)

add_llvm_to_target(${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB})

# =================================================================================================
# libc standard functions library

set(LIBC_STANDART_FUNCTIONS_LIB paracl-frontend-llvmirtranslator-libcstandardfunctions)
add_library(${LIBC_STANDART_FUNCTIONS_LIB})

set(LIBC_STANDART_FUNCTIONS_SRC_DIR ${PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR})
set(LIBC_STANDART_FUNCTIONS_SRC
    ${LIBC_STANDART_FUNCTIONS_SRC_DIR}/libc-standard-functions.cppm
)

target_sources(${LIBC_STANDART_FUNCTIONS_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${LIBC_STANDART_FUNCTIONS_SRC}
)

add_llvm_to_target(${LIBC_STANDART_FUNCTIONS_LIB})

# =================================================================================================
# llvm ir translator context library

set(LLVM_IR_TRANSLATOR_CONTEXT_LIB paracl-frontend-llvmirtranslator-context)
add_library(${LLVM_IR_TRANSLATOR_CONTEXT_LIB})

set(LLVM_IR_TRANSLATOR_SRC
    ${PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR}/llvm-ir-translator-context.cppm
)

target_sources(${LLVM_IR_TRANSLATOR_CONTEXT_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${LLVM_IR_TRANSLATOR_SRC}
)

target_link_libraries(${LLVM_IR_TRANSLATOR_CONTEXT_LIB}
PUBLIC
    ${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB}
    ${LIBC_STANDART_FUNCTIONS_LIB}
    paracl-frontend-errors
)

add_llvm_to_target(${LLVM_IR_TRANSLATOR_CONTEXT_LIB})

# =================================================================================================
# llvm ir translator nodes functional library

set(LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB paracl-frontend-llvmirtranslator-nodesfunction)
add_library(${LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB})

set(LLVM_IR_TRANSLATOR_SRC
    ${PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR}/nodes-functional.cppm
)

target_sources(${LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${LLVM_IR_TRANSLATOR_SRC}
)

target_link_libraries(${LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB}
PUBLIC
    ${LLVM_IR_TRANSLATOR_CONTEXT_LIB}
)

add_llvm_to_target(${LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB})

# =================================================================================================
# llvm ir translator library

set(LLVM_IR_TRANSLATOR_LIB paracl-frontend-llvmirtranslator)
add_library(${LLVM_IR_TRANSLATOR_LIB})

set(LLVM_IR_TRANSLATOR_SRC
    ${PARACL_FRONTEND_IR_TRANSLATOR_SRC_DIR}/llvm-ir-translator.cppm
)

target_sources(${LLVM_IR_TRANSLATOR_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${LLVM_IR_TRANSLATOR_SRC}
)

target_link_libraries(${LLVM_IR_TRANSLATOR_LIB}
PRIVATE
    TheLast::TheLast
    ${PARACL_FRONTEND_IR_TRANSLATOR_NAMETABLE_LIB}
    ${LIBC_STANDART_FUNCTIONS_LIB}
    ${LLVM_IR_TRANSLATOR_CONTEXT_LIB}
    ${LLVM_IR_TRANSLATOR_NODES_FUNCTIONAL_LIB}
    paracl-frontend-errors
    paracl-options
)

add_llvm_to_target(${LLVM_IR_TRANSLATOR_LIB})

# =================================================================================================
