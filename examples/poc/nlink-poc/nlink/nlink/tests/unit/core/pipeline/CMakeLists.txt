# CMakeLists.txt for NexusLink unit/core/pipeline tests
cmake_minimum_required(VERSION 3.13)

# Include the test framework module
include(TestFramework)

# Create component stubs if needed
nlink_create_component_stubs(pipeline)

# Create target for pipeline unit tests
add_custom_target(unit_core_pipeline_tests
    COMMENT "pipeline unit tests target"
)

# Get all test sources in this directory
file(GLOB pipeline_TEST_SOURCES "*.c")

# Add each test file
foreach(TEST_SOURCE ${pipeline_TEST_SOURCES})
    # Get test name from file name
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Add the test using the AAA pattern
    nlink_add_aaa_test(
        NAME ${TEST_NAME}
        COMPONENT "pipeline"
        SOURCES ${TEST_SOURCE}
        MOCK_COMPONENTS "pipeline"
    )
endforeach()

# Create a target that runs all pipeline tests
add_custom_target(run_core_pipeline_tests
    DEPENDS unit_core_pipeline_tests
    COMMENT "Running all pipeline tests"
)

# Add this component's tests to the unit_core_tests target
add_dependencies(unit_tests unit_core_pipeline_tests)
