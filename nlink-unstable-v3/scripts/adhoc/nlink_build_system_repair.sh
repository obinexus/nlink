#!/bin/bash
# nlink_build_system_repair.sh
# Comprehensive build system restoration protocol for NexusLink
# Copyright © 2025 OBINexus Computing

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}" && pwd)"

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}════════════════════════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}   NexusLink Build System Comprehensive Repair Protocol   ${NC}"
echo -e "${BLUE}════════════════════════════════════════════════════════════════════════════════${NC}"

# Structural validation
if [[ ! -d "${PROJECT_ROOT}/cmake" ]]; then
    echo -e "${RED}FATAL: CMAKE module directory not found${NC}"
    exit 1
fi

# Layer 1: Fix Main CMakeLists.txt to filter CMakeFiles directory
echo -e "\n${YELLOW}[PHASE 1]${NC} Core CMakeLists.txt Correction"

CORE_CMAKE_PATH="${PROJECT_ROOT}/src/core/CMakeLists.txt"
if [[ -f "${CORE_CMAKE_PATH}" ]]; then
    cp "${CORE_CMAKE_PATH}" "${CORE_CMAKE_PATH}.bak"
    
    # Filter out CMakeFiles directory and implement correct component processing
    cat > "${CORE_CMAKE_PATH}" << 'EOF'
# Core components CMakeLists.txt for NexusLink
# Copyright © 2025 OBINexus Computing

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# Define core components
set(NLINK_CORE_COMPONENTS "")
file(GLOB COMPONENT_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")

# Extract valid component directories (filter out CMakeFiles and other special directories)
foreach(DIR ${COMPONENT_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}" AND
       NOT "${DIR}" STREQUAL "CMakeFiles" AND
       NOT "${DIR}" STREQUAL "cmake" AND
       NOT "${DIR}" MATCHES "^\\.")
        
        message(STATUS "Found core component: ${DIR}")
        list(APPEND NLINK_CORE_COMPONENTS ${DIR})
    endif()
endforeach()

# Process components with unique build directories
foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
    # Define unique binary build directory for each component
    set(COMPONENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT}_build")
    
    # Add subdirectory with specified binary directory to prevent collisions
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}" 
                         "${COMPONENT_BINARY_DIR}")
    else()
        message(WARNING "Component directory ${COMPONENT} missing CMakeLists.txt")
    endif()
endforeach()

# Set global component list property
set_property(GLOBAL PROPERTY NLINK_CORE_COMPONENTS ${NLINK_CORE_COMPONENTS})

# Execute post-build hooks
nlink_execute_post_build_hooks()

# Build summary
message(STATUS "=== NexusLink Core Build Summary ===")
message(STATUS "Components: ${NLINK_CORE_COMPONENTS}")

# Get lists of static and shared libraries
get_property(STATIC_LIBS GLOBAL PROPERTY NLINK_STATIC_LIBRARIES)
get_property(SHARED_LIBS GLOBAL PROPERTY NLINK_SHARED_LIBRARIES)

message(STATUS "Static libraries: ${STATIC_LIBS}")
message(STATUS "Shared libraries: ${SHARED_LIBS}")
message(STATUS "======================================")
EOF
    
    echo -e "${GREEN}✓ Core CMakeLists.txt corrected${NC}"
else
    echo -e "${RED}✗ Core CMakeLists.txt not found at ${CORE_CMAKE_PATH}${NC}"
fi

# Layer 2: Implement missing nlink_define_main_executable function
echo -e "\n${YELLOW}[PHASE 2]${NC} Adding Main Executable Definition Function"

EXECUTABLE_IMPL_PATH="${PROJECT_ROOT}/cmake/ExecutableConfig.cmake"
cat > "${EXECUTABLE_IMPL_PATH}" << 'EOF'
# ExecutableConfig.cmake
# Main executable configuration for NexusLink
#
# This module provides functionality for defining and configuring the main
# NexusLink executable with proper component dependencies.

# Include guard
if(DEFINED NLINK_EXECUTABLE_CONFIG_CMAKE_INCLUDED)
  return()
endif()
set(NLINK_EXECUTABLE_CONFIG_CMAKE_INCLUDED TRUE)

include(CMakeParseArguments)

