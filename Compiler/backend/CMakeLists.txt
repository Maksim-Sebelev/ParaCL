cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(ParaCL-Compiler
    VERSION 1.0
    LANGUAGES C CXX
)

# =================================================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================================================================================================

# in default we build project in release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTING "Build tests" OFF)
option(LOGGER "logger" OFF)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================

set(COMPILE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(COMPILE_THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third-party)

# =================================================================================================
# add ast library

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../AST
    ${CMAKE_BINARY_DIR}/subprojects/ast
)

# =================================================================================================
# find LLVM

find_package(LLVM REQUIRED CONFIG)
# find_package(LLVM REQUIRED CONFIG 17.0.6)

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
        ${LLVM_LIBRARIES}
    )
endfunction(add_llvm_to_target )


# =================================================================================================
# compiler options


set(COMPILER_OPTIONS_LIB options)
add_library(${COMPILER_OPTIONS_LIB} EXCLUDE_FROM_ALL)

set(COMPILER_OPTIONS_SRC_DIR ${COMPILE_SRC_DIR}/options)
set(COMPILER_OPTIONS_SRC
    ${COMPILER_OPTIONS_SRC_DIR}/options.cppm
)

target_sources(${COMPILER_OPTIONS_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
      ${COMPILER_OPTIONS_SRC}
)

add_llvm_to_target(${COMPILER_OPTIONS_LIB})

# =================================================================================================
# compiler exceptions

set(COMPILER_EXCEPTIONS_LIB exceptions)
add_library(${COMPILER_EXCEPTIONS_LIB} EXCLUDE_FROM_ALL)

set(COMPILER_EXCEPTIONS_SRC_DIR ${COMPILE_SRC_DIR}/exceptions)
set(COMPILER_EXCEPTIONS_SRC
    ${COMPILER_EXCEPTIONS_SRC_DIR}/exceptions.cppm
)

target_sources(${COMPILER_EXCEPTIONS_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
      ${COMPILER_EXCEPTIONS_SRC}
)

target_link_libraries(${COMPILER_EXCEPTIONS_LIB}
  PRIVATE
    TheLast::TheLast
)

add_llvm_to_target(${COMPILER_EXCEPTIONS_LIB})

# =================================================================================================
# nametable library for compiler

set(COMPILER_NAMETABLE_LIB nametable)
add_library(${COMPILER_NAMETABLE_LIB})

set(COMPILER_NAMETABLE_SRC_DIR ${COMPILE_SRC_DIR}/llvm-ir-translator)
set(COMPILER_NAMETABLE_SRC
    ${COMPILER_NAMETABLE_SRC_DIR}/nametable.cppm
    ${COMPILER_NAMETABLE_SRC_DIR}/functions-table.cppm
)

target_sources(${COMPILER_NAMETABLE_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${COMPILER_NAMETABLE_SRC}
)

target_link_libraries(${COMPILER_NAMETABLE_LIB}
    PUBLIC
        TheLast::TheLast
)

add_llvm_to_target(${COMPILER_NAMETABLE_LIB})

# =================================================================================================
# libc standard functions library

set(LIBC_STANDART_FUNCTIONS_LIB libc-standard-functions)
add_library(${LIBC_STANDART_FUNCTIONS_LIB})

set(LIBC_STANDART_FUNCTIONS_SRC_DIR ${COMPILE_SRC_DIR}/llvm-ir-translator)
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
# llvm ir translator library

set(LLVM_IR_TRANSLATOR_LIB llvm-ir-translator)
add_library(${LLVM_IR_TRANSLATOR_LIB})

set(LLVM_IR_TRANSLATOR_SRC_DIR ${COMPILE_SRC_DIR}/llvm-ir-translator)
set(LLVM_IR_TRANSLATOR_SRC
    ${LLVM_IR_TRANSLATOR_SRC_DIR}/llvm-ir-translator.cppm
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
    ${COMPILER_NAMETABLE_LIB}
    ${LIBC_STANDART_FUNCTIONS_LIB}
    ${LLVM_LIBRARIES}
    ${COMPILER_EXCEPTIONS_LIB}
)


target_include_directories(${LLVM_IR_TRANSLATOR_LIB}
    PUBLIC
        ${INC_DIR}
)

add_llvm_to_target(${LLVM_IR_TRANSLATOR_LIB})

# =================================================================================================
# compiler library (main compiler logic)

set(COMPILER_LIB compiler)
add_library(${COMPILER_LIB})

set(COMPILER_SRC_DIR ${COMPILE_SRC_DIR}/compiler)
set(COMPILER_SRC
    ${COMPILER_SRC_DIR}/compiler.cppm
)

target_sources(${COMPILER_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${COMPILER_SRC}
)

target_link_libraries(${COMPILER_LIB}
  PRIVATE
    ${LLVM_IR_TRANSLATOR_LIB}
    # ${COMPILER_OPTIONS_LIB} # unsupported yet
)

target_include_directories(${COMPILER_LIB}
  PUBLIC
    ${INC_DIR}
)

# =================================================================================================
# main executable

add_executable(paracl-compiler
    ${COMPILE_SRC_DIR}/main.cpp
)

target_link_libraries(paracl-compiler
PRIVATE
    ${EXECUTABLE_BUILDER_LIB}
    ${COMPILER_LIB}
    ${LLVM_IR_TRANSLATOR_LIB}
    ${COMPILER_EXCEPTIONS_LIB}
)

target_include_directories(paracl-compiler
PRIVATE
    ${INC_DIR}
)

# =================================================================================================
# tests

if(BUILD_TESTING)
    enable_testing()
    
    add_executable(compiler-tests
        ${PROJECT_SOURCE_DIR}/tests/test.cpp
    )
    
    target_link_libraries(compiler-tests
        PRIVATE
            ${EXECUTABLE_BUILDER_LIB}
            ${COMPILER_LIB}
            ${LLVM_IR_TRANSLATOR_LIB}
    )
    
    target_include_directories(compiler-tests
        PRIVATE
            ${INC_DIR}
    )
    
    add_test(NAME compiler-tests COMMAND compiler-tests)
endif()

# =================================================================================================
