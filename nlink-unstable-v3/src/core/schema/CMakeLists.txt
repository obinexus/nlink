# CMakeLists.txt for NexusLink schema module
# Copyright © 2025 OBINexus Computing

# Get the directory name for the current module
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Define source files
file(GLOB MODULE_SOURCES "*.c")
file(GLOB MODULE_HEADERS "*.h")

# Add library target with corrected parameter structure
nlink_build_component(
    COMPONENT ${MODULE_NAME}
    SOURCES ${MODULE_SOURCES}
    HEADERS ${MODULE_HEADERS}
    TYPE "core"
    VERBOSE
)

# Register with core components target
add_dependencies(nlink_core_components nlink_component_${MODULE_NAME})
