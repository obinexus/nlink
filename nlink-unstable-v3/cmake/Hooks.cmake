# Hooks.cmake
# Build process hook system for NexusLink
#
# This module provides a flexible hook system for registering and executing
# pre-build and post-build actions during the NexusLink build process.

if(DEFINED NLINK_HOOKS_INCLUDED)
  return()
endif()
set(NLINK_HOOKS_INCLUDED TRUE)

include(CMakeParseArguments)

# Initialize global hook registries
set_property(GLOBAL PROPERTY NLINK_PRE_BUILD_HOOKS "")
set_property(GLOBAL PROPERTY NLINK_POST_BUILD_HOOKS "")
set_property(GLOBAL PROPERTY NLINK_PRE_COMPONENT_HOOKS "")
set_property(GLOBAL PROPERTY NLINK_POST_COMPONENT_HOOKS "")
set_property(GLOBAL PROPERTY NLINK_PRE_TEST_HOOKS "")
set_property(GLOBAL PROPERTY NLINK_POST_TEST_HOOKS "")

# Initialize hook system with required directories
function(nlink_init_hook_system)
  cmake_parse_arguments(
    HOOK_INIT
    "VERBOSE"
    "OBJ_DIR;BIN_DIR;LIB_DIR;TEST_DIR;LOG_DIR"
    ""
    ${ARGN}
  )
  
  # Set default directories if not provided
  if(NOT HOOK_INIT_OBJ_DIR)
    set(HOOK_INIT_OBJ_DIR "${CMAKE_BINARY_DIR}/obj")
  endif()
  
  if(NOT HOOK_INIT_BIN_DIR)
    set(HOOK_INIT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
  endif()
  
  if(NOT HOOK_INIT_LIB_DIR)
    set(HOOK_INIT_LIB_DIR "${CMAKE_BINARY_DIR}/lib")
  endif()
  
  if(NOT HOOK_INIT_TEST_DIR)
    set(HOOK_INIT_TEST_DIR "${CMAKE_BINARY_DIR}/tests")
  endif()
  
  if(NOT HOOK_INIT_LOG_DIR)
    set(HOOK_INIT_LOG_DIR "${CMAKE_BINARY_DIR}/logs")
  endif()
  
  # Create required directories
  file(MAKE_DIRECTORY ${HOOK_INIT_OBJ_DIR})
  file(MAKE_DIRECTORY ${HOOK_INIT_BIN_DIR})
  file(MAKE_DIRECTORY ${HOOK_INIT_LIB_DIR})
  file(MAKE_DIRECTORY ${HOOK_INIT_TEST_DIR})
  file(MAKE_DIRECTORY ${HOOK_INIT_LOG_DIR})
  
  # Create component object directories for all registered components
  get_property(COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
  if(COMPONENTS)
    foreach(COMPONENT ${COMPONENTS})
      file(MAKE_DIRECTORY "${HOOK_INIT_OBJ_DIR}/core/${COMPONENT}")
    endforeach()
  else()
    # Create default directories if no components are registered yet
    file(MAKE_DIRECTORY "${HOOK_INIT_OBJ_DIR}/core")
    file(MAKE_DIRECTORY "${HOOK_INIT_OBJ_DIR}/cli")
  endif()
  
  # Store directories as global properties
  set_property(GLOBAL PROPERTY NLINK_OBJ_DIR ${HOOK_INIT_OBJ_DIR})
  set_property(GLOBAL PROPERTY NLINK_BIN_DIR ${HOOK_INIT_BIN_DIR})
  set_property(GLOBAL PROPERTY NLINK_LIB_DIR ${HOOK_INIT_LIB_DIR})
  set_property(GLOBAL PROPERTY NLINK_TEST_DIR ${HOOK_INIT_TEST_DIR})
  set_property(GLOBAL PROPERTY NLINK_LOG_DIR ${HOOK_INIT_LOG_DIR})
  
  # Register standard pre-build hooks
  nlink_register_pre_build_hook(nlink_create_component_directories)
  nlink_register_pre_build_hook(nlink_validate_includes)
  
  # Register standard post-build hooks
  nlink_register_post_build_hook(nlink_consolidate_object_files)
  
  # Create timestamp for log files
  string(TIMESTAMP HOOK_TIMESTAMP "%Y%m%d_%H%M%S")
  set_property(GLOBAL PROPERTY NLINK_BUILD_TIMESTAMP ${HOOK_TIMESTAMP})
  
  if(HOOK_INIT_VERBOSE)
    message(STATUS "Hook system initialized with directories:")
    message(STATUS "  OBJ_DIR: ${HOOK_INIT_OBJ_DIR}")
    message(STATUS "  BIN_DIR: ${HOOK_INIT_BIN_DIR}")
    message(STATUS "  LIB_DIR: ${HOOK_INIT_LIB_DIR}")
    message(STATUS "  TEST_DIR: ${HOOK_INIT_TEST_DIR}")
    message(STATUS "  LOG_DIR: ${HOOK_INIT_LOG_DIR}")
  endif()
endfunction()

# Register a pre-build hook
function(nlink_register_pre_build_hook HOOK_FUNCTION)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_BUILD_HOOKS)
  list(APPEND HOOKS ${HOOK_FUNCTION})
  set_property(GLOBAL PROPERTY NLINK_PRE_BUILD_HOOKS "${HOOKS}")
endfunction()

# Register a post-build hook
function(nlink_register_post_build_hook HOOK_FUNCTION)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_BUILD_HOOKS)
  list(APPEND HOOKS ${HOOK_FUNCTION})
  set_property(GLOBAL PROPERTY NLINK_POST_BUILD_HOOKS "${HOOKS}")
