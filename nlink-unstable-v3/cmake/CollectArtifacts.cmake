# CollectArtifacts.cmake
# Build artifact collection and manifest generation
# Copyright © 2025 OBINexus Computing

# Load build configuration
if(NOT DEFINED NLINK_OUTPUT_ROOT)
    set(NLINK_OUTPUT_ROOT "${CMAKE_BINARY_DIR}")
endif()

set(NLINK_OBJ_ROOT "${NLINK_OUTPUT_ROOT}/obj")
set(NLINK_BIN_ROOT "${NLINK_OUTPUT_ROOT}/bin")
set(NLINK_LIB_ROOT "${NLINK_OUTPUT_ROOT}/lib")
set(NLINK_LOG_ROOT "${NLINK_OUTPUT_ROOT}/logs")

# Generate timestamp
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
string(TIMESTAMP BUILD_DATE "%Y%m%d")

# Create comprehensive manifest
set(MANIFEST_FILE "${NLINK_OUTPUT_ROOT}/build_manifest.json")

# Start JSON manifest
file(WRITE "${MANIFEST_FILE}" "{\n")
file(APPEND "${MANIFEST_FILE}" "  \"build_info\": {\n")
file(APPEND "${MANIFEST_FILE}" "    \"timestamp\": \"${BUILD_TIMESTAMP}\",\n")
file(APPEND "${MANIFEST_FILE}" "    \"platform\": \"${CMAKE_HOST_SYSTEM_NAME}\",\n")
file(APPEND "${MANIFEST_FILE}" "    \"compiler\": \"${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}\",\n")
file(APPEND "${MANIFEST_FILE}" "    \"build_type\": \"${CMAKE_BUILD_TYPE}\"\n")
file(APPEND "${MANIFEST_FILE}" "  },\n")

# Collect executables
file(GLOB EXECUTABLES "${NLINK_BIN_ROOT}/*")
file(APPEND "${MANIFEST_FILE}" "  \"executables\": [\n")
set(FIRST_EXEC TRUE)
foreach(EXEC IN LISTS EXECUTABLES)
    if(IS_DIRECTORY "${EXEC}")
        continue()
    endif()
    
    get_filename_component(EXEC_NAME "${EXEC}" NAME)
    file(SIZE "${EXEC}" EXEC_SIZE)
    
    if(NOT FIRST_EXEC)
        file(APPEND "${MANIFEST_FILE}" ",\n")
    endif()
    set(FIRST_EXEC FALSE)
    
    file(APPEND "${MANIFEST_FILE}" "    {\n")
    file(APPEND "${MANIFEST_FILE}" "      \"name\": \"${EXEC_NAME}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"path\": \"${EXEC}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"size\": ${EXEC_SIZE}\n")
    file(APPEND "${MANIFEST_FILE}" "    }")
endforeach()
file(APPEND "${MANIFEST_FILE}" "\n  ],\n")

# Collect static libraries
file(GLOB STATIC_LIBS "${NLINK_LIB_ROOT}/*.a")
file(APPEND "${MANIFEST_FILE}" "  \"static_libraries\": [\n")
set(FIRST_LIB TRUE)
foreach(LIB IN LISTS STATIC_LIBS)
    get_filename_component(LIB_NAME "${LIB}" NAME)
    file(SIZE "${LIB}" LIB_SIZE)
    
    if(NOT FIRST_LIB)
        file(APPEND "${MANIFEST_FILE}" ",\n")
    endif()
    set(FIRST_LIB FALSE)
    
    file(APPEND "${MANIFEST_FILE}" "    {\n")
    file(APPEND "${MANIFEST_FILE}" "      \"name\": \"${LIB_NAME}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"path\": \"${LIB}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"size\": ${LIB_SIZE}\n")
    file(APPEND "${MANIFEST_FILE}" "    }")
endforeach()
file(APPEND "${MANIFEST_FILE}" "\n  ],\n")

# Collect shared libraries
file(GLOB SHARED_LIBS "${NLINK_LIB_ROOT}/*.so" "${NLINK_LIB_ROOT}/*.dll" "${NLINK_LIB_ROOT}/*.dylib")
file(APPEND "${MANIFEST_FILE}" "  \"shared_libraries\": [\n")
set(FIRST_SHARED TRUE)
foreach(LIB IN LISTS SHARED_LIBS)
    get_filename_component(LIB_NAME "${LIB}" NAME)
    file(SIZE "${LIB}" LIB_SIZE)
    
    if(NOT FIRST_SHARED)
        file(APPEND "${MANIFEST_FILE}" ",\n")
    endif()
    set(FIRST_SHARED FALSE)
    
    file(APPEND "${MANIFEST_FILE}" "    {\n")
    file(APPEND "${MANIFEST_FILE}" "      \"name\": \"${LIB_NAME}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"path\": \"${LIB}\",\n")
    file(APPEND "${MANIFEST_FILE}" "      \"size\": ${LIB_SIZE}\n")
    file(APPEND "${MANIFEST_FILE}" "    }")
