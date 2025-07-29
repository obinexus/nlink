# BuildSystem.cmake
# Core build system for NexusLink
# 
# This module provides central coordination for the NexusLink build system,
# integrating component compilation, testing, and package generation.

if(DEFINED NLINK_BUILD_SYSTEM_INCLUDED)
  return()
endif()
set(NLINK_BUILD_SYSTEM_INCLUDED TRUE)

# Include all required modules
include(CMakeParseArguments)
include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ComponentSystem.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/TestFramework.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PackageConfig.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ExecutableConfig.cmake)

# Define core project directories
set(NLINK_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
set(NLINK_SRC_DIR "${NLINK_PROJECT_ROOT}/src")
set(NLINK_INCLUDE_DIR "${NLINK_PROJECT_ROOT}/include")
set(NLINK_BUILD_DIR "${CMAKE_BINARY_DIR}")
set(NLINK_OBJ_DIR "${NLINK_BUILD_DIR}/obj")
set(NLINK_LIB_DIR "${NLINK_BUILD_DIR}/lib")
set(NLINK_BIN_DIR "${NLINK_BUILD_DIR}/bin")
set(NLINK_TEST_DIR "${NLINK_PROJECT_ROOT}/tests")
set(NLINK_LOG_DIR "${NLINK_PROJECT_ROOT}/logs")

# Create required directories
nlink_ensure_directory(${NLINK_BUILD_DIR})
nlink_ensure_directory(${NLINK_OBJ_DIR})
nlink_ensure_directory(${NLINK_LIB_DIR})
nlink_ensure_directory(${NLINK_BIN_DIR})
nlink_ensure_directory(${NLINK_LOG_DIR})

# NexusLink core components
set(NLINK_CORE_COMPONENTS
    "common" 
    "minimizer"
    "symbols"
    "versioning"
    "pipeline"
    "pattern"
    "mpsystem"
    "spsystem"
    "metadata"
)

# CLI components
set(NLINK_CLI_COMPONENTS
    "cli"
    "commands"
)

# Create component object directories
foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
  nlink_ensure_directory(${NLINK_OBJ_DIR}/core/${COMPONENT})
endforeach()

foreach(COMPONENT ${NLINK_CLI_COMPONENTS})
  nlink_ensure_directory(${NLINK_OBJ_DIR}/cli/${COMPONENT})
endforeach()

# Setup common compilation flags
set(NLINK_C_STANDARD 11)
set(NLINK_CXX_STANDARD 14)

set(NLINK_WARNING_FLAGS "-Wall -Wextra -Wno-unused-parameter")
set(NLINK_DEBUG_FLAGS "-g -DDEBUG")
set(NLINK_RELEASE_FLAGS "-O2 -DNDEBUG")

# Function to initialize the build system
function(nlink_init_build_system)
  cmake_parse_arguments(
    INIT
    "ENABLE_TESTING;ENABLE_COVERAGE;RELEASE_BUILD"
    "VERSION"
    ""
    ${ARGN}
  )

  # Set default version if not provided
  if(NOT INIT_VERSION)
    set(INIT_VERSION "1.0.0")
  endif()

  # Configure global compiler settings
  set(CMAKE_C_STANDARD ${NLINK_C_STANDARD} PARENT_SCOPE)
  set(CMAKE_CXX_STANDARD ${NLINK_CXX_STANDARD} PARENT_SCOPE)
  
  # Add include directories
  include_directories(${NLINK_INCLUDE_DIR} ${NLINK_SRC_DIR})
  
  # Set build type flags
  if(INIT_RELEASE_BUILD)
    add_compile_options(${NLINK_WARNING_FLAGS} ${NLINK_RELEASE_FLAGS})
    set(CMAKE_BUILD_TYPE "Release" PARENT_SCOPE)
  else()
    add_compile_options(${NLINK_WARNING_FLAGS} ${NLINK_DEBUG_FLAGS})
    set(CMAKE_BUILD_TYPE "Debug" PARENT_SCOPE)
  endif()
  
  # Setup testing if enabled
  if(INIT_ENABLE_TESTING)
    enable_testing()
    
    # Initialize test infrastructure
    nlink_init_test_system(
      COMPONENTS ${NLINK_CORE_COMPONENTS}
      COVERAGE ${INIT_ENABLE_COVERAGE}
    )
  endif()
  
  # Initialize component system
  nlink_init_component_system(
    COMPONENTS ${NLINK_CORE_COMPONENTS}
    VERSION ${INIT_VERSION}
  )
  
  # Define global targets
  nlink_define_global_targets()
  
  # Set global properties
  set_property(GLOBAL PROPERTY NLINK_VERSION ${INIT_VERSION})
  set_property(GLOBAL PROPERTY NLINK_TESTING_ENABLED ${INIT_ENABLE_TESTING})
  set_property(GLOBAL PROPERTY NLINK_COVERAGE_ENABLED ${INIT_ENABLE_COVERAGE})
  
  message(STATUS "NexusLink build system initialized (version ${INIT_VERSION})")
  message(STATUS "Testing enabled: ${INIT_ENABLE_TESTING}")
  message(STATUS "Coverage enabled: ${INIT_ENABLE_COVERAGE}")
  message(STATUS "Release build: ${INIT_RELEASE_BUILD}")
endfunction()

# Function to define global targets
function(nlink_define_global_targets)
  # Static library target
  if(NOT TARGET nlink_static)
    add_custom_target(nlink_static
      DEPENDS nlink_core_components
      COMMENT "Building NexusLink static library"
    )
  endif()
  
  # Shared library target
  if(NOT TARGET nlink_shared)
    add_custom_target(nlink_shared
      DEPENDS nlink_core_components
      COMMENT "Building NexusLink shared library"
    )
  endif()
  
  # Main executable target
  if(NOT TARGET nlink_executable)
    add_custom_target(nlink_executable
      DEPENDS nlink_static
      COMMENT "Building NexusLink executable"
    )
  endif()
  
  # Include standardization target
  if(NOT TARGET validate_includes)
    add_custom_target(validate_includes
      COMMAND ${CMAKE_COMMAND} -E echo "Validating include paths..."
      COMMAND ${Python3_EXECUTABLE} ${NLINK_PROJECT_ROOT}/scripts/standardize_nlink_includes.py
        --project-root ${NLINK_PROJECT_ROOT}
        --validate
      COMMENT "Validating include paths"
    )
  endif()
  
  # Include fix target
  if(NOT TARGET fix_includes)
    add_custom_target(fix_includes
      COMMAND ${CMAKE_COMMAND} -E echo "Fixing include paths..."
      COMMAND ${NLINK_PROJECT_ROOT}/scripts/fix_nlink_includes.sh
        --project-root ${NLINK_PROJECT_ROOT}
        --verbose
      COMMENT "Fixing include paths"
    )
  endif()
  
  # Complete development workflow target
  if(NOT TARGET dev_cycle)
    add_custom_target(dev_cycle
      DEPENDS validate_includes fix_includes nlink_static nlink_shared nlink_executable run_all_tests
      COMMENT "Running complete development cycle"
    )
  endif()
  
  # Documentation target
  if(NOT TARGET docs)
    add_custom_target(docs
      COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
      COMMENT "Generating documentation"
    )
  endif()
  
  # Package target
  if(NOT TARGET package_all)
    add_custom_target(package_all
      DEPENDS nlink_static nlink_shared nlink_executable
      COMMAND ${CMAKE_COMMAND} -E echo "Creating packages..."
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
      COMMENT "Creating packages"
    )
  endif()
endfunction()

# Set timestamp for log files

string(TIMESTAMP NLINK_TIMESTAMP "%Y%m%d_%H%M%S")
