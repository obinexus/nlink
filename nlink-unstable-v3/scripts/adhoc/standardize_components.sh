#!/bin/bash
# standardize_components.sh
# Resolves component directory structure issues

NLINK_ROOT="/home/obinexus/obinexus/projects/nlink/nlink-unstable-v3"
COMPONENTS=(
  "common" "config" "metadata" "minimizer" "mpsystem" "parser" 
  "pattern" "pattern_matching" "pipeline" "schema" "spsystem" 
  "symbols" "tactic" "tatit" "token_type" "token_value" 
  "tokenizer" "type" "versioning"
)

for component in "${COMPONENTS[@]}"; do
  COMPONENT_DIR="${NLINK_ROOT}/src/core/${component}"
  
  # Ensure directory exists
  mkdir -p "${COMPONENT_DIR}"
  
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

# Register with core components target
add_dependencies(nlink_core_components nlink_component_\${MODULE_NAME})
EOF

  echo "Standardized ${component} component"
  
  # Create minimal implementation files
  if [[ ! -f "${COMPONENT_DIR}/${component}.c" ]]; then
    cat > "${COMPONENT_DIR}/${component}.c" << EOF
/**
 * @file ${component}.c
 * @brief Implementation of the NexusLink ${component} subsystem
 * @copyright OBINexus Computing, 2025
 */

#include "${component}.h"
#include <stddef.h>

/**
 * @brief Initialize the ${component} subsystem
 * @return Status code (0 = success)
 */
int ${component}_initialize(void) {
    // Implementation placeholder
    return 0;
}

/**
 * @brief Shutdown the ${component} subsystem
 * @return Status code (0 = success)
 */
int ${component}_shutdown(void) {
    // Implementation placeholder
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

/**
 * @brief Initialize the ${component} subsystem
 * @return Status code (0 = success)
 */
int ${component}_initialize(void);

/**
 * @brief Shutdown the ${component} subsystem
 * @return Status code (0 = success)
 */
int ${component}_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_${component^^}_H */
EOF
  fi
done

echo "Component standardization complete."
