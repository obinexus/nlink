# Updated with tactical modules
# Copyright © 2025 OBINexus Computing

# Include tactical modules first to ensure dependencies are available
add_subdirectory(tactic)
add_subdirectory(type)
add_subdirectory(tokenizer)
add_subdirectory(parser)

# CMakeLists.txt for NexusLink core components
# This file coordinates the build of all core components

# Include common build system modules
include(${CMAKE_SOURCE_DIR}/cmake/BuildSystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ComponentSystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Hooks.cmake)

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# Discover all core components
file(GLOB CORE_COMPONENTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
set(CORE_COMPONENT_DIRS "")

foreach(COMPONENT ${CORE_COMPONENTS})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT})
    # Add to list of component directories
    list(APPEND CORE_COMPONENT_DIRS ${COMPONENT})
    
    # Register component with the build system
    if(NOT DEFINED NLINK_COMPONENT_SYSTEM_INCLUDED)
      # If component system isn't loaded yet, just add to our local list
      message(STATUS "Found core component: ${COMPONENT}")
    else()
      # Register with the component system
      nlink_register_component(${COMPONENT})
    endif()
  endif()
endforeach()

# Add each component subdirectory
foreach(COMPONENT_DIR ${CORE_COMPONENT_DIRS})
  add_subdirectory(${COMPONENT_DIR})
endforeach()

# Create unified core library
add_custom_target(nlink_unified_core ALL
  DEPENDS nlink_core_components
  COMMENT "Building unified core library"
)

# Create static unified library
get_property(STATIC_LIBS GLOBAL PROPERTY NLINK_STATIC_LIBRARIES)
if(STATIC_LIBS)
  add_library(nlink_core STATIC IMPORTED GLOBAL)
  set_property(TARGET nlink_core PROPERTY
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
  )
  
  # Command to create the static unified library
  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_AR} crs "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
            $<TARGET_OBJECTS:nlink_common_objects>
            $<$<TARGET_EXISTS:nlink_symbols_objects>:$<TARGET_OBJECTS:nlink_symbols_objects>>
            $<$<TARGET_EXISTS:nlink_versioning_objects>:$<TARGET_OBJECTS:nlink_versioning_objects>>
            $<$<TARGET_EXISTS:nlink_minimizer_objects>:$<TARGET_OBJECTS:nlink_minimizer_objects>>
            $<$<TARGET_EXISTS:nlink_pipeline_objects>:$<TARGET_OBJECTS:nlink_pipeline_objects>>
            $<$<TARGET_EXISTS:nlink_pattern_objects>:$<TARGET_OBJECTS:nlink_pattern_objects>>
            $<$<TARGET_EXISTS:nlink_mpsystem_objects>:$<TARGET_OBJECTS:nlink_mpsystem_objects>>
            $<$<TARGET_EXISTS:nlink_spsystem_objects>:$<TARGET_OBJECTS:nlink_spsystem_objects>>
            $<$<TARGET_EXISTS:nlink_metadata_objects>:$<TARGET_OBJECTS:nlink_metadata_objects>>
    DEPENDS ${STATIC_LIBS}
    COMMENT "Creating unified static library"
  )
  
  add_custom_target(nlink_unified_static
    DEPENDS "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
  )
  
  add_dependencies(nlink_unified_core nlink_unified_static)
endif()

# Create shared unified library
get_property(SHARED_LIBS GLOBAL PROPERTY NLINK_SHARED_LIBRARIES)
if(SHARED_LIBS)
  add_library(nlink_core_shared SHARED IMPORTED GLOBAL)
  set_property(TARGET nlink_core_shared PROPERTY
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/lib/libnlink_core.so"
  )
  
  # Command to create the shared unified library
  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/lib/libnlink_core.so"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_C_COMPILER} -shared -o "${CMAKE_BINARY_DIR}/lib/libnlink_core.so"
            -Wl,--whole-archive
            $<TARGET_OBJECTS:nlink_common_objects>
            $<$<TARGET_EXISTS:nlink_symbols_objects>:$<TARGET_OBJECTS:nlink_symbols_objects>>
            $<$<TARGET_EXISTS:nlink_versioning_objects>:$<TARGET_OBJECTS:nlink_versioning_objects>>
            $<$<TARGET_EXISTS:nlink_minimizer_objects>:$<TARGET_OBJECTS:nlink_minimizer_objects>>
            $<$<TARGET_EXISTS:nlink_pipeline_objects>:$<TARGET_OBJECTS:nlink_pipeline_objects>>
            $<$<TARGET_EXISTS:nlink_pattern_objects>:$<TARGET_OBJECTS:nlink_pattern_objects>>
            $<$<TARGET_EXISTS:nlink_mpsystem_objects>:$<TARGET_OBJECTS:nlink_mpsystem_objects>>
            $<$<TARGET_EXISTS:nlink_spsystem_objects>:$<TARGET_OBJECTS:nlink_spsystem_objects>>
            $<$<TARGET_EXISTS:nlink_metadata_objects>:$<TARGET_OBJECTS:nlink_metadata_objects>>
            -Wl,--no-whole-archive
            -lpthread -ldl
    DEPENDS ${SHARED_LIBS}
    COMMENT "Creating unified shared library"
  )
  
  add_custom_target(nlink_unified_shared
    DEPENDS "${CMAKE_BINARY_DIR}/lib/libnlink_core.so"
  )
  
  add_dependencies(nlink_unified_core nlink_unified_shared)
endif()

# Add the main NexusLink source file
add_library(nlink_main_objects OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/nlink.c)
target_include_directories(nlink_main_objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Set position independent code for shared library compatibility
set_property(TARGET nlink_main_objects PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Configure version header
configure_file(
    ${CMAKE_SOURCE_DIR}/templates/nlink_version.h.in
    ${CMAKE_BINARY_DIR}/include/nlink/core/nlink_version.h
    @ONLY
)

# Install the version header
install(
    FILES ${CMAKE_BINARY_DIR}/include/nlink/core/nlink_version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlink/core
    COMPONENT devel
)

# Execute post-build hooks
nlink_execute_post_build_hooks()

# Output build summary
message(STATUS "=== NexusLink Core Build Summary ===")
message(STATUS "Components: ${CORE_COMPONENT_DIRS}")
message(STATUS "Static libraries: ${STATIC_LIBS}")
message(STATUS "Shared libraries: ${SHARED_LIBS}")
message(STATUS "======================================")