# Function to define the main executable with component dependencies
function(nlink_define_main_executable)
  cmake_parse_arguments(
    EXEC
    "INSTALL;VERSIONED"
    "NAME;VERSION;DESCRIPTION;OUTPUT_NAME"
    "SOURCES;INCLUDES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS"
    ${ARGN}
  )

  # Validate required arguments
  if(NOT EXEC_NAME)
    set(EXEC_NAME "nlink")
  endif()

  if(NOT EXEC_OUTPUT_NAME)
    set(EXEC_OUTPUT_NAME "${EXEC_NAME}")
  endif()

  if(NOT EXEC_VERSION)
    # Get version from global property
    get_property(NLINK_VERSION GLOBAL PROPERTY NLINK_VERSION)
    if(NLINK_VERSION)
      set(EXEC_VERSION "${NLINK_VERSION}")
    else()
      set(EXEC_VERSION "1.0.0")
    endif()
  endif()

  # Define main executable target
  add_executable(${EXEC_NAME} ${EXEC_SOURCES})
  
  # Set properties
  set_target_properties(${EXEC_NAME} PROPERTIES
    OUTPUT_NAME ${EXEC_OUTPUT_NAME}
    VERSION ${EXEC_VERSION}
  )

  # Add include directories
  target_include_directories(${EXEC_NAME} PRIVATE
    ${NLINK_INCLUDE_DIR}
    ${EXEC_INCLUDES}
  )

  # Add compile definitions
  if(EXEC_COMPILE_DEFINITIONS)
    target_compile_definitions(${EXEC_NAME} PRIVATE
      ${EXEC_COMPILE_DEFINITIONS}
    )
  endif()

  # Add compile options
  if(EXEC_COMPILE_OPTIONS)
    target_compile_options(${EXEC_NAME} PRIVATE
      ${EXEC_COMPILE_OPTIONS}
    )
  endif()

  # Link with all components
  get_property(NLINK_CORE_COMPONENTS GLOBAL PROPERTY NLINK_CORE_COMPONENTS)
  
  foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
    target_link_libraries(${EXEC_NAME} PRIVATE nlink_component_${COMPONENT})
  endforeach()
  
  # Link with additional dependencies
  if(EXEC_DEPENDENCIES)
    target_link_libraries(${EXEC_NAME} PRIVATE ${EXEC_DEPENDENCIES})
  endif()
  
  # Install if requested
  if(EXEC_INSTALL)
    install(TARGETS ${EXEC_NAME}
      RUNTIME DESTINATION bin
    )
  endif()
  
  # Create versioned symlink if requested
  if(EXEC_INSTALL AND EXEC_VERSIONED)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink 
      ${CMAKE_INSTALL_PREFIX}/bin/${EXEC_OUTPUT_NAME}
      ${CMAKE_INSTALL_PREFIX}/bin/${EXEC_OUTPUT_NAME}-${EXEC_VERSION}
    )")
  endif()
  
  # Set global property
  set_property(GLOBAL PROPERTY NLINK_MAIN_EXECUTABLE ${EXEC_NAME})
  
  message(STATUS "Defined main executable: ${EXEC_NAME} (version ${EXEC_VERSION})")
endfunction()
EOF

echo -e "${GREEN}✓ Created ExecutableConfig.cmake with nlink_define_main_executable function${NC}"

# Layer 3: Fix Main CMakeLists.txt to include ExecutableConfig.cmake
echo -e "\n${YELLOW}[PHASE 3]${NC} Updating Main CMakeLists.txt to Include ExecutableConfig"

MAIN_CMAKE_PATH="${PROJECT_ROOT}/CMakeLists.txt"
if [[ -f "${MAIN_CMAKE_PATH}" ]]; then
    # Check if ExecutableConfig.cmake is already included
    if ! grep -q "ExecutableConfig.cmake" "${MAIN_CMAKE_PATH}"; then
        sed -i '/include(.*BuildSystem.cmake)/a include(${CMAKE_CURRENT_LIST_DIR}/cmake/ExecutableConfig.cmake)' "${MAIN_CMAKE_PATH}"
        echo -e "${GREEN}✓ Added ExecutableConfig.cmake include to main CMakeLists.txt${NC}"
    else
        echo -e "${BLUE}ℹ ExecutableConfig.cmake already included in main CMakeLists.txt${NC}"
    fi
else
    echo -e "${RED}✗ Main CMakeLists.txt not found at ${MAIN_CMAKE_PATH}${NC}"
fi

# Layer 4: Fix CLI CMakeLists.txt set_property errors
echo -e "\n${YELLOW}[PHASE 4]${NC} Correcting CLI CMakeLists.txt set_property Calls"

