cmake_minimum_required(VERSION 3.16)
project(NexusLink VERSION 1.0.0 LANGUAGES C)

# ════════════════════════════════════════════════════════════
# COMPONENT FRAMEWORK INITIALIZATION
# ════════════════════════════════════════════════════════════

# Include core build system modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(BuildSystem)
include(Utils)
include(Hooks)
include(ComponentSystem)
include(TestFramework)
include(PackageConfig)

# ════════════════════════════════════════════════════════════
# BUILD OUTPUT STANDARDIZATION ENFORCEMENT
# ════════════════════════════════════════════════════════════

# Include build output standardization module
include(BuildOutputStandard)

# Initialize build system with standardized directories
# The BuildOutputStandard module automatically enforces:
# - ${CMAKE_BINARY_DIR}/obj   : Object files
# - ${CMAKE_BINARY_DIR}/bin   : Executables
# - ${CMAKE_BINARY_DIR}/lib   : Libraries
# - ${CMAKE_BINARY_DIR}/logs  : Build logs
nlink_init_hook_system(
  OBJ_DIR "${NLINK_OBJ_ROOT}"
  BIN_DIR "${NLINK_BIN_ROOT}"
  LIB_DIR "${NLINK_LIB_ROOT}"
  TEST_DIR "${CMAKE_BINARY_DIR}/tests"
  LOG_DIR "${NLINK_LOG_ROOT}"
  VERBOSE
)

# Define options
option(BUILD_TESTING "Build tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(NLINK_USE_CLI "Build with CLI support" ON)

# Initialize build system
nlink_init_build_system(
  VERSION "${PROJECT_VERSION}"
  ENABLE_TESTING ${BUILD_TESTING}
  ENABLE_COVERAGE ${ENABLE_COVERAGE}
  RELEASE_BUILD ${CMAKE_BUILD_TYPE} STREQUAL "Release"
)

# Find dependencies
nlink_find_dependencies()

# ════════════════════════════════════════════════════════════
# COMPONENT MATERIALIZATION SEQUENCE - CRITICAL BOUNDARY LAYER
# ════════════════════════════════════════════════════════════

# PRIMARY BOUNDARY: Core component materialization
add_subdirectory(src/core)

# SECONDARY BOUNDARY: CLI component materialization (if enabled)
if(NLINK_USE_CLI)
  add_subdirectory(src/cli)
endif()

# ════════════════════════════════════════════════════════════
# CROSS-BOUNDARY DEPENDENCY RESOLUTION
# ════════════════════════════════════════════════════════════

# Conditional cross-component dependencies - only after materialization
if(TARGET nlink_cli_components AND TARGET nlink_unified_core)
    add_dependencies(nlink_cli_components nlink_unified_core)
endif()

if(TARGET nlink_cli_static AND TARGET nlink_core)
    target_link_libraries(nlink_cli_static PRIVATE nlink_core)
endif()

# ════════════════════════════════════════════════════════════
# ARTIFACT PRODUCTION VALIDATION
# ════════════════════════════════════════════════════════════

# Ensure CLI artifact production (if CLI is enabled and target exists)
if(NLINK_USE_CLI AND TARGET nlink_cli_static)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a
        DEPENDS $<TARGET_FILE:nlink_cli_static>
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:nlink_cli_static> ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a
        COMMENT "Ensuring CLI static library artifact production"
        VERBATIM
    )
    add_custom_target(ensure_cli_artifact ALL DEPENDS ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a)
endif()

# ════════════════════════════════════════════════════════════
# MAIN EXECUTABLE DEFINITION
# ════════════════════════════════════════════════════════════

# Define main executable with proper core library dependency
if(NLINK_USE_CLI AND TARGET nlink_cli_static AND TARGET nlink_core)
    nlink_define_main_executable(
      NAME nlink_executable
      SOURCES ${CMAKE_SOURCE_DIR}/src/cli/main.c
      INCLUDES ${CMAKE_SOURCE_DIR}/src/cli
      DEPENDENCIES nlink_cli_static nlink_core pthread dl
    )
endif()

