# ExecutableConfig.cmake implementation
# ExecutableConfig.cmake
# OBINexus Component Boundary Definition Protocol
if(DEFINED NLINK_EXECUTABLE_CONFIG_INCLUDED)
  return()
endif()
set(NLINK_EXECUTABLE_CONFIG_INCLUDED TRUE)
# ExecutableConfig.cmake - ENHANCED VERSION
include_guard(GLOBAL)  # Prevent multiple inclusions

# Global flag to track executable creation
set(NLINK_EXECUTABLE_CREATED FALSE CACHE INTERNAL "Track executable creation")

function(nlink_define_main_executable)
    if(NLINK_EXECUTABLE_CREATED)
        message(STATUS "nlink_executable already defined - skipping")
        return()
    endif()
    
    # Extract main sources
    file(GLOB NLINK_MAIN_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")
    
    # Create executable with proper guards
    add_executable(nlink_executable ${NLINK_MAIN_SOURCES})
    
    # Link against core and CLI libraries
    target_link_libraries(nlink_executable
        PRIVATE
            nlink_core
            $<$<BOOL:${NLINK_USE_CLI}>:nlink_cli_static>
            Threads::Threads
    )
    
    # Set output properties
    set_target_properties(nlink_executable PROPERTIES
        OUTPUT_NAME "nlink"
        RUNTIME_OUTPUT_DIRECTORY "${NLINK_BIN_ROOT}"
        INSTALL_RPATH "$ORIGIN/../lib"
    )
    
    # Mark as created
    set(NLINK_EXECUTABLE_CREATED TRUE CACHE INTERNAL "Track executable creation")
endfunction()
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
