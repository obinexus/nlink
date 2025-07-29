# CMakeLists.txt for NexusLink config module
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

# Consolidate object files
foreach(SOURCE_FILE ${MODULE_SOURCES})
    get_filename_component(OBJECT_NAME ${SOURCE_FILE} NAME_WE)
    set(OBJECT_FILE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/nlink_component_${MODULE_NAME}.dir/${OBJECT_NAME}.o")
    
    if(EXISTS "${OBJECT_FILE}")
        file(COPY "${OBJECT_FILE}" 
             DESTINATION "${NLINK_OBJ_DIR}/core/${MODULE_NAME}")
    endif()
endforeach()
