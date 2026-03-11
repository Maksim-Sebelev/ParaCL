cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(ParaCL-Test-Generator
    LANGUAGES CXX
    VERSION 1.0
)

# =================================================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================================================================================================

set(AUTHORS 
    "Max Sebelev|https://github.com/Maksim-Sebelev"
    "Matthew Galicin|https://github.com/Matvey787"
)

# =================================================================================================

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../AST
    ${CMAKE_BINARY_DIR}/subproject/ast
)

# =================================================================================================

set(TEST_GENERATOR_SRC_DIR ${PROJECT_SOURCE_DIR}/src)


# =================================================================================================
# ast serializer library


set(TEST_GENERATOR_AST_SERIALIZER_LIB ast-serializer)
add_library(${TEST_GENERATOR_AST_SERIALIZER_LIB})

set(TEST_GENERATOR_AST_SERIALIZER_SRC_DIR ${TEST_GENERATOR_SRC_DIR}/ast-serializer)
set(TEST_GENERATOR_AST_SERIALIZER_SRCS
    ${TEST_GENERATOR_AST_SERIALIZER_SRC_DIR}/ast-serializer.cppm
)

target_sources(${TEST_GENERATOR_AST_SERIALIZER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${TEST_GENERATOR_AST_SERIALIZER_SRCS}
)

target_link_libraries(${TEST_GENERATOR_AST_SERIALIZER_LIB}
PRIVATE
    TheLast::TheLast
)


# =================================================================================================
# nametable library

set(TEST_GENERATOR_NAMETABLE_LIB test-generator-nametable)
add_library(${TEST_GENERATOR_NAMETABLE_LIB})

set(TEST_GENERATOR_NAMETABLE_SRC_DIR ${TEST_GENERATOR_SRC_DIR}/generator/nametable)
set(TEST_GENERATOR_NAMETABLE_SRCS
    ${TEST_GENERATOR_NAMETABLE_SRC_DIR}/nametable.cppm
)

target_sources(${TEST_GENERATOR_NAMETABLE_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${TEST_GENERATOR_NAMETABLE_SRCS}
)

# =================================================================================================
# generator library

set(TEST_GENERATOR_LIB test-generator)
add_library(${TEST_GENERATOR_LIB})

set(TEST_GENERATOR_LIB_SRC_DIR ${TEST_GENERATOR_SRC_DIR}/generator)
set(TEST_GENERATOR_LIB_SRCS
    ${TEST_GENERATOR_LIB_SRC_DIR}/generator.cppm
)

target_sources(${TEST_GENERATOR_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${TEST_GENERATOR_LIB_SRCS}
)

target_link_libraries(${TEST_GENERATOR_LIB}
PUBLIC
    TheLast::TheLast
PRIVATE
    ${TEST_GENERATOR_NAMETABLE_LIB}
    ${TEST_GENERATOR_AST_SERIALIZER_LIB}
)

# =================================================================================================

set(TEST_GENERATOR paracl-test-generator)
add_executable(${TEST_GENERATOR}
    ${TEST_GENERATOR_SRC_DIR}/main.cpp
)

target_link_libraries(${TEST_GENERATOR}
PRIVATE
    ${TEST_GENERATOR_LIB}
)

# =================================================================================================
