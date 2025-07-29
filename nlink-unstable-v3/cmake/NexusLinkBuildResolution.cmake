# NexusLinkBuildResolution.cmake
# Master build orchestration script for proper initialization sequencing
# Copyright © 2025 OBINexus Computing

# ════════════════════════════════════════════════════════════
# BUILD SYSTEM INITIALIZATION PROTOCOL
# ════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.16)

# Include this at the very beginning of your root CMakeLists.txt
# after the project() command

# ════════════════════════════════════════════════════════════
# MODULE LOADING SEQUENCE (CRITICAL ORDER)
# ════════════════════════════════════════════════════════════

# 1. Load utility functions first
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

# 2. Initialize hooks system with proper directories
include(${CMAKE_SOURCE_DIR}/cmake/Hooks.cmake)
nlink_init_hook_system(
    OBJ_DIR "${CMAKE_BINARY_DIR}/obj"
    BIN_DIR "${CMAKE_BINARY_DIR}/bin"
    LIB_DIR "${CMAKE_BINARY_DIR}/lib"
    TEST_DIR "${CMAKE_BINARY_DIR}/tests"
    LOG_DIR "${CMAKE_BINARY_DIR}/logs"
)

# 3. Load build output standardization
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake")
    include(${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake)
endif()

# 4. Initialize component system
include(${CMAKE_SOURCE_DIR}/cmake/ComponentSystem.cmake)

# 5. Load executable configuration (with new unified implementation)
include(${CMAKE_SOURCE_DIR}/cmake/ExecutableConfig.cmake)

# 6. Load package configuration
include(${CMAKE_SOURCE_DIR}/cmake/PackageConfig.cmake)

# 7. Initialize test framework if testing enabled
if(BUILD_TESTING OR NEXUS_BUILD_TESTS)
    include(${CMAKE_SOURCE_DIR}/cmake/TestFramework.cmake)
endif()

# ════════════════════════════════════════════════════════════
# COMPONENT MATERIALIZATION SEQUENCE
# ════════════════════════════════════════════════════════════

# Core components must be built before CLI
add_subdirectory(src/core)

# CLI components depend on core
add_subdirectory(src/cli)

# ════════════════════════════════════════════════════════════
# EXECUTABLE DEFINITION (SINGLETON ENFORCEMENT)
# ════════════════════════════════════════════════════════════

# Define main executable ONLY ONCE
nlink_define_main_executable(
    SOURCES ${CMAKE_SOURCE_DIR}/src/cli/main.c
    STATIC_LINKING ${NEXUS_STATIC_BUILD}
    ENABLE_PROFILING ${NEXUS_ENABLE_PROFILING}
)

# ════════════════════════════════════════════════════════════
# DEPENDENCY INJECTION
# ════════════════════════════════════════════════════════════

# Ensure proper dependency chain
if(TARGET nlink_cli_static AND TARGET nlink_core)
    add_dependencies(nlink_cli_static nlink_core)
endif()

if(TARGET nlink_cli_components AND TARGET nlink_unified_core)
    add_dependencies(nlink_cli_components nlink_unified_core)
endif()

# ════════════════════════════════════════════════════════════
# POST-BUILD VALIDATION
# ════════════════════════════════════════════════════════════

# Validate artifact production
add_custom_target(validate_build ALL
    COMMAND ${CMAKE_COMMAND} -E echo "════════════════════════════════════════════════════"
    COMMAND ${CMAKE_COMMAND} -E echo "NexusLink Build Validation"
    COMMAND ${CMAKE_COMMAND} -E echo "════════════════════════════════════════════════════"
    COMMAND ${CMAKE_COMMAND} -E echo "Checking core library..."
    COMMAND ${CMAKE_COMMAND} -E echo "  ${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
    COMMAND ${CMAKE_COMMAND} -E echo "Checking CLI library..."
    COMMAND ${CMAKE_COMMAND} -E echo "  ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a"
    COMMAND ${CMAKE_COMMAND} -E echo "Checking executable..."
    COMMAND ${CMAKE_COMMAND} -E echo "  ${CMAKE_BINARY_DIR}/bin/nlink"
    COMMAND ${CMAKE_COMMAND} -E echo "════════════════════════════════════════════════════"
    VERBATIM
)

# Add validation dependency
if(TARGET nlink)
    add_dependencies(validate_build nlink)
endif()

# ════════════════════════════════════════════════════════════
# INSTALLATION CONFIGURATION
# ════════════════════════════════════════════════════════════

# Initialize package configuration with correct template location
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/NexusLinkConfig.cmake.in")
    nlink_init_package_config(
        VERSION ${PROJECT_VERSION}
        COMPONENTS ${NLINK_CORE_COMPONENTS}
    )
else()
    message(WARNING "Package configuration template not found - skipping package config")
endif()

# Install main executable
if(COMMAND nlink_install_main_executable)
    nlink_install_main_executable()
endif()

message(STATUS "════════════════════════════════════════════════════")
message(STATUS "NexusLink Build Resolution Complete")
message(STATUS "════════════════════════════════════════════════════")