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

set(PARACL_BACKEND_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(PARACL_BACKEND_LIB paracl-backend)
add_library(${PARACL_BACKEND_LIB})

target_sources(${PARACL_BACKEND_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_BACKEND_SRC_DIR}/backend.cppm
)

target_link_libraries(${PARACL_BACKEND_LIB}
PUBLIC
    paracl-options
)

# =================================================================================================
