# BuildOutputStandard.cmake
# Enforces standardized build output directory structure for NexusLink
# Copyright © 2025 OBINexus Computing

if(DEFINED NLINK_BUILD_OUTPUT_STANDARD_INCLUDED)
  return()
endif()
set(NLINK_BUILD_OUTPUT_STANDARD_INCLUDED TRUE)

# ════════════════════════════════════════════════════════════
# DIRECTORY STRUCTURE INVARIANTS
# ════════════════════════════════════════════════════════════

# Define canonical output directory structure
set(NLINK_OUTPUT_ROOT "${CMAKE_BINARY_DIR}" CACHE PATH "Root build output directory")
set(NLINK_OBJ_ROOT "${NLINK_OUTPUT_ROOT}/obj" CACHE PATH "Object files directory")
set(NLINK_BIN_ROOT "${NLINK_OUTPUT_ROOT}/bin" CACHE PATH "Executable binaries directory")
set(NLINK_LIB_ROOT "${NLINK_OUTPUT_ROOT}/lib" CACHE PATH "Library archives directory")
set(NLINK_LOG_ROOT "${NLINK_OUTPUT_ROOT}/logs" CACHE PATH "Build logs directory")

# ════════════════════════════════════════════════════════════
# DIRECTORY MATERIALIZATION PROTOCOL
# ════════════════════════════════════════════════════════════

function(nlink_enforce_output_structure)
    # Primary directory creation with validation
    foreach(DIR IN ITEMS 
            ${NLINK_OUTPUT_ROOT}
            ${NLINK_OBJ_ROOT}
            ${NLINK_BIN_ROOT}
            ${NLINK_LIB_ROOT}
            ${NLINK_LOG_ROOT})
        
        if(NOT EXISTS "${DIR}")
            file(MAKE_DIRECTORY "${DIR}")
            message(STATUS "Created output directory: ${DIR}")
        endif()
        
        # Verify directory is writable
        file(TOUCH "${DIR}/.build_marker")
        if(NOT EXISTS "${DIR}/.build_marker")
            message(FATAL_ERROR "Cannot write to output directory: ${DIR}")
        endif()
        file(REMOVE "${DIR}/.build_marker")
    endforeach()
    
    # Component-specific object directories
    foreach(COMPONENT IN LISTS NLINK_CORE_COMPONENTS NLINK_CLI_COMPONENTS)
        set(COMPONENT_OBJ_DIR "${NLINK_OBJ_ROOT}/${COMPONENT}")
        file(MAKE_DIRECTORY "${COMPONENT_OBJ_DIR}")
    endforeach()
    
    # Build configuration specific directories
    file(MAKE_DIRECTORY "${NLINK_BIN_ROOT}/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY "${NLINK_LIB_ROOT}/${CMAKE_BUILD_TYPE}")
    
    # Timestamp-based log directory
    string(TIMESTAMP BUILD_TIMESTAMP "%Y%m%d_%H%M%S")
    set(NLINK_LOG_SESSION_DIR "${NLINK_LOG_ROOT}/session_${BUILD_TIMESTAMP}" PARENT_SCOPE)
    file(MAKE_DIRECTORY "${NLINK_LOG_SESSION_DIR}")
endfunction()

# ════════════════════════════════════════════════════════════
# OUTPUT PATH CONFIGURATION ENFORCEMENT
# ════════════════════════════════════════════════════════════

function(nlink_configure_output_paths)
    # Global output path configuration
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${NLINK_LIB_ROOT}" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${NLINK_LIB_ROOT}" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${NLINK_BIN_ROOT}" PARENT_SCOPE)
    
    # Configuration-specific paths
    foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NLINK_LIB_ROOT}" PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NLINK_LIB_ROOT}" PARENT_SCOPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NLINK_BIN_ROOT}" PARENT_SCOPE)
    endforeach()
    
    # Object file redirection
    set(CMAKE_OBJECT_PATH_PREFIX "${NLINK_OBJ_ROOT}/" PARENT_SCOPE)
endfunction()

# ════════════════════════════════════════════════════════════
# TARGET OUTPUT VALIDATION
# ════════════════════════════════════════════════════════════

