# Core components CMakeLists.txt for NexusLink
# Copyright © 2025 OBINexus Computing

# ════════════════════════════════════════════════════════════
# PRE-BUILD VALIDATION
# ════════════════════════════════════════════════════════════

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# ════════════════════════════════════════════════════════════
# COMPONENT DISCOVERY
# ════════════════════════════════════════════════════════════

# Define core components through filesystem discovery
set(NLINK_CORE_COMPONENTS "")
file(GLOB COMPONENT_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")

# Extract valid component directories
foreach(DIR ${COMPONENT_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}" AND
       NOT "${DIR}" STREQUAL "CMakeFiles" AND
       NOT "${DIR}" STREQUAL "cmake" AND
       NOT "${DIR}" MATCHES "^\\.")
        
        message(STATUS "Found core component: ${DIR}")
        list(APPEND NLINK_CORE_COMPONENTS ${DIR})
    endif()
endforeach()

# Store component list globally
set_property(GLOBAL PROPERTY NLINK_CORE_COMPONENTS ${NLINK_CORE_COMPONENTS})

# ════════════════════════════════════════════════════════════
# CORE LIBRARY MATERIALIZATION
# ════════════════════════════════════════════════════════════

# Define core sources (collected from components)
set(CORE_SOURCES "")

# Process each component
foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
    # Define unique binary build directory for each component
    set(COMPONENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT}_build")
    
    # Add subdirectory with specified binary directory to prevent collisions
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}" 
                         "${COMPONENT_BINARY_DIR}")
    else()
        # If no CMakeLists.txt, collect sources directly
        file(GLOB COMPONENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/*.c")
        list(APPEND CORE_SOURCES ${COMPONENT_SOURCES})
    endif()
endforeach()

# ════════════════════════════════════════════════════════════
# PRIMARY TARGET MATERIALIZATION
# ════════════════════════════════════════════════════════════

# Manually specify core sources if automatic collection failed
if(NOT CORE_SOURCES)
    set(CORE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/common/nexus_core.c
        ${CMAKE_CURRENT_SOURCE_DIR}/common/nexus_error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/common/nexus_result.c
        ${CMAKE_CURRENT_SOURCE_DIR}/versioning/nexus_version.c
        ${CMAKE_CURRENT_SOURCE_DIR}/versioning/semver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/symbols/nexus_symbols.c
        ${CMAKE_CURRENT_SOURCE_DIR}/symbols/registry.c
        ${CMAKE_CURRENT_SOURCE_DIR}/minimizer/nexus_minimizer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/pattern/matcher.c
    )
endif()

# Create nlink_core static library
add_library(nlink_core STATIC ${CORE_SOURCES})

# ════════════════════════════════════════════════════════════
# BUILD OUTPUT STANDARDIZATION INTEGRATION
# ════════════════════════════════════════════════════════════

# Include build output standard module if available
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake")
    include(${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake)
endif()

# Set library properties with standardized output
set_target_properties(nlink_core PROPERTIES
    OUTPUT_NAME "nlink_core"
    ARCHIVE_OUTPUT_DIRECTORY "${NLINK_LIB_ROOT}"
)

# Validate output using standardization framework
if(COMMAND nlink_validate_target_output)
    nlink_validate_target_output(nlink_core "STATIC_LIBRARY")
endif()

# ════════════════════════════════════════════════════════════
# INTERFACE BOUNDARY SPECIFICATION
# ════════════════════════════════════════════════════════════

target_include_directories(nlink_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Component-specific dependencies
target_link_libraries(nlink_core PRIVATE
    pthread
    dl
)

# ════════════════════════════════════════════════════════════
# AGGREGATION TARGET MATERIALIZATION
# ════════════════════════════════════════════════════════════

add_custom_target(nlink_unified_core
    DEPENDS nlink_core
    COMMENT "Building unified NexusLink core system"
)

# ════════════════════════════════════════════════════════════
# ARTIFACT PRODUCTION VALIDATION
# ════════════════════════════════════════════════════════════

# Ensure artifact is produced in expected location
add_custom_command(
    TARGET nlink_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:nlink_core> ${CMAKE_BINARY_DIR}/lib/libnlink_core.a
    COMMENT "Validating core library artifact production"
    VERBATIM
)

# ════════════════════════════════════════════════════════════
# SHARED LIBRARY CONFIGURATION (if enabled)
# ════════════════════════════════════════════════════════════

if(BUILD_SHARED_LIBS)
    add_library(nlink_core_shared SHARED ${CORE_SOURCES})
    
    set_target_properties(nlink_core_shared PROPERTIES
        OUTPUT_NAME "nlink_core"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    target_include_directories(nlink_core_shared PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    target_link_libraries(nlink_core_shared PRIVATE
        pthread
        dl
    )
    
    add_dependencies(nlink_unified_core nlink_core_shared)
endif()

# ════════════════════════════════════════════════════════════
# COMPONENT OBJECT LIBRARIES (for granular control)
# ════════════════════════════════════════════════════════════

# Create object libraries for each component (allows fine-grained linking)
foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}")
        file(GLOB COMPONENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/*.c")
        if(COMPONENT_SOURCES)
            add_library(nlink_${COMPONENT}_objects OBJECT ${COMPONENT_SOURCES})
            target_include_directories(nlink_${COMPONENT}_objects PRIVATE
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}
            )
            set_property(TARGET nlink_${COMPONENT}_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
        endif()
    endif()
endforeach()

# ════════════════════════════════════════════════════════════
# POST-BUILD VALIDATION
# ════════════════════════════════════════════════════════════

# Execute post-build hooks
nlink_execute_post_build_hooks()

# Build summary
message(STATUS "════════════════════════════════════════════════════")
message(STATUS "NexusLink Core Build Summary")
message(STATUS "════════════════════════════════════════════════════")
message(STATUS "Components:        ${NLINK_CORE_COMPONENTS}")
message(STATUS "Core library:      nlink_core")
message(STATUS "Shared library:    ${BUILD_SHARED_LIBS}")
message(STATUS "Output directory:  ${CMAKE_BINARY_DIR}/lib")
message(STATUS "════════════════════════════════════════════════════")