endforeach()
file(APPEND "${MANIFEST_FILE}" "\n  ],\n")

# Collect object file statistics
file(GLOB_RECURSE OBJECT_FILES "${NLINK_OBJ_ROOT}/*.o")
list(LENGTH OBJECT_FILES OBJECT_COUNT)
set(TOTAL_OBJECT_SIZE 0)
foreach(OBJ IN LISTS OBJECT_FILES)
    if(EXISTS "${OBJ}")
        file(SIZE "${OBJ}" OBJ_SIZE)
        math(EXPR TOTAL_OBJECT_SIZE "${TOTAL_OBJECT_SIZE} + ${OBJ_SIZE}")
    endif()
endforeach()

file(APPEND "${MANIFEST_FILE}" "  \"object_files\": {\n")
file(APPEND "${MANIFEST_FILE}" "    \"count\": ${OBJECT_COUNT},\n")
file(APPEND "${MANIFEST_FILE}" "    \"total_size\": ${TOTAL_OBJECT_SIZE}\n")
file(APPEND "${MANIFEST_FILE}" "  }\n")
file(APPEND "${MANIFEST_FILE}" "}\n")

# Generate human-readable summary
set(SUMMARY_FILE "${NLINK_OUTPUT_ROOT}/build_summary.txt")
file(WRITE "${SUMMARY_FILE}" "NexusLink Build Summary\n")
file(APPEND "${SUMMARY_FILE}" "=======================\n\n")
file(APPEND "${SUMMARY_FILE}" "Build Date: ${BUILD_TIMESTAMP}\n")
file(APPEND "${SUMMARY_FILE}" "Platform: ${CMAKE_HOST_SYSTEM_NAME}\n")
file(APPEND "${SUMMARY_FILE}" "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}\n")
file(APPEND "${SUMMARY_FILE}" "Build Type: ${CMAKE_BUILD_TYPE}\n\n")

file(APPEND "${SUMMARY_FILE}" "Artifacts:\n")
file(APPEND "${SUMMARY_FILE}" "----------\n")

list(LENGTH EXECUTABLES EXEC_COUNT)
list(LENGTH STATIC_LIBS STATIC_COUNT)
list(LENGTH SHARED_LIBS SHARED_COUNT)

file(APPEND "${SUMMARY_FILE}" "Executables: ${EXEC_COUNT}\n")
foreach(EXEC IN LISTS EXECUTABLES)
    if(NOT IS_DIRECTORY "${EXEC}")
        get_filename_component(EXEC_NAME "${EXEC}" NAME)
        file(APPEND "${SUMMARY_FILE}" "  - ${EXEC_NAME}\n")
    endif()
endforeach()

file(APPEND "${SUMMARY_FILE}" "\nStatic Libraries: ${STATIC_COUNT}\n")
foreach(LIB IN LISTS STATIC_LIBS)
    get_filename_component(LIB_NAME "${LIB}" NAME)
    file(APPEND "${SUMMARY_FILE}" "  - ${LIB_NAME}\n")
endforeach()

file(APPEND "${SUMMARY_FILE}" "\nShared Libraries: ${SHARED_COUNT}\n")
foreach(LIB IN LISTS SHARED_LIBS)
    get_filename_component(LIB_NAME "${LIB}" NAME)
    file(APPEND "${SUMMARY_FILE}" "  - ${LIB_NAME}\n")
endforeach()

file(APPEND "${SUMMARY_FILE}" "\nObject Files: ${OBJECT_COUNT} files (${TOTAL_OBJECT_SIZE} bytes)\n")

# Display summary to console
message(STATUS "Build artifacts collected:")
message(STATUS "  Manifest: ${MANIFEST_FILE}")
message(STATUS "  Summary: ${SUMMARY_FILE}")
message(STATUS "  Executables: ${EXEC_COUNT}")
message(STATUS "  Static Libraries: ${STATIC_COUNT}")
message(STATUS "  Shared Libraries: ${SHARED_COUNT}")
message(STATUS "  Object Files: ${OBJECT_COUNT}")