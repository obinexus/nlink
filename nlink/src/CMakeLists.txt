# NexusLink Master CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# Project metadata
project(NexusLink 
    VERSION 1.0.0
    DESCRIPTION "Dynamic Component Linkage System"
    LANGUAGES C
)

# Build configuration options
option(NEXUS_BUILD_TESTS "Build NexusLink tests" ON)
option(NEXUS_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(NEXUS_ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(NEXUS_ENABLE_VERBOSE_LOGGING "Enable detailed logging" OFF)
option(NEXUS_USE_MEMORY_POOL "Use custom memory management" ON)
option(NEXUS_ENABLE_THREAD_SAFETY "Enable thread-safe components" ON)
option(NEXUS_BUILD_SPS "Build Single-Pass System components" ON)
option(NEXUS_BUILD_MPS "Build Multi-Pass System components" ON)

# Configure compiler warnings
if(NEXUS_ENABLE_WARNINGS)
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
      -Wall
      -Wextra
      -Wpedantic
      -Werror=implicit-function-declaration
    )
  elseif(MSVC)
    add_compile_options(
      /W4
      /WX
    )
  endif()
endif()

# Configure Address Sanitizer
if(NEXUS_ENABLE_ASAN)
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
  else()
    message(WARNING "Address Sanitizer not supported for this compiler")
  endif()
endif()

# Set standard C version
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add position-independent code flag for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add preprocessor definitions
if(NEXUS_ENABLE_VERBOSE_LOGGING)
  add_definitions(-DNEXUS_VERBOSE_LOGGING=1)
endif()

if(NEXUS_USE_MEMORY_POOL)
  add_definitions(-DNEXUS_USE_MEMORY_POOL=1)
endif()

if(NEXUS_ENABLE_THREAD_SAFETY)
  add_definitions(-DNEXUS_THREAD_SAFETY=1)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Setup output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add components in dependency order
add_subdirectory(src/core/common)
add_subdirectory(src/core/symbols)
add_subdirectory(src/core/versioning)
add_subdirectory(src/core/minimizer)
add_subdirectory(src/core/pattern)
add_subdirectory(src/core/schema)

# Add SPS/MPS systems if enabled
if(NEXUS_BUILD_SPS)
  add_subdirectory(src/core/spsystem)
endif()

if(NEXUS_BUILD_MPS)
  add_subdirectory(src/core/mpsystem)
endif()

# Add core and CLI
add_subdirectory(src/core)
add_subdirectory(src/cli)

# Create the main library target
add_library(nlink SHARED
    src/nlink.c
)

# Set library properties
set_target_properties(nlink PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/nlink.h
)

# Collect all components for linking
set(NLINK_COMPONENTS
    nexus_core
    nexus_common
    nexus_symbols
    nexus_versioning
    nexus_minimizer
    nexus_pattern
    nexus_cli
)

# Add SPS/MPS components if enabled
if(NEXUS_BUILD_SPS)
    list(APPEND NLINK_COMPONENTS nexus_sps)
endif()

if(NEXUS_BUILD_MPS)
    list(APPEND NLINK_COMPONENTS nexus_mps)
endif()

# Link all components
target_link_libraries(nlink 
    PUBLIC ${NLINK_COMPONENTS}
    PRIVATE
        dl
        pthread
)

# Define the CLI executable
add_executable(nlink_cli src/cli/main.c)
target_link_libraries(nlink_cli PRIVATE nlink)
set_target_properties(nlink_cli PROPERTIES OUTPUT_NAME "nlink")

# Install rules
include(GNUInstallDirs)
install(TARGETS nlink nlink_cli
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ 
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
)

# Install schema files
install(DIRECTORY src/core/schema/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/nlink/schema
        FILES_MATCHING PATTERN "*.json")

# Build tests if enabled
if(NEXUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Create package config files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/NexusLinkConfig.cmake.in
    ${CMAKE_BINARY_DIR}/NexusLinkConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NexusLink
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/NexusLinkConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/NexusLinkConfig.cmake
        ${CMAKE_BINARY_DIR}/NexusLinkConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/NexusLink
)

# Print configuration summary
message(STATUS "NexusLink configuration:")
message(STATUS "  Version:              ${PROJECT_VERSION}")
message(STATUS "  Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "  C compiler:           ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Build tests:          ${NEXUS_BUILD_TESTS}")
message(STATUS "  Enable warnings:      ${NEXUS_ENABLE_WARNINGS}")
message(STATUS "  Enable ASAN:          ${NEXUS_ENABLE_ASAN}")
message(STATUS "  Memory pool:          ${NEXUS_USE_MEMORY_POOL}")
message(STATUS "  Thread safety:        ${NEXUS_ENABLE_THREAD_SAFETY}")
message(STATUS "  Build SPS:            ${NEXUS_BUILD_SPS}")
message(STATUS "  Build MPS:            ${NEXUS_BUILD_MPS}")
message(STATUS "  Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library directory:    ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Binary directory:     ${CMAKE_INSTALL_BINDIR}")
message(STATUS "  Include directory:    ${CMAKE_INSTALL_INCLUDEDIR}")