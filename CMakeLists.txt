cmake_minimum_required(VERSION 3.30)

project(ParaCL
    LANGUAGES CXX
    VERSION 6.1
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
# TESTS

enable_testing()
include(CTest)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
