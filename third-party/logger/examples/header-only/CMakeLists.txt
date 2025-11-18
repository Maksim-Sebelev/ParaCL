cmake_minimum_required(VERSION 3.14)
project(PineapLog_examples
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOGGER_SRC_DIR ${PROJECT_SOURCE_DIR}/../../Src)

# ====================================================================
# create header-only example
add_executable(pineaplog_ho_example)

target_sources(pineaplog_ho_example
    PRIVATE
        ${PROJECT_SOURCE_DIR}/test.cpp
)

target_include_directories(pineaplog_ho_example
    PRIVATE
        ${LOGGER_SRC_DIR}/header-only/
)