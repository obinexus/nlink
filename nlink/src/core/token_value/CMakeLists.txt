# CMakeLists.txt for NexusLink token module
# Copyright © 2025 OBINexus Computing

# Get the directory name for the current module
get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Define source files
set(MODULE_SOURCES
    token.c
    token_type.c
    nlink_token_system.c
)

# Define header files
set(MODULE_HEADERS
    token.h
    token_type.h
    nlink_token_system.h
)

# Add library target
nlink_build_component(
    NAME ${MODULE_NAME}
    SOURCES ${MODULE_SOURCES}
    HEADERS ${MODULE_HEADERS}
    TYPE "core"
    VERBOSE
)

# Define include directories
target_include_directories(nlink_${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../type
    ${CMAKE_CURRENT_SOURCE_DIR}/../tactic
    ${CMAKE_CURRENT_SOURCE_DIR}/../tokenizer
)

# Define dependencies
target_link_libraries(nlink_${MODULE_NAME} PRIVATE
    nlink_type
    nlink_tactic
    nlink_tokenizer
)

# Register the component with NexusLink
add_dependencies(nlink_core_components nlink_${MODULE_NAME})

# Install headers
install(FILES ${MODULE_HEADERS} DESTINATION include/nlink/${MODULE_NAME})