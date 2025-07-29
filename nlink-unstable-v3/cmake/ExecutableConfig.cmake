# ExecutableConfig.cmake
# Core executable configuration for NexusLink build orchestration
# 
# This module provides standardized executable definition and linking
# for the NexusLink toolchain, ensuring proper progression through the
# riftlang.exe → .so.a → rift.exe → gosilang pathway.

if(DEFINED NLINK_EXECUTABLE_CONFIG_INCLUDED)
  return()
endif()
set(NLINK_EXECUTABLE_CONFIG_INCLUDED TRUE)

include(CMakeParseArguments)

# Main executable definition function with standardized architecture
function(nlink_define_main_executable)
  cmake_parse_arguments(
    EXEC
    "ENABLE_OPTIMIZATIONS;STATIC_LINKING;ENABLE_PROFILING"
    "NAME;VERSION;OUTPUT_DIR"
    "SOURCES;INCLUDES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS"
    ${ARGN}
  )

  # Validate required parameters
  if(NOT EXEC_NAME)
    message(FATAL_ERROR "NAME parameter is required for nlink_define_main_executable")
  endif()

  # Set default values for optional parameters
  if(NOT EXEC_VERSION)
    set(EXEC_VERSION ${PROJECT_VERSION})
  endif()

  if(NOT EXEC_OUTPUT_DIR)
    set(EXEC_OUTPUT_DIR "${NLINK_BIN_DIR}")
  endif()

  # Create executable target
  add_executable(${EXEC_NAME} ${EXEC_SOURCES})

  # Configure include directories
  target_include_directories(${EXEC_NAME} PRIVATE
    ${NLINK_INCLUDE_DIR}
    ${EXEC_INCLUDES}
  )

  # Add compile definitions
  if(EXEC_COMPILE_DEFINITIONS)
    target_compile_definitions(${EXEC_NAME} PRIVATE ${EXEC_COMPILE_DEFINITIONS})
  endif()

  # Add compile options
  if(EXEC_COMPILE_OPTIONS)
    target_compile_options(${EXEC_NAME} PRIVATE ${EXEC_COMPILE_OPTIONS})
  endif()

  # Configure linking
  if(EXEC_STATIC_LINKING)
    # Static linking configuration
    target_link_libraries(${EXEC_NAME} PRIVATE
      -static
      ${EXEC_DEPENDENCIES}
    )
    
    # Add compile definition for static build
    target_compile_definitions(${EXEC_NAME} PRIVATE NLINK_STATIC_BUILD)
  else()
    # Dynamic linking configuration
    target_link_libraries(${EXEC_NAME} PRIVATE
      ${EXEC_DEPENDENCIES}
    )
  endif()

  # Enable optimizations if requested
  if(EXEC_ENABLE_OPTIMIZATIONS)
    target_compile_options(${EXEC_NAME} PRIVATE
      -O2
      -ffast-math
      -ftree-vectorize
    )
  endif()

  # Enable profiling if requested
  if(EXEC_ENABLE_PROFILING)
    target_compile_options(${EXEC_NAME} PRIVATE
      -pg
      -fno-omit-frame-pointer
    )
    
    target_link_options(${EXEC_NAME} PRIVATE
      -pg
    )
  endif()

  # Set output directory
  set_target_properties(${EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXEC_OUTPUT_DIR}"
    VERSION ${EXEC_VERSION}
  )

  # Register executable with build system
  set_property(GLOBAL APPEND PROPERTY NLINK_EXECUTABLES ${EXEC_NAME})
  
  # Create install rule for executable
  install(TARGETS ${EXEC_NAME}
    RUNTIME DESTINATION bin
  )

  # Create symlink for development environment
  add_custom_command(TARGET ${EXEC_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
      $<TARGET_FILE:${EXEC_NAME}>
      "${NLINK_BIN_DIR}/${EXEC_NAME}"
    COMMENT "Creating symlink for ${EXEC_NAME} in ${NLINK_BIN_DIR}"
  )
  
  # Verify component boundary integrity
  add_custom_command(TARGET ${EXEC_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Verifying executable integrity: ${EXEC_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "Executable path: $<TARGET_FILE:${EXEC_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E echo "Dependencies: ${EXEC_DEPENDENCIES}"
  )

  message(STATUS "Defined main executable: ${EXEC_NAME} (version ${EXEC_VERSION})")
endfunction()

# Function to define CLI executable with standardized interface
function(nlink_define_cli_executable)
  cmake_parse_arguments(
    CLI
    "ENABLE_COMMAND_COMPLETION;ENABLE_INTERACTIVE_MODE"
    "NAME;BASE_EXECUTABLE"
    "COMMAND_MODULES"
    ${ARGN}
  )

  # Validate required parameters
  if(NOT CLI_NAME)
    message(FATAL_ERROR "NAME parameter is required for nlink_define_cli_executable")
  endif()

  if(NOT CLI_BASE_EXECUTABLE)
    message(FATAL_ERROR "BASE_EXECUTABLE parameter is required for nlink_define_cli_executable")
  endif()

  # Define compile definitions based on options
  set(CLI_COMPILE_DEFS "")
  
  if(CLI_ENABLE_COMMAND_COMPLETION)
    list(APPEND CLI_COMPILE_DEFS "NLINK_ENABLE_COMMAND_COMPLETION")
  endif()
  
  if(CLI_ENABLE_INTERACTIVE_MODE)
    list(APPEND CLI_COMPILE_DEFS "NLINK_ENABLE_INTERACTIVE_MODE")
  endif()

  # Forward to main executable definition with CLI-specific configuration
  nlink_define_main_executable(
    NAME ${CLI_NAME}
    SOURCES ${NLINK_SRC_DIR}/cli/main.c
    DEPENDENCIES ${CLI_BASE_EXECUTABLE} ${CLI_COMMAND_MODULES}
    COMPILE_DEFINITIONS ${CLI_COMPILE_DEFS}
    ${ARGN}
  )

  message(STATUS "Defined CLI executable: ${CLI_NAME}")
endfunction()

# Function to collect all component executables into a registry
function(nlink_collect_executables)
  # Get global property containing all registered executables
  get_property(EXECUTABLES GLOBAL PROPERTY NLINK_EXECUTABLES)
  
  # Create custom target that depends on all executables
  add_custom_target(all_executables ALL
    DEPENDS ${EXECUTABLES}
    COMMENT "Building all NexusLink executables"
  )
  
  # Create summary file for build system
  set(SUMMARY_FILE "${NLINK_BUILD_DIR}/executables.txt")
  file(WRITE ${SUMMARY_FILE} "# NexusLink Executables\n\n")
  
  foreach(EXEC ${EXECUTABLES})
    file(APPEND ${SUMMARY_FILE} "- ${EXEC}\n")
  endforeach()
  
  message(STATUS "Collected ${CMAKE_FIND_PACKAGE_NAME} executables: ${EXECUTABLES}")
endfunction()

# Set up environment for executable builds
function(nlink_setup_executable_environment)
  # Create bin directory if it doesn't exist
  nlink_ensure_directory(${NLINK_BIN_DIR})
  
  # Create registry for executables
  set_property(GLOBAL PROPERTY NLINK_EXECUTABLES "")
  
  message(STATUS "Executable build environment initialized")
endfunction()

# Initialize executable configuration
nlink_setup_executable_environment()