endfunction()

# Register a pre-component build hook
function(nlink_register_pre_component_hook HOOK_FUNCTION COMPONENT)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_COMPONENT_HOOKS)
  list(APPEND HOOKS "${COMPONENT}:${HOOK_FUNCTION}")
  set_property(GLOBAL PROPERTY NLINK_PRE_COMPONENT_HOOKS "${HOOKS}")
endfunction()

# Register a post-component build hook
function(nlink_register_post_component_hook HOOK_FUNCTION COMPONENT)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_COMPONENT_HOOKS)
  list(APPEND HOOKS "${COMPONENT}:${HOOK_FUNCTION}")
  set_property(GLOBAL PROPERTY NLINK_POST_COMPONENT_HOOKS "${HOOKS}")
endfunction()

# Register a pre-test hook
function(nlink_register_pre_test_hook HOOK_FUNCTION)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_TEST_HOOKS)
  list(APPEND HOOKS ${HOOK_FUNCTION})
  set_property(GLOBAL PROPERTY NLINK_PRE_TEST_HOOKS "${HOOKS}")
endfunction()

# Register a post-test hook
function(nlink_register_post_test_hook HOOK_FUNCTION)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_TEST_HOOKS)
  list(APPEND HOOKS ${HOOK_FUNCTION})
  set_property(GLOBAL PROPERTY NLINK_POST_TEST_HOOKS "${HOOKS}")
endfunction()

# Execute all registered pre-build hooks
function(nlink_execute_pre_build_hooks)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_BUILD_HOOKS)
  foreach(HOOK ${HOOKS})
    message(STATUS "Executing pre-build hook: ${HOOK}")
    if(COMMAND ${HOOK})
      cmake_language(CALL ${HOOK})
    else()
      message(WARNING "Pre-build hook not found: ${HOOK}")
    endif()
  endforeach()
endfunction()

# Execute all registered post-build hooks
function(nlink_execute_post_build_hooks)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_BUILD_HOOKS)
  foreach(HOOK ${HOOKS})
    message(STATUS "Executing post-build hook: ${HOOK}")
    if(COMMAND ${HOOK})
      cmake_language(CALL ${HOOK})
    else()
      message(WARNING "Post-build hook not found: ${HOOK}")
    endif()
  endforeach()
endfunction()

