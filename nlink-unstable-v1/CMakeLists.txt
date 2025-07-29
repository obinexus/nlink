cmake_minimum_required(VERSION 3.14)
project(NexusLink VERSION 0.1.0 LANGUAGES C)

# Global compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# Build options
option(NEXUS_BUILD_TESTS "Build NexusLink tests" ON)
option(NEXUS_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(NEXUS_ENABLE_ASAN "Enable Address Sanitizer" OFF)

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

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add components in dependency order
add_subdirectory(src/core/common)
add_subdirectory(src/core)
add_subdirectory(src/core/symbols)
add_subdirectory(src/core/versioning)
add_subdirectory(src/core/metadata)
add_subdirectory(src/core/minimizer)
add_subdirectory(src/cli)

# Build tests if enabled
if(NEXUS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "NexusLink configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Build tests: ${NEXUS_BUILD_TESTS}")
message(STATUS "  Enable warnings: ${NEXUS_ENABLE_WARNINGS}")
message(STATUS "  Enable ASAN: ${NEXUS_ENABLE_ASAN}")