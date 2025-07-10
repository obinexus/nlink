# CMakeLists.txt for NexusLink unit/core/symbols tests
cmake_minimum_required(VERSION 3.13)

# Include the test framework module
include(TestFramework)

# Create component stubs if needed
nlink_create_component_stubs(symbols)

# Create target for symbols unit tests
add_custom_target(unit_core_symbols_tests
    COMMENT "symbols unit tests target"
)

# Get all test sources in this directory
file(GLOB symbols_TEST_SOURCES "*.c")

# Add each test file
foreach(TEST_SOURCE ${symbols_TEST_SOURCES})
    # Get test name from file name
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Add the test using the AAA pattern
    nlink_add_aaa_test(
        NAME ${TEST_NAME}
        COMPONENT "symbols"
        SOURCES ${TEST_SOURCE}
        MOCK_COMPONENTS "symbols"
    )
endforeach()

# Create a target that runs all symbols tests
add_custom_target(run_core_symbols_tests
    DEPENDS unit_core_symbols_tests
    COMMENT "Running all symbols tests"
)

# Add this component's tests to the unit_core_tests target
add_dependencies(unit_tests unit_core_symbols_tests)
