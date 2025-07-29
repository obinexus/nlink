# ComponentSystem.cmake
# Core component management for NexusLink
#
# This module provides component initialization, registration, and dependency
# management functionality for the NexusLink system.

# Include guard
if(DEFINED NLINK_COMPONENT_SYSTEM_CMAKE_INCLUDED)
  return()
endif()
set(NLINK_COMPONENT_SYSTEM_CMAKE_INCLUDED TRUE)

include(CMakeParseArguments)

# Function to initialize the component system
function(nlink_init_component_system)
  cmake_parse_arguments(
    COMP_INIT
    ""
    "VERSION"
    "COMPONENTS"
    ${ARGN}
  )

  # Validate required arguments
  if(NOT COMP_INIT_COMPONENTS)
    message(WARNING "No components specified for nlink_init_component_system")
  endif()

  # Set default version if not provided
  if(NOT COMP_INIT_VERSION)
    set(COMP_INIT_VERSION "1.0.0")
  endif()

  # Register components
  foreach(COMPONENT ${COMP_INIT_COMPONENTS})
    # Create component directory if it doesn't exist
    nlink_ensure_directory("${NLINK_SRC_DIR}/core/${COMPONENT}")
    nlink_ensure_directory("${NLINK_INCLUDE_DIR}/nlink/core/${COMPONENT}")
    
    # Register component
    nlink_register_component(${COMPONENT} ${COMP_INIT_VERSION})
  endforeach()

  # Define component targets
  add_custom_target(nlink_core_components
    COMMENT "Building all NexusLink core components"
  )

  message(STATUS "Component system initialized with ${COMP_INIT_VERSION}")
  message(STATUS "Registered components: ${COMP_INIT_COMPONENTS}")
endfunction()

# Function to register a component
function(nlink_register_component COMPONENT VERSION)
  # Set component properties
  set_property(GLOBAL PROPERTY NLINK_COMPONENT_${COMPONENT}_VERSION ${VERSION})
  set_property(GLOBAL PROPERTY NLINK_COMPONENT_${COMPONENT}_REGISTERED TRUE)
  
  # Define component include directories
  set(COMPONENT_INCLUDE_DIR "${NLINK_INCLUDE_DIR}/nlink/core/${COMPONENT}")
  set(COMPONENT_SRC_DIR "${NLINK_SRC_DIR}/core/${COMPONENT}")
  
  # Create component library targets
  add_library(nlink_component_${COMPONENT} OBJECT
    "${COMPONENT_SRC_DIR}/${COMPONENT}_core.c"
    "${COMPONENT_SRC_DIR}/${COMPONENT}_context.c"
    "${COMPONENT_SRC_DIR}/nexus_${COMPONENT}.c"
  )
  
  # Set include directories for component
  target_include_directories(nlink_component_${COMPONENT} PUBLIC
    ${NLINK_INCLUDE_DIR}
    ${NLINK_SRC_DIR}
  )
  
  # Add component to core components target
  add_dependencies(nlink_core_components nlink_component_${COMPONENT})
  
  message(STATUS "Registered component: ${COMPONENT} (version ${VERSION})")
endfunction()

# Function to get component dependencies
function(nlink_get_component_dependencies COMPONENT OUTPUT_VAR)
  # Implementation to be added based on dependency specifications
  set(${OUTPUT_VAR} "" PARENT_SCOPE)
endfunction()
