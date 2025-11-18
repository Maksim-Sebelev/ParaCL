cmake_minimum_required(VERSION 3.28)

project(PineapLog
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================================================================

# user want to use only Release-build-type without ExternalProject_add
if (NOT FORCE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ======================================================================

include(${PROJECT_SOURCE_DIR}/cmake/debug.cmake)

# ======================================================================

if (NOT DEFINED LOG_OUT_DIR)
    message("Warning: no log-out dir given.")
    message("Is will be made with one level with binary dir of logger")
    set(LOG_OUT_DIR ${CMAKE_BINARY_DIR}/../log)
endif()

set(LOGGER_LIB pineaplog_mb) # _mb = modules-based

add_library(${LOGGER_LIB})

target_sources(${LOGGER_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${PROJECT_SOURCE_DIR}/src/log.cppm
)

target_include_directories(${LOGGER_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/logger/
        ${PROJECT_SOURCE_DIR}/include/global/
)

target_compile_definitions(${LOGGER_LIB} PRIVATE
    LOG_DIR="${LOG_OUT_DIR}/"
    $<$<CONFIG:Debug>:SAVE_HTML_STYLE>
    # GLOBAL_LOGGER_IMAGE="/path/to/image"
    # GLOBAL_LOGGER_GRADIENT
    # GLOBAL_LOGGER_DEFAULT
)

target_hard_debug_options(${LOGGER_LIB})

# ======================================================================
# remove all log files target

add_custom_target(clean-pineaplog
    COMMAND ${CMAKE_COMMAND} -E clean_directory ${LOG_OUT_DIR}/
)

# ======================================================================