# Execute pre-component hooks for a specific component
function(nlink_execute_pre_component_hooks COMPONENT)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_COMPONENT_HOOKS)
  foreach(HOOK_ENTRY ${HOOKS})
    # Extract component and hook function from entry
    string(REPLACE ":" ";" HOOK_PARTS ${HOOK_ENTRY})
    list(GET HOOK_PARTS 0 HOOK_COMPONENT)
    list(GET HOOK_PARTS 1 HOOK)
    
    # Execute hook if it matches the component
    if(HOOK_COMPONENT STREQUAL COMPONENT OR HOOK_COMPONENT STREQUAL "ALL")
      message(STATUS "Executing pre-component hook for ${COMPONENT}: ${HOOK}")
      if(COMMAND ${HOOK})
        cmake_language(CALL ${HOOK} ${COMPONENT})
      else()
        message(WARNING "Pre-component hook not found: ${HOOK}")
      endif()
    endif()
  endforeach()
endfunction()

# Execute post-component hooks for a specific component
function(nlink_execute_post_component_hooks COMPONENT)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_COMPONENT_HOOKS)
  foreach(HOOK_ENTRY ${HOOKS})
    # Extract component and hook function from entry
    string(REPLACE ":" ";" HOOK_PARTS ${HOOK_ENTRY})
    list(GET HOOK_PARTS 0 HOOK_COMPONENT)
    list(GET HOOK_PARTS 1 HOOK)
    
    # Execute hook if it matches the component
    if(HOOK_COMPONENT STREQUAL COMPONENT OR HOOK_COMPONENT STREQUAL "ALL")
      message(STATUS "Executing post-component hook for ${COMPONENT}: ${HOOK}")
      if(COMMAND ${HOOK})
        cmake_language(CALL ${HOOK} ${COMPONENT})
      else()
        message(WARNING "Post-component hook not found: ${HOOK}")
      endif()
    endif()
  endforeach()
endfunction()

# Execute all registered pre-test hooks
function(nlink_execute_pre_test_hooks)
  get_property(HOOKS GLOBAL PROPERTY NLINK_PRE_TEST_HOOKS)
  foreach(HOOK ${HOOKS})
    message(STATUS "Executing pre-test hook: ${HOOK}")
    if(COMMAND ${HOOK})
      cmake_language(CALL ${HOOK})
    else()
      message(WARNING "Pre-test hook not found: ${HOOK}")
    endif()
  endforeach()
endfunction()

# Execute all registered post-test hooks
function(nlink_execute_post_test_hooks)
  get_property(HOOKS GLOBAL PROPERTY NLINK_POST_TEST_HOOKS)
  foreach(HOOK ${HOOKS})
    message(STATUS "Executing post-test hook: ${HOOK}")
    if(COMMAND ${HOOK})
      cmake_language(CALL ${HOOK})
    else()
      message(WARNING "Post-test hook not found: ${HOOK}")
    endif()
  endforeach()
endfunction()

