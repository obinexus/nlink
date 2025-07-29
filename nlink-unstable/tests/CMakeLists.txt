# NexusLink Test Suite
# CMake configuration for tests

# Enable testing
enable_testing()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/symbols
    ${CMAKE_SOURCE_DIR}/include/versioning
    ${CMAKE_SOURCE_DIR}/include/metadata
    ${CMAKE_SOURCE_DIR}/include/minimizer
)

# Common test library
add_library(test_common STATIC
    common/test_common.c
)

# Link with thread library (needed for some tests)
find_package(Threads REQUIRED)

# Find dl library for dynamic loading
find_library(DL_LIBRARY dl)

# Add subdirectories
add_subdirectory(unit)
add_subdirectory(integration)

# Output directory for test binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)