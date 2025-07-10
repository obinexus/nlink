# CMakeLists.txt for NexusLink pattern matching module
# Copyright © 2025 OBINexus Computing

# Get the directory name for the current module
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Define source files
set(MODULE_SOURCES
    pattern_matcher.c
)

# Define header files
set(MODULE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pattern_matching/pattern_matcher.h
)

# Add library target
nlink_build_component(
    NAME ${MODULE_NAME}
    SOURCES ${MODULE_SOURCES}
    HEADERS ${MODULE_HEADERS}
    TYPE "core"
    VERBOSE
)

# Register the component with NexusLink
add_dependencies(nlink_core_components nlink_${MODULE_NAME})