function(nlink_validate_target_output TARGET_NAME OUTPUT_TYPE)
    # Define expected output paths
    if(OUTPUT_TYPE STREQUAL "EXECUTABLE")
        set(EXPECTED_PATH "${NLINK_BIN_ROOT}")
        set(EXTENSION "${CMAKE_EXECUTABLE_SUFFIX}")
    elseif(OUTPUT_TYPE STREQUAL "STATIC_LIBRARY")
        set(EXPECTED_PATH "${NLINK_LIB_ROOT}")
        set(EXTENSION "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    elseif(OUTPUT_TYPE STREQUAL "SHARED_LIBRARY")
        set(EXPECTED_PATH "${NLINK_LIB_ROOT}")
        set(EXTENSION "${CMAKE_SHARED_LIBRARY_SUFFIX}")
    else()
        message(FATAL_ERROR "Unknown output type: ${OUTPUT_TYPE}")
    endif()
    
    # Add post-build validation
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} 
            -DTARGET_FILE=$<TARGET_FILE:${TARGET_NAME}>
            -DEXPECTED_PATH=${EXPECTED_PATH}
            -P ${CMAKE_CURRENT_LIST_DIR}/ValidateOutput.cmake
        COMMENT "Validating output location for ${TARGET_NAME}"
        VERBATIM
    )
endfunction()

# ════════════════════════════════════════════════════════════
# ARTIFACT COLLECTION PROTOCOL
# ════════════════════════════════════════════════════════════

function(nlink_collect_build_artifacts)
    # Create artifact manifest
    set(MANIFEST_FILE "${NLINK_OUTPUT_ROOT}/build_manifest.txt")
    file(WRITE "${MANIFEST_FILE}" "# NexusLink Build Manifest\n")
    file(APPEND "${MANIFEST_FILE}" "# Generated: ${BUILD_TIMESTAMP}\n\n")
    
    # Collect executables
    file(GLOB EXECUTABLES "${NLINK_BIN_ROOT}/*${CMAKE_EXECUTABLE_SUFFIX}")
    if(EXECUTABLES)
        file(APPEND "${MANIFEST_FILE}" "[EXECUTABLES]\n")
        foreach(EXEC IN LISTS EXECUTABLES)
            get_filename_component(EXEC_NAME "${EXEC}" NAME)
            file(APPEND "${MANIFEST_FILE}" "${EXEC_NAME}\n")
        endforeach()
        file(APPEND "${MANIFEST_FILE}" "\n")
    endif()
    
    # Collect libraries
    file(GLOB STATIC_LIBS "${NLINK_LIB_ROOT}/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    file(GLOB SHARED_LIBS "${NLINK_LIB_ROOT}/*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    
    if(STATIC_LIBS)
        file(APPEND "${MANIFEST_FILE}" "[STATIC_LIBRARIES]\n")
        foreach(LIB IN LISTS STATIC_LIBS)
            get_filename_component(LIB_NAME "${LIB}" NAME)
            file(APPEND "${MANIFEST_FILE}" "${LIB_NAME}\n")
        endforeach()
        file(APPEND "${MANIFEST_FILE}" "\n")
    endif()
    
    if(SHARED_LIBS)
        file(APPEND "${MANIFEST_FILE}" "[SHARED_LIBRARIES]\n")
        foreach(LIB IN LISTS SHARED_LIBS)
            get_filename_component(LIB_NAME "${LIB}" NAME)
            file(APPEND "${MANIFEST_FILE}" "${LIB_NAME}\n")
        endforeach()
        file(APPEND "${MANIFEST_FILE}" "\n")
    endif()
    
    # Generate build summary
    message(STATUS "════════════════════════════════════════════════════")
    message(STATUS "Build Artifact Summary")
    message(STATUS "════════════════════════════════════════════════════")
    message(STATUS "Executables: ${EXECUTABLES}")
    message(STATUS "Static Libraries: ${STATIC_LIBS}")
    message(STATUS "Shared Libraries: ${SHARED_LIBS}")
    message(STATUS "Manifest: ${MANIFEST_FILE}")
    message(STATUS "════════════════════════════════════════════════════")
endfunction()

# ════════════════════════════════════════════════════════════
# INITIALIZATION PROTOCOL
# ════════════════════════════════════════════════════════════

# Automatically enforce structure when module is included
nlink_enforce_output_structure()
nlink_configure_output_paths()

# Register artifact collection as final build step
if(NOT TARGET collect_artifacts)
    add_custom_target(collect_artifacts ALL
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CollectArtifacts.cmake
        COMMENT "Collecting build artifacts"
        VERBATIM
    )
endif()