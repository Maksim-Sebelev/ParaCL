cmake_minimum_required(VERSION 3.28)

project(ParaCL-frontend
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

set(PARACL_FRONTEND_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# =================================================================================================

add_subdirectory(${PARACL_FRONTEND_SRC_DIR}/AST)
add_subdirectory(${PARACL_FRONTEND_SRC_DIR}/errors)
add_subdirectory(${PARACL_FRONTEND_SRC_DIR}/grammar)
add_subdirectory(${PARACL_FRONTEND_SRC_DIR}/llvm-ir-translator)

# =================================================================================================

set(PARACL_FRONTEND_LIB paracl-frontend)
add_library(${PARACL_FRONTEND_LIB})

target_sources(${PARACL_FRONTEND_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_FRONTEND_SRC_DIR}/frontend.cppm
)

target_link_libraries(${PARACL_FRONTEND_LIB}
PUBLIC
    paracl-options
PRIVATE
    paracl-frontend-ast-reader
    paracl-frontend-llvmirtranslator
)

# =================================================================================================
