# Core components CMakeLists.txt for NexusLink
# Copyright © 2025 OBINexus Computing

# Execute pre-build hooks
nlink_execute_pre_build_hooks()

# Define core components
set(NLINK_CORE_COMPONENTS "")
file(GLOB COMPONENT_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")

# Extract valid component directories (filter out CMakeFiles and other special directories)
foreach(DIR ${COMPONENT_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}" AND
       NOT "${DIR}" STREQUAL "CMakeFiles" AND
       NOT "${DIR}" STREQUAL "cmake" AND
       NOT "${DIR}" MATCHES "^\\.")
        
        message(STATUS "Found core component: ${DIR}")
        list(APPEND NLINK_CORE_COMPONENTS ${DIR})
    endif()
endforeach()

# Process components with unique build directories
foreach(COMPONENT ${NLINK_CORE_COMPONENTS})
    # Define unique binary build directory for each component
    set(COMPONENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT}_build")
    
    # Add subdirectory with specified binary directory to prevent collisions
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}" 
                         "${COMPONENT_BINARY_DIR}")
    else()
        message(WARNING "Component directory ${COMPONENT} missing CMakeLists.txt")
    endif()
endforeach()


# Create unified core library
add_custom_target(nlink_unified_core ALL
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
            # Additional component object references
    DEPENDS ${STATIC_LIBS}
    COMMENT "Creating unified static library"
  )
  
  add_custom_target(nlink_unified_static
    DEPENDS "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
  )
  
  add_dependencies(nlink_unified_core nlink_unified_static)
endif()

# Set global component list property
set_property(GLOBAL PROPERTY NLINK_CORE_COMPONENTS ${NLINK_CORE_COMPONENTS})


# Create unified core library
add_custom_target(nlink_unified_core ALL
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
            # Additional component object references
    DEPENDS ${STATIC_LIBS}
    COMMENT "Creating unified static library"
  )
  
  add_custom_target(nlink_unified_static
    DEPENDS "${CMAKE_BINARY_DIR}/lib/libnlink_core.a"
  )
  
  add_dependencies(nlink_unified_core nlink_unified_static)
endif()
# Execute post-build hooks
nlink_execute_post_build_hooks()

# Build summary
message(STATUS "=== NexusLink Core Build Summary ===")
message(STATUS "Components: ${NLINK_CORE_COMPONENTS}")

# Get lists of static and shared libraries
get_property(STATIC_LIBS GLOBAL PROPERTY NLINK_STATIC_LIBRARIES)
get_property(SHARED_LIBS GLOBAL PROPERTY NLINK_SHARED_LIBRARIES)

message(STATUS "Static libraries: ${STATIC_LIBS}")
message(STATUS "Shared libraries: ${SHARED_LIBS}")
message(STATUS "======================================")
