#!/bin/bash
# path_resolution_protocol.sh
# Path vulnerability remediation for NexusLink build system
# Copyright © 2025 OBINexus Computing

set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Critical BuildSystem.cmake path correction
BUILD_SYSTEM_PATH="${PROJECT_ROOT}/cmake/BuildSystem.cmake"
if [[ -f "${BUILD_SYSTEM_PATH}" ]]; then
    cp "${BUILD_SYSTEM_PATH}" "${BUILD_SYSTEM_PATH}.bak"
    
    # Correct path resolution vulnerability
    sed -i 's|include(${CMAKE_CURRENT_LIST_DIR}/cmake/ExecutableConfig.cmake)|include(${CMAKE_CURRENT_LIST_DIR}/ExecutableConfig.cmake)|g' "${BUILD_SYSTEM_PATH}"
    echo "✓ BuildSystem.cmake path vulnerability remediated"
else
    echo "✗ BuildSystem.cmake not found at ${BUILD_SYSTEM_PATH}"
    exit 1
fi

# ExecutableConfig.cmake deduplication
EXEC_CONFIG_PATH="${PROJECT_ROOT}/cmake/ExecutableConfig.cmake"
if [[ -f "${EXEC_CONFIG_PATH}" ]]; then
    cp "${EXEC_CONFIG_PATH}" "${EXEC_CONFIG_PATH}.bak"
    
    # Filter duplicate include guards and function definitions
    awk '
    BEGIN { printing = 1; guard_found = 0; }
    /if\(DEFINED NLINK_EXECUTABLE_CONFIG_CMAKE_INCLUDED\)/ {
        if (guard_found) { printing = 0; }
        else { guard_found = 1; }
    }
    { if (printing) print $0; }
    /endfunction\(\)/ { printing = 1; }
    ' "${EXEC_CONFIG_PATH}.bak" > "${EXEC_CONFIG_PATH}"
    
    echo "✓ ExecutableConfig.cmake duplicate declarations removed"
else
    echo "✗ ExecutableConfig.cmake not found at ${EXEC_CONFIG_PATH}"
    exit 1
fi

# Component build directory filtration implementation
CORE_CMAKE_PATH="${PROJECT_ROOT}/src/core/CMakeLists.txt"
if [[ -f "${CORE_CMAKE_PATH}" ]]; then
    cp "${CORE_CMAKE_PATH}" "${CORE_CMAKE_PATH}.bak"
    
    # Implement build directory filtration logic
    cat > "${CORE_CMAKE_PATH}" << 'EOF'
# Core components CMakeLists.txt for NexusLink
# Copyright © 2025 OBINexus Computing

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# Define core components
set(NLINK_CORE_COMPONENTS "")
file(GLOB COMPONENT_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")

# Extract valid component directories (filter out CMakeFiles and build directories)
foreach(DIR ${COMPONENT_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}" AND
       NOT "${DIR}" STREQUAL "CMakeFiles" AND
       NOT "${DIR}" STREQUAL "cmake" AND
       NOT "${DIR}" MATCHES "^\\." AND
       NOT "${DIR}" MATCHES "_build$")
        
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
    
    echo "✓ Core CMakeLists.txt component filtration implemented"
else
    echo "✗ Core CMakeLists.txt not found at ${CORE_CMAKE_PATH}"
    exit 1
fi

echo "Path Resolution Protocol completed successfully"
