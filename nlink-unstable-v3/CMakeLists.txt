cmake_minimum_required(VERSION 3.13)
project(NexusLink VERSION 1.0.0 LANGUAGES C)

# Include core build system modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(BuildSystem)
include(Utils)
include(Hooks)
include(ComponentSystem)
include(TestFramework)
include(PackageConfig)

add_dependencies(nlink_cli_components nlink_unified_core)
# Implement explicit CLI-to-Core dependency
target_link_libraries(nlink_cli_static PRIVATE nlink_core)

# Create explicit file-level dependency to enforce build order
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a
  DEPENDS ${CMAKE_BINARY_DIR}/lib/libnlink_core.a
  APPEND
)
# Define main executable with proper core library dependency
nlink_define_main_executable(
  NAME nlink_executable
  SOURCES ${CMAKE_SOURCE_DIR}/src/cli/main.c
  INCLUDES ${CMAKE_SOURCE_DIR}/src/cli
  DEPENDENCIES nlink_cli_static nlink_core
)

# Initialize build system with directories
nlink_init_hook_system(
  OBJ_DIR "${CMAKE_BINARY_DIR}/obj"
  BIN_DIR "${CMAKE_BINARY_DIR}/bin"
  LIB_DIR "${CMAKE_BINARY_DIR}/lib"
  TEST_DIR "${CMAKE_BINARY_DIR}/tests"
  LOG_DIR "${CMAKE_BINARY_DIR}/logs"
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

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# Core component definitions - discover components first to ensure proper registration
file(GLOB CORE_COMPONENTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/core" "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*")
set(VALID_CORE_COMPONENTS "")

foreach(COMPONENT ${CORE_COMPONENTS})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/core/${COMPONENT}")
    list(APPEND VALID_CORE_COMPONENTS ${COMPONENT})
    nlink_register_component(${COMPONENT})
    # Create component object directory
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/obj/core/${COMPONENT}")
  endif()
endforeach()

message(STATUS "Discovered core components: ${VALID_CORE_COMPONENTS}")

# Add core components
add_subdirectory("src/core")

# CLI components
if(NLINK_USE_CLI)
  add_subdirectory("src/cli")
endif()

# Main executable
nlink_define_main_executable(
  NAME "nlink"
  VERSION "${PROJECT_VERSION}"
  SOURCES "${NLINK_SRC_DIR}/cli/main.c"
  DEPENDENCIES nlink_core_static nlink_cli_static pthread dl
)

# Generate unified header
nlink_generate_unified_header(
  OUTPUT "${NLINK_BUILD_DIR}/include/nlink.h"
  TEMPLATE "${NLINK_PROJECT_ROOT}/templates/nlink.h.in"
  COMPONENTS ${VALID_CORE_COMPONENTS}
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
nlink_install_executable(TARGET nlink)

# Testing setup
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

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
  COMMAND ${NLINK_PROJECT_ROOT}/scripts/fix_nlink_includes.sh --project-root ${NLINK_PROJECT_ROOT} --verbose
  COMMENT "Fixing include paths"
)

# Create development workflow target
add_custom_target(dev-cycle
  DEPENDS validate fix-includes nlink_static nlink_shared nlink_executable run_all_tests
  COMMENT "Running complete development workflow"
)

# Documentation generation (if Doxygen is available)
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
    
    # Generate Doxyfile
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/templates/Doxyfile.in")
      configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/templates/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
      )
    else()
      message(STATUS "Doxyfile template not found - using default configuration")
      doxygen_add_docs(docs
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        COMMENT "Generating API documentation with Doxygen"
      )
    endif()
    
    message(STATUS "Doxygen found: Documentation target available")
  else()
    add_custom_target(docs
      COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found - documentation generation disabled"
      COMMENT "Doxygen not found - documentation generation disabled"
    )
    message(STATUS "Doxygen not found - documentation generation disabled")
  endif()
endif()

# Execute post-build hooks
nlink_execute_post_build_hooks()

# Print build system status
message(STATUS "NexusLink build system configuration complete")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Testing enabled: ${BUILD_TESTING}")
message(STATUS "CLI support: ${NLINK_USE_CLI}")
message(STATUS "Registered components: ${VALID_CORE_COMPONENTS}")