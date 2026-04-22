cmake_minimum_required(VERSION 3.28)

project(ParaCL-frontend-grammar
    LANGUAGES CXX
    VERSION 2.0
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

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# =================================================================================================
# check bison

find_program(BISON_EXECUTABLE 
    NAMES bison bison.exe
)

if (NOT BISON_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_force_declare (
        bison
        URL https://github.com/akimd/bison/archive/refs/tags/v3.8.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(bison)
    find_package(bison REQUIRED)

else ()
    message(STATUS "Using system bison: ${BISON_EXECUTABLE}")
endif()

# =================================================================================================
# check flex

find_program(FLEX_EXECUTABLE
    NAMES flex flex.exe
    DOC "flex executable"
)

if (NOT FLEX_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_force_declare(
        flex
        URL https://github.com/westes/flex/archive/refs/tags/v2.6.4.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(flex)
    find_package(flex REQUIRED)
else ()
    message(STATUS "Using system flex: ${FLEX_EXECUTABLE}")
endif()

# ====================== BISON (Parser) ======================
set(PARSER_SRC_DIR          ${PROJECT_SOURCE_DIR}/src/parser)
set(BISON_PARSER_CPP_OUT    parser.tab.cpp)
set(BISON_PARSER_HPP_OUT    parser.tab.hpp)
set(BISON_INPUT_FILE        ${PARSER_SRC_DIR}/parser.y)
set(BISON_OUTPUT_DIR        ${CMAKE_BINARY_DIR}/generated/bison)

add_custom_command(
    OUTPUT
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_HPP_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BISON_OUTPUT_DIR}
    COMMAND ${BISON_EXECUTABLE}
        -d
        -o ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        -Wcounterexamples # for debug grammar confllicts
        -v
        ${BISON_INPUT_FILE}
    DEPENDS ${BISON_INPUT_FILE}
    COMMENT "Generating Bison parser (${BISON_PARSER_CPP_OUT})"
    VERBATIM
)

add_custom_target(generate_parser
    DEPENDS
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_HPP_OUT}
)


# =================================================================================================

set(ERRORS_PARSING_LIB paracl-frontend-grammar-errors)
add_library(${ERRORS_PARSING_LIB})

add_dependencies(${ERRORS_PARSING_LIB}
    generate_parser
)

target_sources(${ERRORS_PARSING_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${SRC_DIR}/errors/errors.cppm
)

target_link_libraries(${ERRORS_PARSING_LIB}
PRIVATE
    TheLast::TheLast
    paracl-frontend-errors
)

target_include_directories(${ERRORS_PARSING_LIB}
PRIVATE
    ${BISON_OUTPUT_DIR}
)

# =================================================================================================

add_library(parser STATIC)

target_sources(parser
PUBLIC
    ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
)

target_include_directories(parser
PUBLIC
    ${BISON_OUTPUT_DIR} # parser.tab.hpp
    ${PARSER_SRC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/inc
)

target_link_libraries(parser
PUBLIC
    TheLast::TheLast
PRIVATE
    paracl-frontend-llvmirtranslator
    ${ERRORS_PARSING_LIB}
)

# ====================== FLEX (Lexer) ======================
set(LEXER_SRC_DIR          ${PROJECT_SOURCE_DIR}/src/lexer)
set(FLEX_LEXER_CPP_OUT     lexer.yy.cpp)
set(FLEX_INPUT_FILE        ${LEXER_SRC_DIR}/lexer.l)
set(FLEX_OUTPUT_DIR        ${CMAKE_BINARY_DIR}/generated/flex)

add_custom_command(
OUTPUT
    ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
COMMAND ${CMAKE_COMMAND} -E make_directory ${FLEX_OUTPUT_DIR}
COMMAND ${FLEX_EXECUTABLE}
    -o ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
    ${FLEX_INPUT_FILE}
DEPENDS
    ${FLEX_INPUT_FILE}
    ${BISON_OUTPUT_DIR}/${BISON_PARSER_HPP_OUT}
COMMENT "Generating Flex lexer (${FLEX_LEXER_CPP_OUT})"
VERBATIM
)

add_library(lexer STATIC)

target_sources(lexer
PRIVATE
    ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
)

target_include_directories(lexer
PUBLIC
    ${FLEX_OUTPUT_DIR}   # lexer.yy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/inc
    ${BISON_OUTPUT_DIR}  # parser.tab.hpp
    ${PARSER_SRC_DIR}
)

target_link_libraries(lexer
PUBLIC
    parser
    paracl-frontend-errors
PRIVATE
    ${ERRORS_PARSING_LIB}
)

# ================= AST Reader ======================

set(AST_READER_LIB paracl-frontend-ast-reader)
add_library(${AST_READER_LIB})

set(AST_READER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(AST_READER_SRCS
    ${AST_READER_SRC_DIR}/ast-reader.cppm
)

target_sources(${AST_READER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${AST_READER_SRCS}
)

target_link_libraries(${AST_READER_LIB}
PRIVATE
    lexer
    parser
PUBLIC
    paracl-options
    paracl-frontend-errors
    TheLast::TheLast
)

# =====================================================
