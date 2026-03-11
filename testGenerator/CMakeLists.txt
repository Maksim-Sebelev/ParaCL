cmake_minimum_required(VERSION 3.28)
project(ParaCL_Front VERSION 3.0.0)

set(AUTHORS 
    "Max Sebelev|https://github.com/Maksim-Sebelev"
    "Matthew Galicin|https://github.com/Matvey787"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../AST
    ${CMAKE_BINARY_DIR}/subproject/ast
)

set(TEST_GENERATOR generator)
add_executable(${TEST_GENERATOR} main.cpp)

target_link_libraries(${TEST_GENERATOR}
    PRIVATE
        TheLast::TheLast
)


