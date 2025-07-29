# ValidateOutput.cmake
# Post-build validation script for output location verification
# Copyright © 2025 OBINexus Computing

# Verify target file exists
if(NOT EXISTS "${TARGET_FILE}")
    message(FATAL_ERROR "Target file does not exist: ${TARGET_FILE}")
endif()

# Extract directory from target file
get_filename_component(ACTUAL_DIR "${TARGET_FILE}" DIRECTORY)
get_filename_component(TARGET_NAME "${TARGET_FILE}" NAME)

# Normalize paths for comparison
get_filename_component(ACTUAL_DIR_ABS "${ACTUAL_DIR}" ABSOLUTE)
get_filename_component(EXPECTED_PATH_ABS "${EXPECTED_PATH}" ABSOLUTE)

# Verify output is in expected directory
if(NOT "${ACTUAL_DIR_ABS}" STREQUAL "${EXPECTED_PATH_ABS}")
    message(WARNING "Output location mismatch for ${TARGET_NAME}")
    message(WARNING "  Expected: ${EXPECTED_PATH_ABS}")
    message(WARNING "  Actual:   ${ACTUAL_DIR_ABS}")
    
    # Attempt to relocate artifact to correct location
    file(COPY "${TARGET_FILE}" DESTINATION "${EXPECTED_PATH_ABS}")
    message(STATUS "Relocated ${TARGET_NAME} to standardized location")
endif()

# Verify artifact permissions
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    execute_process(
        COMMAND stat -c "%a" "${TARGET_FILE}"
        OUTPUT_VARIABLE FILE_PERMS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(FILE_PERMS LESS 644)
        message(WARNING "Insufficient permissions on ${TARGET_NAME}: ${FILE_PERMS}")
        execute_process(COMMAND chmod 644 "${TARGET_FILE}")
    endif()
endif()

message(STATUS "Validated output: ${TARGET_NAME} in ${ACTUAL_DIR_ABS}")