# ExecutableConfig.cmake
# Unified executable configuration for NexusLink
# Copyright © 2025 OBINexus Computing

# ════════════════════════════════════════════════════════════
# SINGLETON PATTERN ENFORCEMENT
# ════════════════════════════════════════════════════════════

include_guard(GLOBAL)

# Global state tracking for executable materialization
set_property(GLOBAL PROPERTY NLINK_EXECUTABLE_DEFINED FALSE)


/

# ════════════════════════════════════════════════════════════
# PRIMARY EXECUTABLE DEFINITION
# ════════════════════════════════════════════════════════════

function(nlink_define_main_executable)
    # Check global singleton constraint
    get_property(EXEC_DEFINED GLOBAL PROPERTY NLINK_EXECUTABLE_DEFINED)
    if(EXEC_DEFINED)
        message(STATUS "NexusLink executable already defined - enforcing singleton pattern")
        return()
    endif()
    
    # Parse arguments for flexibility
    cmake_parse_arguments(
        EXEC
        "STATIC_LINKING;ENABLE_PROFILING"
        "NAME;VERSION;OUTPUT_DIR"
        "SOURCES;INCLUDES;DEPENDENCIES"
        ${ARGN}
    )
    
    # Set defaults
    if(NOT EXEC_NAME)
        set(EXEC_NAME "nlink")
    endif()
    
    if(NOT EXEC_SOURCES)
        # Auto-discover main source
        if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.c")
            set(EXEC_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")
        elseif(EXISTS "${CMAKE_SOURCE_DIR}/src/cli/main.c")
            set(EXEC_SOURCES "${CMAKE_SOURCE_DIR}/src/cli/main.c")
        else()
            message(FATAL_ERROR "No main source file found for executable")
        endif()
    endif()
    
    # Create the executable target
    add_executable(${EXEC_NAME} ${EXEC_SOURCES})
    
    # Mark as created globally
    set_property(GLOBAL PROPERTY NLINK_EXECUTABLE_DEFINED TRUE)
    set_property(GLOBAL PROPERTY NLINK_EXECUTABLE_TARGET ${EXEC_NAME})
    
    # Configure target properties
    set_target_properties(${EXEC_NAME} PROPERTIES
        OUTPUT_NAME "nlink"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        INSTALL_RPATH "$ORIGIN/../lib"
    )
    
    # Setup include directories
    target_include_directories(${EXEC_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${EXEC_INCLUDES}
    )
    
    # Link dependencies
    if(TARGET nlink_core)
        target_link_libraries(${EXEC_NAME} PRIVATE nlink_core)
    endif()
    
    if(TARGET nlink_cli_static)
        target_link_libraries(${EXEC_NAME} PRIVATE nlink_cli_static)
    endif()
    
    # Add thread support
    find_package(Threads REQUIRED)
    target_link_libraries(${EXEC_NAME} PRIVATE Threads::Threads)
    
    # Additional dependencies
    if(EXEC_DEPENDENCIES)
        target_link_libraries(${EXEC_NAME} PRIVATE ${EXEC_DEPENDENCIES})
    endif()
    
    # Static linking configuration
    if(EXEC_STATIC_LINKING)
        set_target_properties(${EXEC_NAME} PROPERTIES
            LINK_FLAGS "-static"
        )
        target_compile_definitions(${EXEC_NAME} PRIVATE NLINK_STATIC_BUILD)
    endif()
    
    # Profiling configuration
    if(EXEC_ENABLE_PROFILING)
        target_compile_options(${EXEC_NAME} PRIVATE -pg)
        target_link_options(${EXEC_NAME} PRIVATE -pg)
    endif()
    
    message(STATUS "════════════════════════════════════════════════════")
    message(STATUS "NexusLink Executable Configuration")
    message(STATUS "════════════════════════════════════════════════════")
    message(STATUS "Target name:       ${EXEC_NAME}")
    message(STATUS "Output name:       nlink")
    message(STATUS "Main source:       ${EXEC_SOURCES}")
    message(STATUS "Static linking:    ${EXEC_STATIC_LINKING}")
    message(STATUS "Profiling:         ${EXEC_ENABLE_PROFILING}")
    message(STATUS "════════════════════════════════════════════════════")
endfunction()

# ════════════════════════════════════════════════════════════
# EXECUTABLE QUERY FUNCTIONS
# ════════════════════════════════════════════════════════════

function(nlink_get_executable_target OUTPUT_VAR)
    get_property(EXEC_DEFINED GLOBAL PROPERTY NLINK_EXECUTABLE_DEFINED)
    if(NOT EXEC_DEFINED)
        set(${OUTPUT_VAR} "" PARENT_SCOPE)
        return()
    endif()
    
    get_property(EXEC_TARGET GLOBAL PROPERTY NLINK_EXECUTABLE_TARGET)
    set(${OUTPUT_VAR} ${EXEC_TARGET} PARENT_SCOPE)
endfunction()

# ════════════════════════════════════════════════════════════
# INSTALLATION HELPER
# ════════════════════════════════════════════════════════════

function(nlink_install_main_executable)
    # Get the executable target
    nlink_get_executable_target(EXEC_TARGET)
    
    if(NOT EXEC_TARGET)
        message(WARNING "No executable target defined for installation")
        return()
    endif()
    
    # Verify target exists
    if(NOT TARGET ${EXEC_TARGET})
        message(FATAL_ERROR "Executable target '${EXEC_TARGET}' does not exist")
    endif()
    
    # Install the executable
    install(
        TARGETS ${EXEC_TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT runtime
    )
    
    message(STATUS "Registered executable '${EXEC_TARGET}' for installation")
endfunction()