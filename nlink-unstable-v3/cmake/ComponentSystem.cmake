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
# Component registration fix in ComponentSystem.cmake
macro(nlink_register_component comp_name comp_version)
    # Critical: Properly expand the component name variable
    set(_COMP_NAME "${comp_name}")
    if(NOT _COMP_NAME OR _COMP_NAME STREQUAL "NAME")
        message(FATAL_ERROR "Invalid component name: ${comp_name}")
    endif()
    
    set(NLINK_COMPONENT_${_COMP_NAME}_VERSION ${comp_version})
    list(APPEND NLINK_REGISTERED_COMPONENTS ${_COMP_NAME})
    
    message(STATUS "Registered component: ${_COMP_NAME} (version ${comp_version})")
endmacro()
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
function(nlink_register_component COMPONENT)
  # Parse additional arguments
  cmake_parse_arguments(
    COMP_REG
    "REQUIRED;OPTIONAL"
    "VERSION;DESCRIPTION;AUTHOR"
    "DEPENDENCIES;SOURCES;INCLUDES"
    ${ARGN}
  )

  # Provide default values for optional parameters
  if(NOT COMP_REG_VERSION)
    set(COMP_REG_VERSION "1.0.0")
  endif()

  # Register component in global property
  set_property(GLOBAL APPEND PROPERTY NLINK_REGISTERED_COMPONENTS ${COMPONENT})
  set_property(GLOBAL PROPERTY NLINK_COMPONENT_${COMPONENT}_VERSION ${COMP_REG_VERSION})
  
  message(STATUS "Registered component: ${COMPONENT} (version ${COMP_REG_VERSION})")
endfunction()

# Function to get component dependencies
function(nlink_get_component_dependencies COMPONENT OUTPUT_VAR)
  # Implementation to be added based on dependency specifications
  set(${OUTPUT_VAR} "" PARENT_SCOPE)
endfunction()

# Function to build a component with enhanced parameter handling
function(nlink_build_component)
  cmake_parse_arguments(
    BUILD_COMP
    "SHARED;STATIC;MODULE;VERBOSE"
    "COMPONENT;NAME;OUTPUT_NAME;VERSION;TYPE"
    "SOURCES;HEADERS;INCLUDES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS"
    ${ARGN}
  )

  # Parameter normalization protocol: NAME → COMPONENT (backwards compatibility)
  if(BUILD_COMP_NAME AND NOT BUILD_COMP_COMPONENT)
    set(BUILD_COMP_COMPONENT ${BUILD_COMP_NAME})
    if(BUILD_COMP_VERBOSE)
      message(STATUS "Component parameter normalization: NAME → COMPONENT (${BUILD_COMP_NAME})")
    endif()
  endif()

  # Validate required arguments
  if(NOT BUILD_COMP_COMPONENT)
    message(FATAL_ERROR "COMPONENT parameter is required for nlink_build_component")
  endif()

  # Configure unique binary directory to prevent collisions
  set(COMPONENT_NAME ${BUILD_COMP_COMPONENT})
  set(COMPONENT_TARGET nlink_component_${COMPONENT_NAME})
  set(UNIQUE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT_NAME}_build")
  
  # Set default library type if not specified
  set(LIB_TYPE "STATIC")
  if(BUILD_COMP_SHARED)
    set(LIB_TYPE "SHARED")
  elseif(BUILD_COMP_MODULE)
    set(LIB_TYPE "MODULE")
  endif()
  
  # Set output name if provided
  if(BUILD_COMP_OUTPUT_NAME)
    set(OUTPUT_NAME ${BUILD_COMP_OUTPUT_NAME})
  else()
    set(OUTPUT_NAME "nlink_${COMPONENT_NAME}")
  endif()

  # Set default sources if not provided
  if(NOT BUILD_COMP_SOURCES)
    file(GLOB BUILD_COMP_SOURCES 
      "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )
  endif()

  # Build the component library
  add_library(${COMPONENT_TARGET} ${LIB_TYPE} ${BUILD_COMP_SOURCES})
  
  # Set target properties with proper argument validation
  if(BUILD_COMP_VERSION)
    set_target_properties(${COMPONENT_TARGET} PROPERTIES
      OUTPUT_NAME ${OUTPUT_NAME}
      VERSION ${BUILD_COMP_VERSION}
    )
  else()
    set_target_properties(${COMPONENT_TARGET} PROPERTIES
      OUTPUT_NAME ${OUTPUT_NAME}
    )
  endif()

  # Add include directories
  if(BUILD_COMP_INCLUDES)
    target_include_directories(${COMPONENT_TARGET} PUBLIC
      ${NLINK_INCLUDE_DIR}
      ${BUILD_COMP_INCLUDES}
    )
  else()
    target_include_directories(${COMPONENT_TARGET} PUBLIC
      ${NLINK_INCLUDE_DIR}
    )
  endif()

  # Add compile definitions
  if(BUILD_COMP_COMPILE_DEFINITIONS)
    target_compile_definitions(${COMPONENT_TARGET} PRIVATE
      ${BUILD_COMP_COMPILE_DEFINITIONS}
    )
  endif()

  # Add compile options
  if(BUILD_COMP_COMPILE_OPTIONS)
    target_compile_options(${COMPONENT_TARGET} PRIVATE
      ${BUILD_COMP_COMPILE_OPTIONS}
    )
  endif()

  # Link dependencies
  if(BUILD_COMP_DEPENDENCIES)
    target_link_libraries(${COMPONENT_TARGET} PRIVATE
      ${BUILD_COMP_DEPENDENCIES}
    )
  endif()

  # Add to components target
  add_dependencies(nlink_core_components ${COMPONENT_TARGET})
  
  message(STATUS "Built component: ${COMPONENT_NAME} (${LIB_TYPE})")
endfunction()
