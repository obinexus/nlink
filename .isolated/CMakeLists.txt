cmake_minimum_required(VERSION 3.10)
project(NexusLink VERSION 1.0.0 LANGUAGES C CXX)

# Project settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Include custom modules
include(cmake/CompilerFlags.cmake)
include(cmake/Features.cmake)
include(cmake/Testing.cmake)
include(cmake/Installation.cmake)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(test)

# Export configuration
include(cmake/NexusLinkConfig.cmake)
