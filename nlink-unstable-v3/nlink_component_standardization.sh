#!/bin/bash
# nlink_component_standardization.sh

NLINK_ROOT="/home/obinexus/obinexus/projects/nlink/nlink-unstable-v3"
COMPONENTS=("tactic" "type" "token_type" "token_value")

for component in "${COMPONENTS[@]}"; do
  COMPONENT_DIR="${NLINK_ROOT}/src/core/${component}"
  
  # Ensure directory exists
  if [[ ! -d "${COMPONENT_DIR}" ]]; then
    mkdir -p "${COMPONENT_DIR}"
    echo "Created component directory: ${component}"
  fi
  
  # Create standardized CMakeLists.txt
  cat > "${COMPONENT_DIR}/CMakeLists.txt" << EOF
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

# Register with consistent target naming
add_dependencies(nlink_core_components nlink_component_\${MODULE_NAME})
EOF

  echo "Created standardized CMakeLists.txt for ${component}"
  
  # Create minimal implementation files if needed
  if [[ ! -f "${COMPONENT_DIR}/${component}.c" ]]; then
    touch "${COMPONENT_DIR}/${component}.c"
  fi
  
  if [[ ! -f "${COMPONENT_DIR}/${component}.h" ]]; then
    touch "${COMPONENT_DIR}/${component}.h"
  fi
done
