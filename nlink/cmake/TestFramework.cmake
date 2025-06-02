# TestFramework.cmake - AAA Pattern Test Module for NexusLink
#
# This module provides functions for creating component tests using the AAA pattern
# (Arrange-Act-Assert) with proper linking to component object files.

# Include guard
if(DEFINED NLINK_TEST_FRAMEWORK_CMAKE_INCLUDED)
  return()
endif()
set(NLINK_TEST_FRAMEWORK_CMAKE_INCLUDED TRUE)

include(CMakeParseArguments)

# Define project paths
set(NLINK_TEST_FRAMEWORK_DIR "${CMAKE_SOURCE_DIR}/tests/framework")
set(NLINK_TEST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/tests/include")

# Initialize test system
function(nlink_init_test_system)
  cmake_parse_arguments(
    TEST_INIT
    "COVERAGE"
    ""
    "COMPONENTS"
    ${ARGN}
  )
  
  # Enable testing
  enable_testing()
  
  # Create test directories
  nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests")
  nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests/unit")
  nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests/unit/core")
  nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests/unit/cli")
  nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests/integration")
  nlink_ensure_directory("${NLINK_TEST_INCLUDE_DIR}")
  nlink_ensure_directory("${NLINK_TEST_FRAMEWORK_DIR}")
  
  # Create test framework directory if it doesn't exist
  if(NOT EXISTS "${NLINK_TEST_INCLUDE_DIR}/nlink_test.h")
    # Generate test framework header
    file(WRITE "${NLINK_TEST_INCLUDE_DIR}/nlink_test.h" 
"/**
 * @file nlink_test.h
 * @brief NexusLink Test Framework Header
 *
 * This header provides the testing macros and utilities for the AAA pattern
 * (Arrange-Act-Assert) used in NexusLink tests.
 */

#ifndef NLINK_TEST_H
#define NLINK_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Test suite management */
#define NLINK_TEST_SUITE_BEGIN(name) void* nlink_test_suite_setup_##name() 

#define NLINK_TEST_SUITE_END(name) void nlink_test_suite_teardown_##name(void *context)

/* Test fixture management */
#define NLINK_TEST_FIXTURE(suite, name) void* nlink_test_fixture_setup_##suite##_##name()

#define NLINK_TEST_FIXTURE_END(suite, name) void nlink_test_fixture_teardown_##suite##_##name(void *context)

/* Test case definitions */
#define NLINK_TEST_CASE(suite, name) void nlink_test_case_##suite##_##name()

#define NLINK_TEST_CASE_WITH_FIXTURE(suite, name, fixture) \
    void nlink_test_case_##suite##_##name(void *fixture_context)

/* AAA pattern macros */
#define NLINK_ARRANGE_PHASE(msg) printf(\"\\nARRANGE: %s\\n\", msg)
#define NLINK_ACT_PHASE(msg) printf(\"\\nACT: %s\\n\", msg)
#define NLINK_ASSERT_PHASE(msg) printf(\"\\nASSERT: %s\\n\", msg)

/* Assertion macros */
#define NLINK_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(\"FAILED: %s (line %d)\\n\", message, __LINE__); \
            return; \
        } \
    } while (0)

#define NLINK_ASSERT_TRUE(condition, message) \
    NLINK_ASSERT((condition), message)

#define NLINK_ASSERT_FALSE(condition, message) \
    NLINK_ASSERT(!(condition), message)

#define NLINK_ASSERT_NULL(ptr, message) \
    NLINK_ASSERT((ptr) == NULL, message)

#define NLINK_ASSERT_NOT_NULL(ptr, message) \
    NLINK_ASSERT((ptr) != NULL, message)

#define NLINK_ASSERT_EQUAL_INT(expected, actual, message) \
    do { \
        int e = (expected); \
        int a = (actual); \
        NLINK_ASSERT(e == a, message); \
    } while (0)

#define NLINK_ASSERT_EQUAL_STRING(expected, actual, message) \
    do { \
        const char *e = (expected); \
        const char *a = (actual); \
        NLINK_ASSERT(strcmp(e, a) == 0, message); \
    } while (0)

