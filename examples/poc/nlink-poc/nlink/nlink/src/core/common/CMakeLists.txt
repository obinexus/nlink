# CMakeLists.txt for NexusLink common component

# Get component name from directory
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Execute pre-component hooks
nlink_execute_pre_component_hooks(${COMPONENT_NAME})

# Find all source files in this component
file(GLOB COMPONENT_SOURCES "*.c")
file(GLOB COMPONENT_HEADERS "*.h")

# Check for subdirectories with additional sources
file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
foreach(SUBDIR ${SUBDIRS})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR})
    # Create subdirectory in object directory
    get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
    file(MAKE_DIRECTORY "${OBJ_DIR}/core/${COMPONENT_NAME}/${SUBDIR}")
    
    # Find sources in subdirectory
    file(GLOB SUBDIR_SOURCES "${SUBDIR}/*.c")
    list(APPEND COMPONENT_SOURCES ${SUBDIR_SOURCES})
    
    file(GLOB SUBDIR_HEADERS "${SUBDIR}/*.h")
    list(APPEND COMPONENT_HEADERS ${SUBDIR_HEADERS})
    
    # Add subdirectory for nested building if it has a CMakeLists.txt
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/CMakeLists.txt")
      add_subdirectory(${SUBDIR})
    endif()
  endif()
endforeach()

# Create object library
add_library(nlink_${COMPONENT_NAME}_objects OBJECT ${COMPONENT_SOURCES})

# Set include directories
target_include_directories(nlink_${COMPONENT_NAME}_objects PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set position independent code for shared library compatibility
set_property(TARGET nlink_${COMPONENT_NAME}_objects PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

# Create static library
add_library(nlink_${COMPONENT_NAME}_static STATIC
  $<TARGET_OBJECTS:nlink_${COMPONENT_NAME}_objects>
)

# Set output properties for static library
set_target_properties(nlink_${COMPONENT_NAME}_static PROPERTIES
  OUTPUT_NAME "nlink_${COMPONENT_NAME}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Create shared library if shared libs are enabled
if(BUILD_SHARED_LIBS)
  add_library(nlink_${COMPONENT_NAME}_shared SHARED
    $<TARGET_OBJECTS:nlink_${COMPONENT_NAME}_objects>
  )
  
  # Set output properties for shared library
  set_target_properties(nlink_${COMPONENT_NAME}_shared PROPERTIES
    OUTPUT_NAME "nlink_${COMPONENT_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  )
  
  # Link dependencies for shared library
  target_link_libraries(nlink_${COMPONENT_NAME}_shared
    pthread
    dl
  )
  
  # Register this shared library
  set_property(GLOBAL APPEND PROPERTY NLINK_SHARED_LIBRARIES nlink_${COMPONENT_NAME}_shared)
endif()

# Register this static library
set_property(GLOBAL APPEND PROPERTY NLINK_STATIC_LIBRARIES nlink_${COMPONENT_NAME}_static)

# Extract object files to component object directory
get_property(OBJ_DIR GLOBAL PROPERTY NLINK_OBJ_DIR)
add_custom_command(
  TARGET nlink_${COMPONENT_NAME}_objects
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OBJ_DIR}/core/${COMPONENT_NAME}
  COMMAND ${CMAKE_COMMAND} -E echo "Copying ${COMPONENT_NAME} object files to ${OBJ_DIR}/core/${COMPONENT_NAME}"
  COMMAND ${CMAKE_COMMAND} -E $<$<BOOL:$<TARGET_PROPERTY:nlink_${COMPONENT_NAME}_objects,SOURCES>>:copy_directory>
          $<TARGET_PROPERTY:nlink_${COMPONENT_NAME}_objects,OBJECT_DIR> ${OBJ_DIR}/core/${COMPONENT_NAME}
  COMMENT "Extracting ${COMPONENT_NAME} object files"
)

# Component target for independent building
add_custom_target(${COMPONENT_NAME}
  DEPENDS nlink_${COMPONENT_NAME}_static
  COMMENT "Building ${COMPONENT_NAME} component"
)

# Add to core components target
add_dependencies(nlink_core_components ${COMPONENT_NAME})

# Install component headers
install(
  FILES ${COMPONENT_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlink/core/${COMPONENT_NAME}
  COMPONENT devel
)

# Install component library
install(
  TARGETS nlink_${COMPONENT_NAME}_static
  EXPORT nlink-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT devel
)

if(BUILD_SHARED_LIBS AND TARGET nlink_${COMPONENT_NAME}_shared)
  install(
    TARGETS nlink_${COMPONENT_NAME}_shared
    EXPORT nlink-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT runtime
  )
endif()

# Execute post-component hooks
nlink_execute_post_component_hooks(${COMPONENT_NAME})

message(STATUS "Configured component: ${COMPONENT_NAME}")
# Define specific source files instead of globbing
set(COMPONENT_SOURCES
  nexus_core.c
  nexus_loader.c
  result.c
  types.c
  nexus_error.c
  nexus_exception.c
  nexus_result.c
)

# Define specific header files instead of globbing
set(COMPONENT_HEADERS
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/nexus_core.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/nexus_loader.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/result.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/types.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/nexus_error.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/nexus_exception.h
  ${CMAKE_SOURCE_DIR}/include/nlink/core/common/nexus_result.h
)

# Remove the original GLOB commands
# file(GLOB COMPONENT_SOURCES "*.c")
# file(GLOB COMPONENT_HEADERS "*.h")
