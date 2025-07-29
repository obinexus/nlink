cmake_minimum_required(VERSION 3.12)

# Add basic usage example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/basic_usage/main.c")
    add_executable(nexus_basic_example basic_usage/main.c)
    target_link_libraries(nexus_basic_example PRIVATE nexus_core)
endif()

# Additional examples will be added as they are implemented
# Component System Example - TODO
# Language Toolchain Example - TODO
# Simple Schema Example - TODO