CLI_CMAKE_PATH="${PROJECT_ROOT}/src/cli/CMakeLists.txt"
if [[ -f "${CLI_CMAKE_PATH}" ]]; then
    cp "${CLI_CMAKE_PATH}" "${CLI_CMAKE_PATH}.bak"
    
    # Correct set_property calls
    sed -i 's/set_property(GLOBAL CLI_COMPONENTS/set_property(GLOBAL PROPERTY CLI_COMPONENTS/g' "${CLI_CMAKE_PATH}"
    sed -i 's/set_property(GLOBAL CLI_CONFIGURED/set_property(GLOBAL PROPERTY CLI_CONFIGURED/g' "${CLI_CMAKE_PATH}"
    
    echo -e "${GREEN}✓ CLI CMakeLists.txt set_property calls corrected${NC}"
else
    echo -e "${YELLOW}! CLI CMakeLists.txt not found at ${CLI_CMAKE_PATH}${NC}"
fi

# Layer 5: Create standardized component CMakeLists.txt files
echo -e "\n${YELLOW}[PHASE 5]${NC} Standardizing Component CMakeLists.txt Files"

declare -a COMPONENTS=("common" "config" "metadata" "minimizer" "mpsystem" "parser" 
                    "pattern" "pattern_matching" "pipeline" "schema" "spsystem" 
                    "symbols" "tactic" "tatit" "token_type" "token_value" 
                    "tokenizer" "type" "versioning")

for component in "${COMPONENTS[@]}"; do
    COMPONENT_DIR="${PROJECT_ROOT}/src/core/${component}"
    if [[ -d "${COMPONENT_DIR}" ]]; then
        COMPONENT_CMAKE="${COMPONENT_DIR}/CMakeLists.txt"
        
        cat > "${COMPONENT_CMAKE}" << EOF
# CMakeLists.txt for NexusLink ${component} module
# Copyright © 2025 OBINexus Computing

# Get the directory name for the current module
get_filename_component(MODULE_NAME \${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Define source files
file(GLOB MODULE_SOURCES "*.c")
file(GLOB MODULE_HEADERS "*.h")

# Add library target with corrected parameter structure
nlink_build_component(
    COMPONENT \${MODULE_NAME}
    SOURCES \${MODULE_SOURCES}
    HEADERS \${MODULE_HEADERS}
    TYPE "core"
    VERBOSE
)

# Consolidate object files
foreach(SOURCE_FILE \${MODULE_SOURCES})
    get_filename_component(OBJECT_NAME \${SOURCE_FILE} NAME_WE)
    set(OBJECT_FILE "\${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/nlink_component_\${MODULE_NAME}.dir/\${OBJECT_NAME}.o")
    
    if(EXISTS "\${OBJECT_FILE}")
        file(COPY "\${OBJECT_FILE}" 
             DESTINATION "\${NLINK_OBJ_DIR}/core/\${MODULE_NAME}")
    endif()
endforeach()
EOF
        
        echo -e "${GREEN}✓ Standardized ${component} CMakeLists.txt${NC}"
        
        # Create minimal implementation files if needed
        if [[ ! -f "${COMPONENT_DIR}/${component}.c" ]]; then
            cat > "${COMPONENT_DIR}/${component}.c" << EOF
/**
 * @file ${component}.c
 * @brief Implementation of the NexusLink ${component} subsystem
 * @copyright OBINexus Computing, 2025
 */

#include "${component}.h"
#include <stddef.h>

int ${component}_initialize(void) {
    return 0;
}

int ${component}_shutdown(void) {
    return 0;
}
EOF
        fi
        
        if [[ ! -f "${COMPONENT_DIR}/${component}.h" ]]; then
            cat > "${COMPONENT_DIR}/${component}.h" << EOF
/**
 * @file ${component}.h
 * @brief Header for the NexusLink ${component} subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_${component^^}_H
#define NLINK_${component^^}_H

#ifdef __cplusplus
extern "C" {
#endif

int ${component}_initialize(void);
int ${component}_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_${component^^}_H */
EOF
        fi
    else
        echo -e "${BLUE}ℹ Creating component directory: ${component}${NC}"
        mkdir -p "${COMPONENT_DIR}"
        
        # Create component directory and call the same script recursively
        "${BASH_SOURCE[0]}"
    fi
done

echo -e "\n${GREEN}Build System Repair Protocol Complete${NC}"
echo -e "${BLUE}────────────────────────────────────────────────────────────────────────────────${NC}"
echo -e "${YELLOW}Execution Protocol:${NC}"
echo -e "  1. ${BLUE}make clean${NC}            # Clear build artifacts"
echo -e "  2. ${BLUE}make${NC}                  # Rebuild with corrected system"
echo -e "${BLUE}────────────────────────────────────────────────────────────────────────────────${NC}"
