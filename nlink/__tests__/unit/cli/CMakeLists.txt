# CLI Test Component CMakeLists.txt

# Define CLI tests
set(CLI_TEST_SOURCES
    test_command_params.c
    test_command_registry.c
    test_command_router.c
    test_regex_matcher.c
    test_cli.c
)

# Enable testing
enable_testing()

# Find required packages
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})

# Add each test as an executable
foreach(test_src ${CLI_TEST_SOURCES})
    # Get the test name without extension
    get_filename_component(test_name ${test_src} NAME_WE)
    
    # Add executable
    add_executable(${test_name} ${test_src})
    
    # Link against required libraries
    target_link_libraries(${test_name}
        PRIVATE
            nexus_cli
            nexus_common
            nexus_minimizer
            ${CHECK_LIBRARIES}
            pthread
    )
    
    # Add include directories
    target_include_directories(${test_name}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )
    
    # Add test
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Add integration test for full CLI
add_executable(test_cli_integration test_cli_integration.c)
target_link_libraries(test_cli_integration
    PRIVATE
        nexus_cli
        nlink
)
target_include_directories(test_cli_integration
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
add_test(NAME test_cli_integration COMMAND test_cli_integration)

# Helper script to run all tests with valgrind if available
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_custom_target(cli_test_memcheck
        COMMAND ${CMAKE_COMMAND} -E echo "Running CLI tests with Valgrind..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/memcheck
        COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
            --force-new-ctest-process
            --test-action memcheck
            --output-on-failure
            --output-log ${CMAKE_BINARY_DIR}/memcheck/cli_memcheck.log
        COMMAND ${CMAKE_COMMAND} -E echo "Valgrind log written to ${CMAKE_BINARY_DIR}/memcheck/cli_memcheck.log"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VERBATIM
    )
endif()