cmake_minimum_required(VERSION 3.30)

project(ParaCL
    LANGUAGES CXX
    VERSION 6.0
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

# ===========================================================================

set(PARACL_VERSION ${PROJECT_VERSION})


set(PARACL_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# ===========================================================================

add_subdirectory(${PARACL_SRC_DIR}/options)
add_subdirectory(${PARACL_SRC_DIR}/frontend)
add_subdirectory(${PARACL_SRC_DIR}/backend)

# ===========================================================================

set(PARACL paracl)
add_executable(${PARACL})

target_sources(${PARACL}
PRIVATE
    ${PARACL_SRC_DIR}/paracl.cpp   
)

target_link_libraries(${PARACL}
PRIVATE
    paracl-options
    paracl-options-reader
    paracl-frontend
    paracl-backend
)

# ===========================================================================
# add test generator

set(PARACL_SNIPPY_SRC_DIR ${PROJECT_SOURCE_DIR}/ParaCL-snippy)
set(PARACL_SNIPPY_BINARY_DIR ${PROJECT_BINARY_DIR}/ParaCL-snippy)

include(ExternalProject)

ExternalProject_Add(
    ParaCL-snippy
    GIT_REPOSITORY https://github.com/Maksim-Sebelev/ParaCL-snippy.git
    GIT_TAG main
    SOURCE_DIR ${PARACL_SNIPPY_SRC_DIR}
    BINARY_DIR ${PARACL_SNIPPY_BINARY_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
    INSTALL_COMMAND ""
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS=${CMAKE_EXPORT_COMPILE_COMMANDS}
    BUILD_ALWAYS OFF
)

# ===========================================================================

# TESTS

enable_testing()
include(CTest)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