# Standard hook: Create component directories
function(nlink_create_component_directories)
  get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
  
  # Create core component directories
  get_property(COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
  if(COMPONENTS)
    foreach(COMPONENT ${COMPONENTS})
      file(MAKE_DIRECTORY "${OBJ_DIR}/core/${COMPONENT}")
    endforeach()
  else()
    # Handle case when components aren't registered yet
    file(GLOB CORE_DIRS "${CMAKE_SOURCE_DIR}/src/core/*")
    foreach(DIR ${CORE_DIRS})
      if(IS_DIRECTORY ${DIR})
        get_filename_component(COMPONENT_NAME ${DIR} NAME)
        file(MAKE_DIRECTORY "${OBJ_DIR}/core/${COMPONENT_NAME}")
      endif()
    endforeach()
  endif()
  
  # Create CLI component directories
  file(GLOB CLI_DIRS "${CMAKE_SOURCE_DIR}/src/cli/*")
  foreach(DIR ${CLI_DIRS})
    if(IS_DIRECTORY ${DIR})
      get_filename_component(CLI_COMPONENT_NAME ${DIR} NAME)
      file(MAKE_DIRECTORY "${OBJ_DIR}/cli/${CLI_COMPONENT_NAME}")
    endif()
  endforeach()
endfunction()

# Standard hook: Consolidate object files
function(nlink_consolidate_object_files)
  get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
  get_property(LIB_DIR GLOBAL PROPERTY NLINK_LIB_DIR)
  
  # Create directory for consolidated objects
  file(MAKE_DIRECTORY "${OBJ_DIR}/consolidated")
  
  # Copy all object files to consolidated directory
  file(GLOB_RECURSE ALL_OBJECTS "${OBJ_DIR}/**/*.o")
  foreach(OBJ ${ALL_OBJECTS})
    get_filename_component(OBJ_NAME ${OBJ} NAME)
    file(COPY ${OBJ} DESTINATION "${OBJ_DIR}/consolidated")
  endforeach()
  
  message(STATUS "Consolidated object files to ${OBJ_DIR}/consolidated")
endfunction()

# Standard hook: Create component shared library
function(nlink_create_component_shared_library COMPONENT)
  get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
  get_property(LIB_DIR GLOBAL PROPERTY NLINK_LIB_DIR)
  
  # Get all object files for this component
  file(GLOB COMPONENT_OBJECTS "${OBJ_DIR}/core/${COMPONENT}/*.o")
  
  if(COMPONENT_OBJECTS)
    # Define the shared library target if it doesn't exist
    if(NOT TARGET nlink_${COMPONENT}_shared)
      add_library(nlink_${COMPONENT}_shared SHARED IMPORTED)
      set_target_properties(nlink_${COMPONENT}_shared PROPERTIES
        IMPORTED_LOCATION "${LIB_DIR}/libnlink_${COMPONENT}.so"
      )
    endif()
    
    # Use external command to create shared library
    add_custom_command(
      OUTPUT "${LIB_DIR}/libnlink_${COMPONENT}.so"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DIR}
      COMMAND ${CMAKE_C_COMPILER} -shared -o "${LIB_DIR}/libnlink_${COMPONENT}.so" ${COMPONENT_OBJECTS}
      DEPENDS ${COMPONENT_OBJECTS}
      COMMENT "Creating shared library for ${COMPONENT}"
    )
    
    # Add custom target to trigger the shared library build
    add_custom_target(build_${COMPONENT}_shared
      DEPENDS "${LIB_DIR}/libnlink_${COMPONENT}.so"
    )
    
    message(STATUS "Registered shared library target for ${COMPONENT}")
  else()
    message(WARNING "No object files found for component ${COMPONENT}")
  endif()
endfunction()

# Standard hook: Validate includes
function(nlink_validate_includes)
  # This is a placeholder that would call the include validation functionality
  message(STATUS "Validating includes (placeholder)")
  
  # Run validation script if it exists
  set(VALIDATION_SCRIPT "${NLINK_PROJECT_ROOT}/scripts/standardize_nlink_includes.py")
  if(EXISTS "${VALIDATION_SCRIPT}")
    execute_process(
      COMMAND python3 ${VALIDATION_SCRIPT} --project-root ${NLINK_PROJECT_ROOT} --validate
      RESULT_VARIABLE RESULT
      OUTPUT_VARIABLE OUTPUT
      ERROR_VARIABLE ERROR
    )
    
    if(RESULT EQUAL 0)
      message(STATUS "Include validation passed")
    else()
      message(WARNING "Include validation found issues:\n${OUTPUT}\n${ERROR}")
    endif()
  endif()
endfunction()

# Define pre-hook for object directory setup
function(nlink_setup_component_obj_dir COMPONENT)
  get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
  
  # Create component object directory
  file(MAKE_DIRECTORY "${OBJ_DIR}/core/${COMPONENT}")
  
  # Create subdirectories for this component if needed
  file(GLOB SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/*")
  foreach(DIR ${SUBDIRS})
    if(IS_DIRECTORY ${DIR})
      get_filename_component(SUBDIR_NAME ${DIR} NAME)
      file(MAKE_DIRECTORY "${OBJ_DIR}/core/${COMPONENT}/${SUBDIR_NAME}")
    endif()
  endforeach()
  
  message(STATUS "Set up object directory for ${COMPONENT}: ${OBJ_DIR}/core/${COMPONENT}")
endfunction()
