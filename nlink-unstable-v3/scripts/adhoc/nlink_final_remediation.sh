#!/bin/bash
# nlink_final_remediation.sh
# Final build system remediation protocol for NexusLink
# Copyright © 2025 OBINexus Computing

set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}════════════════════════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}   NexusLink Build System Final Remediation Protocol   ${NC}"
echo -e "${BLUE}════════════════════════════════════════════════════════════════════════════════${NC}"

# 1. Fix CLI CMakeLists.txt property declarations
echo -e "\n${YELLOW}[PHASE 1]${NC} Correcting CLI Property Declarations"

CLI_CMAKE_PATH="${PROJECT_ROOT}/src/cli/CMakeLists.txt"
if [[ -f "${CLI_CMAKE_PATH}" ]]; then
    cp "${CLI_CMAKE_PATH}" "${CLI_CMAKE_PATH}.bak"
    
    # Replace properties with property and preserve indentation
    sed -i 's/set_property(TARGET nlink_cli_objects PROPERTIES/set_property(TARGET nlink_cli_objects PROPERTY/g' "${CLI_CMAKE_PATH}"
    sed -i 's/set_property(TARGET nlink_commands_objects PROPERTIES/set_property(TARGET nlink_commands_objects PROPERTY/g' "${CLI_CMAKE_PATH}"
    
    echo -e "${GREEN}✓ CLI CMakeLists.txt property declarations corrected${NC}"
else
    echo -e "${RED}✗ CLI CMakeLists.txt not found at ${CLI_CMAKE_PATH}${NC}"
fi

# 2. Create ExecutableConfig.cmake module
echo -e "\n${YELLOW}[PHASE 2]${NC} Implementing Executable Definition Module"

EXEC_CONFIG_PATH="${PROJECT_ROOT}/cmake/ExecutableConfig.cmake"
mkdir -p "${PROJECT_ROOT}/cmake"

cat > "${EXEC_CONFIG_PATH}" << 'EOF'
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

  # Get global version if not specified
  if(NOT EXEC_VERSION)
    get_property(NLINK_VERSION GLOBAL PROPERTY NLINK_VERSION)
    if(NLINK_VERSION)
      set(EXEC_VERSION "${NLINK_VERSION}")
    else()
      set(EXEC_VERSION "1.0.0")
    endif()
  endif()

  # Define executable target
  add_executable(${EXEC_NAME} ${EXEC_SOURCES})
  
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

  # Link with core components
  target_link_libraries(${EXEC_NAME} PRIVATE
    nlink_core_static
    nlink_cli_static
  )
  
  # Link with additional dependencies
  if(EXEC_DEPENDENCIES)
    target_link_libraries(${EXEC_NAME} PRIVATE
      ${EXEC_DEPENDENCIES}
    )
  endif()
  
  # Install executable if requested
  if(EXEC_INSTALL)
    install(TARGETS ${EXEC_NAME}
      RUNTIME DESTINATION bin
    )
    
    # Create versioned symlink if requested
    if(EXEC_VERSIONED)
      install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink 
        ${CMAKE_INSTALL_PREFIX}/bin/${EXEC_OUTPUT_NAME}
        ${CMAKE_INSTALL_PREFIX}/bin/${EXEC_OUTPUT_NAME}-${EXEC_VERSION}
      )")
    endif()
  endif()
  
  # Register executable globally
  set_property(GLOBAL PROPERTY NLINK_MAIN_EXECUTABLE ${EXEC_NAME})
  
  message(STATUS "Defined main executable: ${EXEC_NAME} (version ${EXEC_VERSION})")
endfunction()
EOF

echo -e "${GREEN}✓ ExecutableConfig.cmake module implemented${NC}"

# 3. Update main CMakeLists.txt to include ExecutableConfig module
echo -e "\n${YELLOW}[PHASE 3]${NC} Updating Module Integration"

MAIN_CMAKE_PATH="${PROJECT_ROOT}/CMakeLists.txt"
if [[ -f "${MAIN_CMAKE_PATH}" ]]; then
    cp "${MAIN_CMAKE_PATH}" "${MAIN_CMAKE_PATH}.bak"
    
    # Add include directive after BuildSystem.cmake if not already present
    if ! grep -q "ExecutableConfig.cmake" "${MAIN_CMAKE_PATH}"; then
        sed -i '/include.*BuildSystem.cmake/a include(${CMAKE_CURRENT_LIST_DIR}/cmake/ExecutableConfig.cmake)' "${MAIN_CMAKE_PATH}"
        echo -e "${GREEN}✓ Main CMakeLists.txt updated with ExecutableConfig module${NC}"
    else
        echo -e "${BLUE}ℹ ExecutableConfig.cmake already included in main CMakeLists.txt${NC}"
    fi
else
    echo -e "${RED}✗ Main CMakeLists.txt not found at ${MAIN_CMAKE_PATH}${NC}"
fi

echo -e "\n${GREEN}Build System Final Remediation Complete${NC}"
echo -e "${BLUE}────────────────────────────────────────────────────────────────────────────────${NC}"
echo -e "${YELLOW}Execution Protocol:${NC}"
echo -e "  1. ${BLUE}make clean${NC}            # Clear build artifacts"
echo -e "  2. ${BLUE}make${NC}                  # Rebuild with corrected system"
echo -e "${BLUE}────────────────────────────────────────────────────────────────────────────────${NC}"
