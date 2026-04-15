cmake_minimum_required(VERSION 3.10)

project(ParaCL-tests-e2e)

# =================================================================================================

# function for adding tests (not unit) to some executable files
function(target_e2e_test test_target run_script test_dir ans_dir)

    file(GLOB test_files "${test_dir}/*.cl") # save all tests -files in variable 'test_files'

    # get quantity of tests
    list(LENGTH test_files num_tests)

    # check that at least 1 test was found
    if(${num_tests} EQUAL 0)
        message(WARNING "No dat files found in ${test_dir}")
        return()
    endif()

    set(test_it 0)

    # add tests
    while (test_it LESS num_tests)
        math(EXPR test_it "${test_it} + 1")

        set(TEST_COMMAND ${run_script} ${test_it})

        # add test
        add_test(
            NAME paracl::compiler::test::${test_it}
            # execute test
            COMMAND ${TEST_COMMAND}
            # working directory - dircetory with executable file
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        )
    endwhile()

    # add deps for test_target 
    target_include_directories(${test_target}
    PRIVATE
        ${test_dir}
    )
endfunction(target_e2e_test)

# =================================================================================================
# set it here, cause this variabes need fot python script configure

set(E2E_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(PARACL_E2E_DAT_DIR ${E2E_SRC_DIR}/dat)
set(PARACL_E2E_ANS_DIR ${E2E_SRC_DIR}/ans)

# =================================================================================================

# check thah pyhon3 exist.
# python need for e2e tests
find_program(PYTHON_EXECUTABLE 
    NAMES python3 python
    DOC "Python executable"
)

if (NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python3 NOT find. No test available.")
endif()

# =================================================================================================

# create tun_test script for e2e paracl test
set(RUN_TEST_SCRIPT_IN             ${E2E_SRC_DIR}/run_test.sh.in)

set(PYTHON_RUN_TEST_SCRIPT         ${E2E_SRC_DIR}/parse_result.py)

set(E2E_OUTPUT_SCRIPT             ${CMAKE_BINARY_DIR}/rt)

# give execute permission to python script
file(CHMOD ${PYTHON_RUN_TEST_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================

# creating run_test from run_test.in
configure_file(
    ${RUN_TEST_SCRIPT_IN}
    ${E2E_OUTPUT_SCRIPT}
    @ONLY
)

# give execute permission to run_test
file(CHMOD ${E2E_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
)

# =================================================================================================

target_e2e_test(paracl
    ${E2E_OUTPUT_SCRIPT}
    ${PARACL_E2E_DAT_DIR}
    ${PARACL_E2E_ANS_DIR}
)

# =================================================================================================
