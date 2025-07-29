# tatit_build/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(tatit_build LANGUAGES C)

# Import build system utilities
include(${CMAKE_SOURCE_DIR}/cmake/ComponentSystem.cmake)

# Register component with build system
file(GLOB COMPONENT_SOURCES "${CMAKE_SOURCE_DIR}/src/core/tatit/*.c")
nlink_register_component(
    NAME tatit
    SOURCES ${COMPONENT_SOURCES}
    ENABLE_STATIC TRUE
)
