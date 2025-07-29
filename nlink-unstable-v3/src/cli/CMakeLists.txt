# CMakeLists.txt for NexusLink CLI components
# Copyright © 2025 OBINexus Computing

# ════════════════════════════════════════════════════════════
# CLI COMPONENT BOUNDARY DEFINITION
# ════════════════════════════════════════════════════════════

# Define CLI source files explicitly
set(CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command_params.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command_registration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command_registry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command_router.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parse.c
)

# ════════════════════════════════════════════════════════════
# AGGREGATION TARGET MATERIALIZATION (MUST COME FIRST)
# ════════════════════════════════════════════════════════════

# Create CLI component aggregation target BEFORE using it
add_custom_target(nlink_cli_components
    COMMENT "Building all NexusLink CLI components"
)

# ════════════════════════════════════════════════════════════
# PRIMARY TARGET MATERIALIZATION
# ════════════════════════════════════════════════════════════

# Create CLI static library
add_library(nlink_cli_static STATIC ${CLI_SOURCES})

# ════════════════════════════════════════════════════════════
# BUILD OUTPUT STANDARDIZATION INTEGRATION
# ════════════════════════════════════════════════════════════

# Include build output standard module if available
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake")
    include(${CMAKE_SOURCE_DIR}/cmake/BuildOutputStandard.cmake)
endif()

# Set library properties with standardized output
set_target_properties(nlink_cli_static PROPERTIES
    OUTPUT_NAME "nlink_cli"
    ARCHIVE_OUTPUT_DIRECTORY "${NLINK_LIB_ROOT}"
)

# Validate output using standardization framework
if(COMMAND nlink_validate_target_output)
    nlink_validate_target_output(nlink_cli_static "STATIC_LIBRARY")
endif()

# ════════════════════════════════════════════════════════════
# INTERFACE BOUNDARY SPECIFICATION
# ════════════════════════════════════════════════════════════

target_include_directories(nlink_cli_static PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define compile-time flags for CLI support
target_compile_definitions(nlink_cli_static PRIVATE 
    NLINK_ENABLE_CLI
)

# ════════════════════════════════════════════════════════════
# COMPONENT AGGREGATION
# ════════════════════════════════════════════════════════════

# Add static library to components target
add_dependencies(nlink_cli_components nlink_cli_static)

# ════════════════════════════════════════════════════════════
# COMMAND SUBCOMPONENT INTEGRATION
# ════════════════════════════════════════════════════════════

# Check for commands subdirectory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/commands/CMakeLists.txt")
    add_subdirectory(commands)
    # Link command components if they exist
    if(TARGET nlink_commands_static)
        target_link_libraries(nlink_cli_static PRIVATE nlink_commands_static)
        add_dependencies(nlink_cli_components nlink_commands_static)
    endif()
else()
    # If no CMakeLists.txt in commands, collect sources directly
    file(GLOB COMMAND_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/commands/*.c")
    if(COMMAND_SOURCES)
        target_sources(nlink_cli_static PRIVATE ${COMMAND_SOURCES})
    endif()
endif()

# ════════════════════════════════════════════════════════════
# OBJECT LIBRARY CONFIGURATION (for flexibility)
# ════════════════════════════════════════════════════════════

# Create object library for position-independent code
add_library(nlink_cli_objects OBJECT ${CLI_SOURCES})
target_include_directories(nlink_cli_objects PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(nlink_cli_objects PRIVATE NLINK_ENABLE_CLI)
set_property(TARGET nlink_cli_objects PROPERTY POSITION_INDEPENDENT_CODE ON)

# ════════════════════════════════════════════════════════════
# SHARED LIBRARY CONFIGURATION (if enabled)
# ════════════════════════════════════════════════════════════

if(BUILD_SHARED_LIBS)
    add_library(nlink_cli_shared SHARED $<TARGET_OBJECTS:nlink_cli_objects>)
    
    set_target_properties(nlink_cli_shared PROPERTIES
        OUTPUT_NAME "nlink_cli"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    target_include_directories(nlink_cli_shared PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    target_compile_definitions(nlink_cli_shared PRIVATE NLINK_ENABLE_CLI)
    
    # Note: Dependencies on nlink_core will be established by root CMakeLists.txt
    # to ensure proper target materialization order
    
    add_dependencies(nlink_cli_components nlink_cli_shared)
endif()

# ════════════════════════════════════════════════════════════
# ARTIFACT PRODUCTION VALIDATION
# ════════════════════════════════════════════════════════════

# Ensure artifact is produced in expected location
add_custom_command(
    TARGET nlink_cli_static POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:nlink_cli_static> ${CMAKE_BINARY_DIR}/lib/libnlink_cli.a
    COMMENT "Validating CLI library artifact production"
    VERBATIM
)

# ════════════════════════════════════════════════════════════
# INSTALLATION CONFIGURATION
# ════════════════════════════════════════════════════════════

# Collect CLI headers
file(GLOB CLI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Install CLI headers
install(
    FILES ${CLI_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlink/cli
    COMPONENT devel
)

# Install CLI library
install(
    TARGETS nlink_cli_static
    EXPORT nlink-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT devel
)

# Install CLI shared library if built
if(BUILD_SHARED_LIBS AND TARGET nlink_cli_shared)
    install(
        TARGETS nlink_cli_shared
        EXPORT nlink-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT runtime
    )
endif()

# ════════════════════════════════════════════════════════════
# BUILD STATUS REPORT
# ════════════════════════════════════════════════════════════

message(STATUS "════════════════════════════════════════════════════")
message(STATUS "NexusLink CLI Component Configuration")
message(STATUS "════════════════════════════════════════════════════")
message(STATUS "CLI sources:       ${CLI_SOURCES}")
message(STATUS "Static library:    nlink_cli_static")
message(STATUS "Shared library:    ${BUILD_SHARED_LIBS}")
message(STATUS "Output directory:  ${CMAKE_BINARY_DIR}/lib")
message(STATUS "════════════════════════════════════════════════════")