# ════════════════════════════════════════════════════════════
# AUXILIARY SYSTEM CONFIGURATION
# ════════════════════════════════════════════════════════════

# Generate unified header
nlink_generate_unified_header(
  OUTPUT "${CMAKE_BINARY_DIR}/include/nlink.h"
  COMPONENTS ${NLINK_CORE_COMPONENTS}
  VERBOSE
)

# Package configuration
nlink_init_package_config(
  VERSION "${PROJECT_VERSION}"
  DESCRIPTION "NexusLink: Dynamic Component Linkage System"
  VENDOR "NexusLink Team"
)

# Installation setup
nlink_install_unified_header()
nlink_install_development_components()
if(TARGET nlink_executable)
    nlink_install_executable(TARGET nlink_executable)
endif()

# Testing setup
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# ════════════════════════════════════════════════════════════
# DEVELOPMENT TARGETS
# ════════════════════════════════════════════════════════════

# Create debug/release targets
add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
  COMMENT "Building debug configuration"
)

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
  COMMENT "Building release configuration"
)

# Add validation targets
add_custom_target(validate
  DEPENDS validate_includes
  COMMENT "Validating codebase"
)

# Fix includes target
add_custom_target(fix-includes
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/fix_nlink_includes.sh --project-root ${CMAKE_CURRENT_SOURCE_DIR} --verbose
  COMMENT "Fixing include paths"
)

# Create development workflow target
add_custom_target(dev-cycle
  DEPENDS validate fix-includes nlink_static nlink_shared nlink_executable run_all_tests
  COMMENT "Running complete development workflow"
)

# ════════════════════════════════════════════════════════════
# DOCUMENTATION GENERATION
# ════════════════════════════════════════════════════════════

if(BUILD_DOCUMENTATION)
  find_package(Doxygen QUIET)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    set(DOXYGEN_PROJECT_NAME "NexusLink")
    set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
    set(DOXYGEN_PROJECT_BRIEF "Dynamic Component Linkage System")
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    
    add_custom_target(docs
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
    )
    
    message(STATUS "Doxygen found: Documentation target available")
  else()
    add_custom_target(docs
      COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found - documentation generation disabled"
      COMMENT "Doxygen not found - documentation generation disabled"
    )
  endif()
endif()

# ════════════════════════════════════════════════════════════
# BUILD OUTPUT VALIDATION AND ARTIFACT COLLECTION
# ════════════════════════════════════════════════════════════

# Final build validation target
add_custom_target(validate_build ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Validating build output structure..."
    COMMAND ${CMAKE_COMMAND} 
        -DNLINK_OUTPUT_ROOT=${CMAKE_BINARY_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -P ${CMAKE_SOURCE_DIR}/cmake/CollectArtifacts.cmake
    DEPENDS nlink_unified_core
    COMMENT "Validating standardized build output structure"
    VERBATIM
)

# Add dependency on CLI if enabled
if(NLINK_USE_CLI AND TARGET nlink_cli_components)
    add_dependencies(validate_build nlink_cli_components)
endif()

# Add dependency on executable if built
if(TARGET nlink_executable)
    add_dependencies(validate_build nlink_executable)
endif()

# ════════════════════════════════════════════════════════════
# BUILD SYSTEM STATUS REPORT
# ════════════════════════════════════════════════════════════

message(STATUS "════════════════════════════════════════════════════")
message(STATUS "NexusLink Build System Configuration Complete")
message(STATUS "════════════════════════════════════════════════════")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "Testing enabled:   ${BUILD_TESTING}")
message(STATUS "CLI support:       ${NLINK_USE_CLI}")
message(STATUS "Documentation:     ${BUILD_DOCUMENTATION}")
message(STATUS "Output structure:")
message(STATUS "  Objects:         ${NLINK_OBJ_ROOT}")
message(STATUS "  Binaries:        ${NLINK_BIN_ROOT}")
message(STATUS "  Libraries:       ${NLINK_LIB_ROOT}")
message(STATUS "  Logs:            ${NLINK_LOG_ROOT}")
message(STATUS "════════════════════════════════════════════════════")