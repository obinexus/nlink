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