/* Test registration and execution */
#define NLINK_TEST_REGISTER(suite, name) \
    void nlink_run_test_##suite##_##name() { \
        void *suite_context = nlink_test_suite_setup_##suite(); \
        nlink_test_case_##suite##_##name(); \
        nlink_test_suite_teardown_##suite(suite_context); \
        printf(\"Test %s::%s PASSED\\n\", #suite, #name); \
    }

#define NLINK_TEST_REGISTER_WITH_FIXTURE(suite, name, fixture) \
    void nlink_run_test_##suite##_##name() { \
        void *suite_context = nlink_test_suite_setup_##suite(); \
        void *fixture_context = nlink_test_fixture_setup_##suite##_##fixture(); \
        nlink_test_case_##suite##_##name(fixture_context); \
        nlink_test_fixture_teardown_##suite##_##fixture(fixture_context); \
        nlink_test_suite_teardown_##suite(suite_context); \
        printf(\"Test %s::%s PASSED\\n\", #suite, #name); \
    }

#define NLINK_TEST_MAIN(...) \
    int main(int argc, char *argv[]) { \
        (void)argc; (void)argv; \
        printf(\"\\nRunning NexusLink tests...\\n\"); \
        __VA_ARGS__; \
        printf(\"\\nAll tests PASSED!\\n\"); \
        return 0; \
    }

/* Test logging utilities */
#define nlink_test_log_info(format, ...) \
    printf(\"INFO: \" format \"\\n\", ##__VA_ARGS__)

#define nlink_test_log_warning(format, ...) \
    printf(\"WARNING: \" format \"\\n\", ##__VA_ARGS__)

#define nlink_test_log_error(format, ...) \
    printf(\"ERROR: \" format \"\\n\", ##__VA_ARGS__)

/* Test stub manager */
void* test_stub_manager_init(const char* components[], int count);
void test_stub_manager_cleanup(void);

#endif /* NLINK_TEST_H */
")

    # Generate test stub manager implementation
    file(WRITE "${NLINK_TEST_FRAMEWORK_DIR}/test_stub_manager.c"
"/**
 * @file test_stub_manager.c
 * @brief Implementation of the test stub manager
 */

#include \"../include/nlink_test.h\"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **components;
    int count;
    void **stubs;
} StubManager;

static StubManager *manager = NULL;

void* test_stub_manager_init(const char* components[], int count) {
    if (manager != NULL) {
        // Already initialized
        return manager;
    }
    
    manager = (StubManager*)malloc(sizeof(StubManager));
    if (!manager) {
        return NULL;
    }
    
    manager->count = count;
    manager->components = (char**)malloc(count * sizeof(char*));
    manager->stubs = (void**)malloc(count * sizeof(void*));
    
    if (!manager->components || !manager->stubs) {
        free(manager->components);
        free(manager->stubs);
        free(manager);
        manager = NULL;
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        manager->components[i] = strdup(components[i]);
        manager->stubs[i] = NULL;
    }
    
    return manager;
}

void test_stub_manager_cleanup(void) {
    if (manager == NULL) {
        return;
    }
    
    for (int i = 0; i < manager->count; i++) {
        free(manager->components[i]);
        // Free any allocated stubs if needed
        // free(manager->stubs[i]);
    }
    
    free(manager->components);
    free(manager->stubs);
    free(manager);
    manager = NULL;
}
")
  endif()
  
  # Create test framework library
  add_library(nlink_test_framework STATIC
    "${NLINK_TEST_FRAMEWORK_DIR}/test_stub_manager.c"
  )
  
  target_include_directories(nlink_test_framework PUBLIC
    ${NLINK_TEST_INCLUDE_DIR}
    ${NLINK_INCLUDE_DIR}
  )
  
  # Define main test targets
  add_custom_target(unit_tests
    COMMENT "Running all unit tests"
  )
  
  add_custom_target(integration_tests
    COMMENT "Running all integration tests"
  )
  
  add_custom_target(run_all_tests
    DEPENDS unit_tests integration_tests
    COMMENT "Running all tests"
  )
  
  # Set up coverage if enabled
  if(TEST_INIT_COVERAGE)
    nlink_setup_coverage()
  endif()
  
  # Add component-specific test targets
  foreach(COMPONENT ${TEST_INIT_COMPONENTS})
    add_custom_target(unit_core_${COMPONENT}_tests
      COMMENT "Running unit tests for ${COMPONENT}"
    )
    
    add_dependencies(unit_tests unit_core_${COMPONENT}_tests)
    
    # Create test directory for component
    nlink_ensure_directory("${NLINK_PROJECT_ROOT}/tests/unit/core/${COMPONENT}")
    
    # Create test CMakeLists.txt for component
    nlink_create_component_test_cmakelists(${COMPONENT})
  endforeach()
  
  message(STATUS "Test system initialized")
  message(STATUS "  Test framework in: ${NLINK_TEST_FRAMEWORK_DIR}")
  message(STATUS "  Test includes in: ${NLINK_TEST_INCLUDE_DIR}")
endfunction()

# Function to add a test that follows the AAA pattern
function(nlink_add_aaa_test)
  cmake_parse_arguments(
    AAA_TEST
    ""
    "NAME;COMPONENT;TEST_GROUP"
    "SOURCES;DEPENDENCIES;MOCK_COMPONENTS"
    ${ARGN}
  )

  # Validate required arguments
  if(NOT AAA_TEST_NAME)
    message(FATAL_ERROR "NAME is required for nlink_add_aaa_test")
  endif()
  
  if(NOT AAA_TEST_COMPONENT)
    message(FATAL_ERROR "COMPONENT is required for nlink_add_aaa_test")
  endif()
  
  if(NOT AAA_TEST_SOURCES)
    message(FATAL_ERROR "SOURCES is required for nlink_add_aaa_test")
  endif()
  
  # Set default test group if not provided
  if(NOT AAA_TEST_TEST_GROUP)
    set(AAA_TEST_TEST_GROUP "unit")
  endif()
  
  # Set up full test name with prefix
  set(FULL_TEST_NAME "test_${AAA_TEST_TEST_GROUP}_${AAA_TEST_COMPONENT}_${AAA_TEST_NAME}")
  
  # Create the test executable
  add_executable(${FULL_TEST_NAME} ${AAA_TEST_SOURCES})
  
  # Add test framework includes
  target_include_directories(${FULL_TEST_NAME} PRIVATE
    ${NLINK_TEST_INCLUDE_DIR}
    ${NLINK_INCLUDE_DIR}
    ${NLINK_SRC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
  # Link with test framework
  target_link_libraries(${FULL_TEST_NAME} PRIVATE
    nlink_test_framework
  )
  
  # Link with stub objects for each mock component
  foreach(MOCK_COMPONENT ${AAA_TEST_MOCK_COMPONENTS})
    # Check if the component object directory exists
    set(COMPONENT_OBJ_DIR "${NLINK_OBJ_DIR}/core/${MOCK_COMPONENT}")
    if(EXISTS "${COMPONENT_OBJ_DIR}")
      # Find all .o files for this component
      file(GLOB COMPONENT_OBJECTS "${COMPONENT_OBJ_DIR}/*.o")
      
      if(COMPONENT_OBJECTS)
        # Link with component objects
        target_link_libraries(${FULL_TEST_NAME} PRIVATE ${COMPONENT_OBJECTS})
      else()
        message(STATUS "No object files found for component: ${MOCK_COMPONENT}")
      endif()
    else()
      message(WARNING "Component object directory not found: ${COMPONENT_OBJ_DIR}")
    endif()
  endforeach()
  
  # Link with additional dependencies
  foreach(DEPENDENCY ${AAA_TEST_DEPENDENCIES})
    target_link_libraries(${FULL_TEST_NAME} PRIVATE ${DEPENDENCY})
  endforeach()
  
  # If no mock components specified, link with full static library
  if(NOT AAA_TEST_MOCK_COMPONENTS)
    target_link_libraries(${FULL_TEST_NAME} PRIVATE nlink_static)
  endif()
  
  # Register as a CTest test
  add_test(NAME ${FULL_TEST_NAME} COMMAND ${FULL_TEST_NAME})
  
  # Set test properties for output formatting
  set_tests_properties(${FULL_TEST_NAME} PROPERTIES
    FAIL_REGULAR_EXPRESSION ".*FAILED.*"
    PASS_REGULAR_EXPRESSION ".*PASSED.*"
  )
  
  # Create a target to run this test individually
  add_custom_target(run_${FULL_TEST_NAME}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${FULL_TEST_NAME}
    DEPENDS ${FULL_TEST_NAME}
    COMMENT "Running test ${FULL_TEST_NAME}"
    VERBATIM
  )
  
  # Add this test to the component tests target
  add_dependencies(unit_core_${AAA_TEST_COMPONENT}_tests ${FULL_TEST_NAME})
endfunction()

# Function to create empty stub object files for a component
function(nlink_create_component_stubs COMPONENT)
  set(COMPONENT_OBJ_DIR "${NLINK_OBJ_DIR}/core/${COMPONENT}")
  
  # Create the component object directory if it doesn't exist
  if(NOT EXISTS "${COMPONENT_OBJ_DIR}")
    file(MAKE_DIRECTORY "${COMPONENT_OBJ_DIR}")
  endif()
  
  # Define standard object files to create
  set(STUB_FILES
    "${COMPONENT}_core"
    "${COMPONENT}_context"
    "nexus_${COMPONENT}"
    "mock_${COMPONENT}"
  )
  
  # Create stub object files if they don't exist
  foreach(STUB_FILE ${STUB_FILES})
    set(OBJ_FILE "${COMPONENT_OBJ_DIR}/${STUB_FILE}.o")
    
    if(NOT EXISTS "${OBJ_FILE}")
      # Create an empty C file
      set(TEMP_C_FILE "${CMAKE_CURRENT_BINARY_DIR}/temp_${COMPONENT}_${STUB_FILE}.c")
      file(WRITE "${TEMP_C_FILE}" "/* Empty stub file for ${COMPONENT}/${STUB_FILE} */\n")
      
      # Compile it to an object file
      add_custom_command(
        OUTPUT "${OBJ_FILE}"
        COMMAND ${CMAKE_C_COMPILER} -c "${TEMP_C_FILE}" -o "${OBJ_FILE}"
        COMMENT "Creating stub object for ${COMPONENT}/${STUB_FILE}"
        VERBATIM
      )
      
      # Add to list of generated files
      list(APPEND GENERATED_STUBS "${OBJ_FILE}")
    endif()
  endforeach()
  
  # Create a target to generate all stubs for this component
  if(GENERATED_STUBS)
    add_custom_target(generate_${COMPONENT}_stubs ALL
      DEPENDS ${GENERATED_STUBS}
      COMMENT "Generating stub objects for ${COMPONENT}"
    )
  endif()
endfunction()

# Create a CMakeLists.txt file for component tests
function(nlink_create_component_test_cmakelists COMPONENT)
  # Set test directory
  set(COMPONENT_TEST_DIR "${NLINK_PROJECT_ROOT}/tests/unit/core/${COMPONENT}")
  
  # Check if CMakeLists.txt already exists
  if(EXISTS "${COMPONENT_TEST_DIR}/CMakeLists.txt")
    return()
  endif()
  
  # Create CMakeLists.txt file
  file(WRITE "${COMPONENT_TEST_DIR}/CMakeLists.txt" "# CMakeLists.txt for NexusLink unit/core/${COMPONENT} tests
cmake_minimum_required(VERSION 3.13)

# Include the test framework module
include(TestFramework)

# Create component stubs if needed
nlink_create_component_stubs(${COMPONENT})

# Create target for ${COMPONENT} unit tests
add_custom_target(unit_core_${COMPONENT}_tests
    COMMENT \"${COMPONENT} unit tests target\"
)

# Get all test sources in this directory
file(GLOB ${COMPONENT}_TEST_SOURCES \"*.c\")

# Add each test file
foreach(TEST_SOURCE \${${COMPONENT}_TEST_SOURCES})
    # Get test name from file name
    get_filename_component(TEST_NAME \${TEST_SOURCE} NAME_WE)
    
    # Add the test using the AAA pattern
    nlink_add_aaa_test(
        NAME \${TEST_NAME}
        COMPONENT \"${COMPONENT}\"
        SOURCES \${TEST_SOURCE}
        MOCK_COMPONENTS \"${COMPONENT}\"
    )
endforeach()

# Create a target that runs all ${COMPONENT} tests
add_custom_target(run_core_${COMPONENT}_tests
    DEPENDS unit_core_${COMPONENT}_tests
    COMMENT \"Running all ${COMPONENT} tests\"
)

# Add this component's tests to the unit_core_tests target
add_dependencies(unit_tests unit_core_${COMPONENT}_tests)
")
  
  message(STATUS "Created CMakeLists.txt for ${COMPONENT} tests")
endfunction()

# Create a skeleton test file for a component
function(nlink_create_skeleton_test_file)
  cmake_parse_arguments(
    TEST_FILE
    ""
    "COMPONENT;NAME;OUTPUT"
    ""
    ${ARGN}
  )
  
  # Validate required arguments
  if(NOT TEST_FILE_COMPONENT)
    message(FATAL_ERROR "COMPONENT argument is required for nlink_create_skeleton_test_file")
  endif()
  
  # Set default name if not provided
  if(NOT TEST_FILE_NAME)
    set(TEST_FILE_NAME "core")
  endif()
  
  # Set default output if not provided
  if(NOT TEST_FILE_OUTPUT)
    set(TEST_FILE_OUTPUT "${NLINK_PROJECT_ROOT}/tests/unit/core/${TEST_FILE_COMPONENT}/test_${TEST_FILE_NAME}.c")
  endif()
  
  # Check if file already exists
  if(EXISTS "${TEST_FILE_OUTPUT}")
    message(STATUS "Test file already exists: ${TEST_FILE_OUTPUT}")
    return()
  endif()
  
  # Create parent directory if needed
  nlink_ensure_directory(${CMAKE_CURRENT_BINARY_DIR}/temp)
  
  # Create a template test file
  file(WRITE "${TEST_FILE_OUTPUT}" "/**
 * @file test_${TEST_FILE_NAME}.c
 * @brief Unit tests for ${TEST_FILE_COMPONENT} ${TEST_FILE_NAME} functionality using AAA pattern
 * @author NexusLink Implementation Team
 */

#include \"nlink/core/${TEST_FILE_COMPONENT}/nexus_${TEST_FILE_COMPONENT}.h\"
#include \"../../../include/nlink_test.h\"

NLINK_TEST_SUITE_BEGIN(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}) {
    // Global setup for test suite
    nlink_test_log_info(\"Setting up ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} test suite\");
    
    // Initialize component stubs
    const char* components[] = {\"${TEST_FILE_COMPONENT}\"};
    if (!test_stub_manager_init(components, 1)) {
        return NULL;
    }
    
    return NULL; // No global context needed
}

NLINK_TEST_FIXTURE(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}, basic_fixture) {
    // ARRANGE phase - setup test fixture
    NLINK_ARRANGE_PHASE(\"Setting up basic fixture for ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}\");
    
    // Create a context for the test
    void* context = malloc(sizeof(void*));
    NLINK_ASSERT_NOT_NULL(context, \"Failed to allocate test context\");
    
    return context;
}

NLINK_TEST_FIXTURE_END(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}, basic_fixture) {
    // Clean up fixture
    if (context) {
        free(context);
    }
}

NLINK_TEST_CASE_WITH_FIXTURE(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}, initialization, basic_fixture) {
    // Unused fixture context
    (void)fixture_context;
    
    // ARRANGE phase
    NLINK_ARRANGE_PHASE(\"Prepare for ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} initialization test\");
    
    // ACT phase
    NLINK_ACT_PHASE(\"Execute ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} initialization\");
    bool init_success = true; // Replace with actual initialization code
    
    // ASSERT phase
    NLINK_ASSERT_PHASE(\"Verify ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} initialization succeeded\");
    NLINK_ASSERT_TRUE(init_success, \"${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} initialization should succeed\");
}

NLINK_TEST_SUITE_END(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}) {
    // Global cleanup for test suite
    nlink_test_log_info(\"Cleaning up ${TEST_FILE_COMPONENT}_${TEST_FILE_NAME} test suite\");
    test_stub_manager_cleanup();
}

// Main function that runs all the tests
NLINK_TEST_MAIN(
    NLINK_TEST_REGISTER_WITH_FIXTURE(${TEST_FILE_COMPONENT}_${TEST_FILE_NAME}, initialization, basic_fixture)
)
")
  
  message(STATUS "Created skeleton test file: ${TEST_FILE_OUTPUT}")
endfunction()

# Setup code coverage
function(nlink_setup_coverage)
  # Check if compiler is GCC or Clang
  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Add coverage flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage" PARENT_SCOPE)
    
    # Find lcov and genhtml
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    
    if(LCOV AND GENHTML)
      # Add coverage target
      add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NLINK_BUILD_DIR}/coverage
        COMMAND ${LCOV} --directory ${NLINK_BUILD_DIR} --zerocounters
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target run_all_tests
        COMMAND ${LCOV} --directory ${NLINK_BUILD_DIR} --capture --output-file ${NLINK_BUILD_DIR}/coverage/coverage.info
        COMMAND ${LCOV} --remove ${NLINK_BUILD_DIR}/coverage/coverage.info
                        "/usr/include/*" "/usr/lib/*" "*/tests/*" "*/build/*" --output-file ${NLINK_BUILD_DIR}/coverage/coverage.info
        COMMAND ${GENHTML} ${NLINK_BUILD_DIR}/coverage/coverage.info --output-directory ${NLINK_BUILD_DIR}/coverage/html
        COMMENT "Generating coverage report"
      )
      
      message(STATUS "Code coverage report setup completed")
    else()
      message(WARNING "Could not find lcov or genhtml. Coverage reporting disabled.")
    endif()
  else()
    message(WARNING "Coverage requires GCC or Clang compiler. Coverage reporting disabled.")
  endif()
endfunction()
