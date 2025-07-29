# ExecutableConfig.cmake
# Main executable configuration for NexusLink

# Include guard
if(DEFINED NLINK_EXECUTABLE_CONFIG_CMAKE_INCLUDED)
  return()
endif()
set(NLINK_EXECUTABLE_CONFIG_CMAKE_INCLUDED TRUE)

include(CMakeParseArguments)

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
  endif()
  
  # Register executable globally
  set_property(GLOBAL PROPERTY NLINK_MAIN_EXECUTABLE ${EXEC_NAME})
  
  message(STATUS "Defined main executable: ${EXEC_NAME} (version ${EXEC_VERSION})")
endfunction()