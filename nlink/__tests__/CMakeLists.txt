# CMakeLists.txt for NexusLink tests
cmake_minimum_required(VERSION 3.13)

# Define test framework library
set(TEST_FRAMEWORK_SOURCES
    framework/test_stub_manager.c
)

add_library(nlink_test_framework STATIC ${TEST_FRAMEWORK_SOURCES})
target_include_directories(nlink_test_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(nlink_test_framework PRIVATE
    nlink_core_static
)

# Function to add a test directory with all its tests
function(add_test_subdirectory DIR)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/CMakeLists.txt")
        add_subdirectory(${DIR})
    else()
        # Auto-generate tests from .c files in directory
        file(GLOB_RECURSE TEST_SOURCES "${DIR}/*.c")
        
        foreach(TEST_SOURCE ${TEST_SOURCES})
            get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
            get_filename_component(TEST_PATH ${TEST_SOURCE} DIRECTORY)
            file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${TEST_PATH})
            
            # Create namespace from relative path
            string(REPLACE "/" "_" TEST_NAMESPACE ${REL_PATH})
            
            # Final test executable name
            set(TEST_EXEC_NAME "${TEST_NAMESPACE}_${TEST_NAME}")
            
            # Add test executable
            add_executable(${TEST_EXEC_NAME} ${TEST_SOURCE})
            target_link_libraries(${TEST_EXEC_NAME} PRIVATE
                nlink_test_framework
                nlink_core_static
            )
            target_include_directories(${TEST_EXEC_NAME} PRIVATE
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_SOURCE_DIR}/src
                ${CMAKE_CURRENT_SOURCE_DIR}/include
            )
            
            # Add as test
            add_test(NAME ${TEST_EXEC_NAME} COMMAND ${TEST_EXEC_NAME})
            
            # Set properties for test result formatting
            set_tests_properties(${TEST_EXEC_NAME} PROPERTIES
                FAIL_REGULAR_EXPRESSION ".*FAILED.*"
                PASS_REGULAR_EXPRESSION ".*PASSED.*"
            )
            
            # Create target to run this test individually
            add_custom_target(run_${TEST_EXEC_NAME}
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXEC_NAME}
                DEPENDS ${TEST_EXEC_NAME}
                COMMENT "Running test ${TEST_EXEC_NAME}"
                VERBATIM
            )
            
            # Add to test list for the directory
            list(APPEND ${DIR}_TESTS ${TEST_EXEC_NAME})
        endforeach()
        
        # Create target to run all tests in this directory
        add_custom_target(run_${DIR}_tests
            COMMENT "Running all tests in ${DIR}"
        )
        
        # Add dependencies for each test
        foreach(TEST ${${DIR}_TESTS})
            add_dependencies(run_${DIR}_tests ${TEST})
        endforeach()
    endif()
endfunction()

# Add test framework directory (needed only if there are specific framework tests)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/framework/CMakeLists.txt")
    add_subdirectory(framework)
endif()

# Add test directories
add_test_subdirectory(unit)
add_test_subdirectory(integration)

# Create target to run all tests
add_custom_target(run_all_tests
    DEPENDS run_unit_tests run_integration_tests
    COMMENT "Running all tests"
)

# Create targets for each test level
add_custom_target(run_unit_tests
    DEPENDS unit_tests
    COMMENT "Running unit tests"
)

add_custom_target(run_integration_tests
    DEPENDS integration_tests
    COMMENT "Running integration tests"
)

# Create test collection targets
add_custom_target(unit_tests)
add_custom_target(integration_tests)

# Add unit test subdirectories
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unit/core")
    file(GLOB UNIT_CORE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/unit/core/*")
    foreach(DIR ${UNIT_CORE_DIRS})
        if(IS_DIRECTORY ${DIR})
            get_filename_component(COMPONENT_NAME ${DIR} NAME)
            add_test_subdirectory(unit/core/${COMPONENT_NAME})
            add_dependencies(unit_tests run_unit_core_${COMPONENT_NAME}_tests)
        endif()
    endforeach()
endif()

# For each core component, create the test stub directories if they don't exist
get_property(CORE_COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
foreach(COMPONENT ${CORE_COMPONENTS})
    set(COMPONENT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unit/core/${COMPONENT}")
    if(NOT EXISTS "${COMPONENT_TEST_DIR}")
        file(MAKE_DIRECTORY "${COMPONENT_TEST_DIR}")
        # Generate a test CMakeLists.txt
        nlink_create_component_test_cmakelists(${COMPONENT})
        # Generate a skeleton test file
        nlink_create_skeleton_test_file(
            COMPONENT ${COMPONENT}
            NAME "core"
        )
    endif()
endforeach()

# Generate coverage report if enabled
if(CMAKE_COMPILER_IS_GNUCXX AND ENABLE_COVERAGE)
    # Add coverage target
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND lcov --directory ${CMAKE_BINARY_DIR} --zerocounters
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target run_all_tests
        COMMAND lcov --directory ${CMAKE_BINARY_DIR} --capture --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
        COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage/coverage.info "/usr/include/*" "/usr/lib/*" "*/tests/*" "*/build/*" --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
        COMMAND genhtml ${CMAKE_BINARY_DIR}/coverage/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage/html
        COMMENT "Generating coverage report"
    )
endif()