# CMakeLists.txt for NexusLink Unit Tests
# Copyright © 2025 OBINexus Computing

# ════════════════════════════════════════════════════════════
# CORE COMPONENT UNIT TESTS
# ════════════════════════════════════════════════════════════

# Add core component test subdirectories
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core")
    add_subdirectory(core)
else()
    # Create placeholder for core tests
    message(STATUS "Creating unit test infrastructure for core components")
    
    # Iterate through core components and create test targets
    get_property(CORE_COMPONENTS GLOBAL PROPERTY NLINK_CORE_COMPONENTS)
    foreach(COMPONENT ${CORE_COMPONENTS})
        # Create test executable name
        set(TEST_NAME test_${COMPONENT})
        
        # Check if test source exists
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.c")
            nlink_add_aaa_test(
                NAME ${COMPONENT}_basic
                COMPONENT ${COMPONENT}
                SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.c
                MOCK_COMPONENTS ${COMPONENT}
            )
        endif()
    endforeach()
endif()

# ════════════════════════════════════════════════════════════
# CLI COMPONENT UNIT TESTS
# ════════════════════════════════════════════════════════════

if(NLINK_USE_CLI AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cli")
    add_subdirectory(cli)
endif()

# ════════════════════════════════════════════════════════════
# TEST EXECUTION AGGREGATION
# ════════════════════════════════════════════════════════════

# Create aggregated test execution target
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -R "^test_unit_" --output-on-failure
    DEPENDS unit_tests
    COMMENT "Running all unit tests"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ════════════════════════════════════════════════════════════
# UNIT TEST STATUS REPORT
# ════════════════════════════════════════════════════════════

message(STATUS "Unit test configuration complete")
message(STATUS "  Core components: ${CORE_COMPONENTS}")
message(STATUS "  CLI enabled: ${NLINK_USE_CLI}")
