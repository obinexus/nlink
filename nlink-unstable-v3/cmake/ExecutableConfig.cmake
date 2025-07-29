# ExecutableConfig.cmake implementation
# ExecutableConfig.cmake
# OBINexus Component Boundary Definition Protocol
if(DEFINED NLINK_EXECUTABLE_CONFIG_INCLUDED)
  return()
endif()
set(NLINK_EXECUTABLE_CONFIG_INCLUDED TRUE)

include(CMakeParseArguments)
# ExecutableConfig.cmake safety wrapper
macro(nlink_define_main_executable)
    # Implement existence check before creation
    if(NOT TARGET nlink_executable)
        add_executable(nlink_executable ${NLINK_MAIN_SOURCES})
        set_target_properties(nlink_executable PROPERTIES
            OUTPUT_NAME "nlink"
            RUNTIME_OUTPUT_DIRECTORY "${NLINK_BIN_DIR}"
        )
    else()
        message(WARNING "nlink_executable target already exists - skipping redefinition")
    endif()
endmacro()

function(nlink_define_main_executable)
  cmake_parse_arguments(
    EXEC
    "STATIC_LINKING;ENABLE_PROFILING"
    "NAME;VERSION;OUTPUT_DIR"
    "SOURCES;INCLUDES;DEPENDENCIES"
    ${ARGN}
  )

  # Validate boundary integrity
  if(NOT EXEC_NAME)
    message(FATAL_ERROR "NAME parameter required for component boundary definition")
  endif()

  # Create executable with proper interface isolation
  add_executable(${EXEC_NAME} ${EXEC_SOURCES})
  
  # Interface encapsulation layer
  target_include_directories(${EXEC_NAME} PRIVATE
    ${NLINK_INCLUDE_DIR}
    ${EXEC_INCLUDES}
  )

  # Dependency materialization through proper linkage
  if(EXEC_STATIC_LINKING)
    # Static linking with appropriate interface exposure
    target_link_libraries(${EXEC_NAME} PRIVATE
      -static
      ${EXEC_DEPENDENCIES}
    )
    target_compile_definitions(${EXEC_NAME} PRIVATE NLINK_STATIC_BUILD)
  else()
    # Dynamic linking through well-defined boundary
    target_link_libraries(${EXEC_NAME} PRIVATE
      ${EXEC_DEPENDENCIES}
    )
  endif()

  # Register with component registry for verification
  set_property(GLOBAL APPEND PROPERTY NLINK_EXECUTABLES ${EXEC_NAME})
  
  message(STATUS "Component boundary established: ${EXEC_NAME}")
endfunction()
