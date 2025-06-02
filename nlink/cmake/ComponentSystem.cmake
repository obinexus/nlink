# ComponentSystem.cmake patch for NexusLink
# Resolves target registration conflicts by checking if components are already registered
# Copyright © 2025 OBINexus Computing

# This function should replace the original nlink_register_component function
function(nlink_register_component COMPONENT_NAME)
  # Ensure the component name is valid
  if(NOT COMPONENT_NAME MATCHES "^[a-z][a-z0-9_]*$")
    message(FATAL_ERROR "Invalid component name: ${COMPONENT_NAME}")
  endif()
  
  # Check if the component is already registered to prevent duplication
  get_property(REGISTERED_COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
  if(REGISTERED_COMPONENTS)
    list(FIND REGISTERED_COMPONENTS ${COMPONENT_NAME} COMPONENT_INDEX)
    if(NOT COMPONENT_INDEX EQUAL -1)
      message(STATUS "Component ${COMPONENT_NAME} already registered, skipping.")
      return()
    endif()
  endif()
  
  # Ensure component source directory exists
  set(COMPONENT_SRC_DIR "${NLINK_SRC_DIR}/core/${COMPONENT_NAME}")
  if(NOT EXISTS "${COMPONENT_SRC_DIR}")
    message(WARNING "Component source directory not found: ${COMPONENT_SRC_DIR}")
    return()
  endif()
  
  # Create component target
  string(TOUPPER "${COMPONENT_NAME}" COMPONENT_NAME_UPPER)
  set(TARGET_NAME "nlink_${COMPONENT_NAME}")
  
  # Add component to global registry
  set_property(GLOBAL APPEND PROPERTY NLINK_REGISTERED_COMPONENTS ${COMPONENT_NAME})
  
  # Check if target already exists (redundant with above check, but added for safety)
  if(TARGET ${TARGET_NAME})
    message(STATUS "Target ${TARGET_NAME} already exists, skipping creation.")
    return()
  endif()
  
  # Define component build target
  add_custom_target(${TARGET_NAME}
    COMMENT "Building ${COMPONENT_NAME} component"
  )
  
  # Make core_components depend on this component
  add_dependencies(nlink_core_components ${TARGET_NAME})
  
  # Find all source files for this component
  file(GLOB COMPONENT_SOURCES "${COMPONENT_SRC_DIR}/*.c")
  
  # Inform about the component
  message(STATUS "Registered component: ${COMPONENT_NAME} (${TARGET_NAME})")
  message(STATUS "  Source files: ${COMPONENT_SOURCES}")
  
  # Set component properties
  set_property(TARGET ${TARGET_NAME} PROPERTY COMPONENT_NAME ${COMPONENT_NAME})
  set_property(TARGET ${TARGET_NAME} PROPERTY COMPONENT_SOURCES ${COMPONENT_SOURCES})
  set_property(TARGET ${TARGET_NAME} PROPERTY COMPONENT_INCLUDE_DIR "${NLINK_INCLUDE_DIR}/nlink/core/${COMPONENT_NAME}")
  
  # Execute post-registration hooks if defined
  if(COMMAND nlink_post_register_component_hook)
    nlink_post_register_component_hook(${COMPONENT_NAME})
  endif()
endfunction()