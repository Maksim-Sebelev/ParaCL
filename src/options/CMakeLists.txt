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

find_package(Boost REQUIRED COMPONENTS program_options)

# =================================================================================================

set(PARACL_OPTIONS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(PARACL_OPTIONS_LIB paracl-options)
add_library(${PARACL_OPTIONS_LIB})

target_sources(${PARACL_OPTIONS_LIB}
PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  FILES
    ${PARACL_OPTIONS_SRC_DIR}/options.cppm
)

# =================================================================================================

set(PARACL_OPTIONS_READER_LIB paracl-options-reader)
add_library(${PARACL_OPTIONS_READER_LIB})

target_sources(${PARACL_OPTIONS_READER_LIB}
PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  FILES
    ${PARACL_OPTIONS_SRC_DIR}/read-options.cppm
)

target_link_libraries(${PARACL_OPTIONS_READER_LIB}
PUBLIC
  ${PARACL_OPTIONS_LIB}
PRIVATE
  Boost::program_options
)

target_compile_definitions(${PARACL_OPTIONS_READER_LIB}
PRIVATE
    -DPARACL_VERSION="${PARACL_VERSION}"
)

# =================================================================================================
