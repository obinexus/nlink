# NexusLink Test Framework Configuration
cmake_minimum_required(VERSION 3.16)

# Include testing framework
include(CTest)
enable_testing()

# Include NexusLink modules
include(${CMAKE_SOURCE_DIR}/cmake/TestFramework.cmake)

# Initialize component list for testing
set(TEST_COMPONENTS
    common
    pattern
    pipeline
    minimizer
    symbols
    versioning
)

# Add unit test subdirectory
add_subdirectory(unit)

# Only initialize package config if all components are built
set(MISSING_COMPONENTS)
foreach(COMPONENT ${TEST_COMPONENTS})
  if(NOT TARGET nlink_component_${COMPONENT})
    list(APPEND MISSING_COMPONENTS ${COMPONENT})
  endif()
endforeach()

if(MISSING_COMPONENTS)
  message(WARNING "Missing components for package config: ${MISSING_COMPONENTS}")
else()
  # Package configuration
  nlink_init_package_config(
    VERSION "${PROJECT_VERSION}"
    DESCRIPTION "NexusLink: Dynamic Component Linkage System"
    VENDOR "NexusLink Team"
  )
endif()
