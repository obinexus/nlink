# Utils.cmake
# Utility functions for NexusLink build system
#
# This module provides common utility functions used throughout the build system.

if(DEFINED NLINK_UTILS_INCLUDED)
  return()
endif()
set(NLINK_UTILS_INCLUDED TRUE)

include(CMakeParseArguments)

# Ensure a directory exists
function(nlink_ensure_directory DIRECTORY)
  if(NOT EXISTS "${DIRECTORY}")
    file(MAKE_DIRECTORY "${DIRECTORY}")
  endif()
endfunction()

# Generate a timestamp string
function(nlink_timestamp OUTPUT_VAR)
  string(TIMESTAMP TIMESTAMP "%Y%m%d_%H%M%S")
  set(${OUTPUT_VAR} ${TIMESTAMP} PARENT_SCOPE)
endfunction()

# Execute a Python script
function(nlink_run_python_script)
  cmake_parse_arguments(
    SCRIPT
    "QUIET"
    "SCRIPT;OUTPUT_VAR"
    "ARGS"
    ${ARGN}
  )
  
  # Find Python interpreter
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  
  # Validate arguments
  if(NOT SCRIPT_SCRIPT)
    message(FATAL_ERROR "SCRIPT argument is required for nlink_run_python_script")
  endif()
  
  # Construct command
  set(COMMAND ${Python3_EXECUTABLE} ${SCRIPT_SCRIPT} ${SCRIPT_ARGS})
  
  # Execute command
  if(SCRIPT_OUTPUT_VAR)
    execute_process(
      COMMAND ${COMMAND}
      OUTPUT_VARIABLE SCRIPT_OUTPUT
      ERROR_VARIABLE SCRIPT_ERROR
      RESULT_VARIABLE SCRIPT_RESULT
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    # Set output variable
    set(${SCRIPT_OUTPUT_VAR} ${SCRIPT_OUTPUT} PARENT_SCOPE)
    
    # Check result
    if(NOT SCRIPT_RESULT EQUAL 0)
      if(NOT SCRIPT_QUIET)
        message(WARNING "Python script execution failed: ${SCRIPT_ERROR}")
      endif()
    endif()
  else()
    execute_process(
      COMMAND ${COMMAND}
      RESULT_VARIABLE SCRIPT_RESULT
    )
    
    # Check result
    if(NOT SCRIPT_RESULT EQUAL 0)
      if(NOT SCRIPT_QUIET)
        message(WARNING "Python script execution failed with code ${SCRIPT_RESULT}")
      endif()
    endif()
  endif()
endfunction()

# Execute a shell script
function(nlink_run_shell_script)
  cmake_parse_arguments(
    SCRIPT
    "QUIET"
    "SCRIPT;OUTPUT_VAR"
    "ARGS"
    ${ARGN}
  )
  
  # Validate arguments
  if(NOT SCRIPT_SCRIPT)
    message(FATAL_ERROR "SCRIPT argument is required for nlink_run_shell_script")
  endif()
  
  # Ensure script is executable
  if(NOT EXISTS "${SCRIPT_SCRIPT}")
    message(FATAL_ERROR "Script file not found: ${SCRIPT_SCRIPT}")
  endif()
  
  # Make script executable if needed
  execute_process(
    COMMAND chmod +x ${SCRIPT_SCRIPT}
    RESULT_VARIABLE CHMOD_RESULT
  )
  
  if(NOT CHMOD_RESULT EQUAL 0)
    message(WARNING "Failed to make script executable: ${SCRIPT_SCRIPT}")
  endif()
  
  # Construct command
  set(COMMAND "${SCRIPT_SCRIPT}" ${SCRIPT_ARGS})
  
  # Execute command
  if(SCRIPT_OUTPUT_VAR)
    execute_process(
      COMMAND ${COMMAND}
      OUTPUT_VARIABLE SCRIPT_OUTPUT
      ERROR_VARIABLE SCRIPT_ERROR
      RESULT_VARIABLE SCRIPT_RESULT
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    # Set output variable
    set(${SCRIPT_OUTPUT_VAR} ${SCRIPT_OUTPUT} PARENT_SCOPE)
    
    # Check result
    if(NOT SCRIPT_RESULT EQUAL 0)
      if(NOT SCRIPT_QUIET)
        message(WARNING "Shell script execution failed: ${SCRIPT_ERROR}")
      endif()
    endif()
  else()
    execute_process(
      COMMAND ${COMMAND}
      RESULT_VARIABLE SCRIPT_RESULT
    )
    
    # Check result
    if(NOT SCRIPT_RESULT EQUAL 0)
      if(NOT SCRIPT_QUIET)
        message(WARNING "Shell script execution failed with code ${SCRIPT_RESULT}")
      endif()
    endif()
  endif()
endfunction()

# Define a logging function
function(nlink_log)
  cmake_parse_arguments(
    LOG
    "ERROR;WARNING;STATUS;DEBUG"
    "FILE;PREFIX"
    "MESSAGE"
    ${ARGN}
  )
  
  # Set default message prefix
  if(NOT LOG_PREFIX)
    set(LOG_PREFIX "NexusLink")
  endif()
  
  # Construct message
  set(LOG_TEXT "[${LOG_PREFIX}] ${LOG_MESSAGE}")
  
  # Log to console based on level
  if(LOG_ERROR)
    message(ERROR "${LOG_TEXT}")
  elseif(LOG_WARNING)
    message(WARNING "${LOG_TEXT}")
  elseif(LOG_STATUS)
    message(STATUS "${LOG_TEXT}")
  elseif(LOG_DEBUG)
    if(CMAKE_VERBOSE_MAKEFILE)
      message(STATUS "[DEBUG] ${LOG_TEXT}")
    endif()
  else()
    message(STATUS "${LOG_TEXT}")
  endif()
  
  # Log to file if specified
  if(LOG_FILE)
    nlink_ensure_directory(${NLINK_LOG_DIR})
    file(APPEND ${LOG_FILE} "${LOG_TEXT}\n")
  endif()
endfunction()

# Check for code style compliance
function(nlink_check_code_style)
  cmake_parse_arguments(
    STYLE
    "FIX"
    "OUTPUT"
    "FILES;EXCLUDE"
    ${ARGN}
  )
  
  # Set default output file
  if(NOT STYLE_OUTPUT)
    nlink_timestamp(TIMESTAMP)
    set(STYLE_OUTPUT "${NLINK_LOG_DIR}/style_check_${TIMESTAMP}.log")
  endif()
  
  # Set default files if not provided
  if(NOT STYLE_FILES)
    # Find all source files
    file(GLOB_RECURSE STYLE_FILES 
      "${NLINK_SRC_DIR}/*.c"
      "${NLINK_SRC_DIR}/*.h"
      "${NLINK_INCLUDE_DIR}/*.h"
    )
  endif()
  
  # Exclude files if specified
  if(STYLE_EXCLUDE)
    foreach(EXCLUDE_PATTERN ${STYLE_EXCLUDE})
      list(FILTER STYLE_FILES EXCLUDE REGEX "${EXCLUDE_PATTERN}")
    endforeach()
  endif()
  
  # Run clang-format if available
  find_program(CLANG_FORMAT clang-format)
  if(CLANG_FORMAT)
    if(STYLE_FIX)
      # Fix style issues
      foreach(FILE ${STYLE_FILES})
        execute_process(
          COMMAND ${CLANG_FORMAT} -i ${FILE}
          RESULT_VARIABLE FORMAT_RESULT
        )
        
        if(NOT FORMAT_RESULT EQUAL 0)
          nlink_log(
            WARNING
            FILE ${STYLE_OUTPUT}
            MESSAGE "Failed to format file: ${FILE}"
          )
        endif()
      endforeach()
      
      nlink_log(
        STATUS
        FILE ${STYLE_OUTPUT}
        MESSAGE "Fixed code style issues in ${STYLE_FILES}"
      )
    else()
      # Check style without fixing
      set(STYLE_ERRORS FALSE)
      foreach(FILE ${STYLE_FILES})
        execute_process(
          COMMAND ${CLANG_FORMAT} --dry-run --Werror ${FILE}
          RESULT_VARIABLE FORMAT_RESULT
        )
        
        if(NOT FORMAT_RESULT EQUAL 0)
          set(STYLE_ERRORS TRUE)
          nlink_log(
            WARNING
            FILE ${STYLE_OUTPUT}
            MESSAGE "Style issues found in file: ${FILE}"
          )
        endif()
      endforeach()
      
      if(STYLE_ERRORS)
        nlink_log(
          WARNING
          FILE ${STYLE_OUTPUT}
          MESSAGE "Code style issues found. Run with FIX option to fix them."
        )
      else()
        nlink_log(
          STATUS
          FILE ${STYLE_OUTPUT}
          MESSAGE "No code style issues found."
        )
      endif()
    endif()
  else()
    nlink_log(
      WARNING
      FILE ${STYLE_OUTPUT}
      MESSAGE "clang-format not found. Skipping code style check."
    )
  endif()
endfunction()

# Find build dependencies
function(nlink_find_dependencies)
  # Find Python interpreter
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(NLINK_PYTHON_EXECUTABLE ${Python3_EXECUTABLE} PARENT_SCOPE)
  
  # Find threads package
  find_package(Threads REQUIRED)
  
  # Find other dependencies
  if(BUILD_TESTING)
    # Find test dependencies
  endif()
  
  # Report found dependencies
  nlink_log(
    STATUS
    MESSAGE "Found Python3: ${Python3_EXECUTABLE} (version ${Python3_VERSION})"
  )
  
  nlink_log(
    STATUS
    MESSAGE "Found Threads: ${CMAKE_THREAD_LIBS_INIT}"
  )
endfunction()

# Generate a unified header
function(nlink_generate_unified_header)
  cmake_parse_arguments(
    HEADER
    "VERBOSE"
    "OUTPUT;TEMPLATE"
    "COMPONENTS"
    ${ARGN}
  )
  
  # Set default output path
  if(NOT HEADER_OUTPUT)
    set(HEADER_OUTPUT "${CMAKE_BINARY_DIR}/include/nlink.h")
  endif()
  
  # Set default template
  if(NOT HEADER_TEMPLATE)
    set(HEADER_TEMPLATE "${CMAKE_SOURCE_DIR}/templates/nlink.h.in")
    # If template doesn't exist, use default content
    if(NOT EXISTS "${HEADER_TEMPLATE}")
      # Create a default header template
      nlink_ensure_directory("${CMAKE_SOURCE_DIR}/templates")
      file(WRITE "${HEADER_TEMPLATE}" "/**
 * @file nlink.h
 * @brief Unified header for NexusLink dynamic component linkage system
 * @version @NLINK_VERSION@
 * 
 * This header provides access to all NexusLink functionality.
 * 
 * Copyright (c) 2025 NexusLink Team
 */

#ifndef NLINK_H
#define NLINK_H

#ifdef __cplusplus
extern \"C\" {
#endif

/* Core components */
#include <nlink/core/nlink.h>

/* Common utilities */
#include <nlink/core/common/nexus_core.h>
#include <nlink/core/common/result.h>
#include <nlink/core/common/types.h>

/* Symbol management */
#include <nlink/core/symbols/nexus_symbols.h>
#include <nlink/core/symbols/registry.h>

/* Version management */
#include <nlink/core/versioning/nexus_version.h>
#include <nlink/core/versioning/semver.h>

/* Component minimization */
#include <nlink/core/minimizer/nexus_minimizer.h>
#include <nlink/core/minimizer/okpala_automaton.h>

/* Pipeline system */
#include <nlink/core/pipeline/nlink_pipeline.h>

/* Multi-pass system */
#include <nlink/core/mpsystem/mps_pipeline.h>

/* Single-pass system */
#include <nlink/core/spsystem/sps_pipeline.h>

/* Pattern matching */
#include <nlink/core/pattern/matcher.h>

/* CLI functionality (if CLI support is enabled) */
#ifdef NLINK_ENABLE_CLI
#include <nlink/cli/command.h>
#include <nlink/cli/command_registry.h>
#endif

/**
 * Initialize the NexusLink system
 * 
 * This function must be called before any other NexusLink function.
 * It initializes all components and prepares the system for use.
 * 
 * @param config Path to a configuration file or NULL for default configuration
 * @return 0 on success, error code on failure
 */
int nlink_init(const char* config);

/**
 * Clean up and shut down the NexusLink system
 * 
 * This function should be called before the application exits to properly
 * clean up resources allocated by NexusLink.
 * 
 * @return 0 on success, error code on failure
 */
int nlink_shutdown(void);

/**
 * Get the NexusLink version string
 * 
 * @return A string containing the version information
 */
const char* nlink_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_H */
")
    endif()
  endif()
  
  # Ensure output directory exists
  nlink_ensure_directory(${CMAKE_BINARY_DIR}/include)
  
  # Get current version
  get_property(NLINK_VERSION GLOBAL PROPERTY NLINK_VERSION)
  if(NOT NLINK_VERSION)
    set(NLINK_VERSION "1.0.0")
  endif()
  
  # Get component list
  if(NOT HEADER_COMPONENTS)
    get_property(HEADER_COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
  endif()
  
  # Configure the header file
  configure_file(
    ${HEADER_TEMPLATE}
    ${HEADER_OUTPUT}
    @ONLY
  )
  
  if(HEADER_VERBOSE)
    nlink_log(
      STATUS
      MESSAGE "Generated unified header: ${HEADER_OUTPUT}"
    )
  endif()
endfunction()
