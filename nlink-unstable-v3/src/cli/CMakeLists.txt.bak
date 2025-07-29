# CMakeLists.txt for NexusLink CLI components

# Include common build system modules
include(${CMAKE_SOURCE_DIR}/cmake/BuildSystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ComponentSystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Hooks.cmake)

# Create CLI specific target
add_custom_target(nlink_cli_components
  COMMENT "Building CLI components"
)

# Find all source files in this directory
file(GLOB CLI_SOURCES "*.c")
file(GLOB CLI_HEADERS "*.h")

# Find command sources
file(GLOB COMMAND_SOURCES "commands/*.c")
file(GLOB COMMAND_HEADERS "commands/*.h")

# Create CLI components object library
add_library(nlink_cli_objects OBJECT ${CLI_SOURCES})
target_include_directories(nlink_cli_objects PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create commands object library
add_library(nlink_commands_objects OBJECT ${COMMAND_SOURCES})
target_include_directories(nlink_commands_objects PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set position independent code for shared library compatibility
set_property(TARGET nlink_cli_objects PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

set_property(TARGET nlink_commands_objects PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

# Create static library for CLI
add_library(nlink_cli_static STATIC 
  $<TARGET_OBJECTS:nlink_cli_objects>
  $<TARGET_OBJECTS:nlink_commands_objects>
)

set_target_properties(nlink_cli_static PROPERTIES
  OUTPUT_NAME "nlink_cli"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Create shared library for CLI
if(BUILD_SHARED_LIBS)
  add_library(nlink_cli_shared SHARED 
    $<TARGET_OBJECTS:nlink_cli_objects>
    $<TARGET_OBJECTS:nlink_commands_objects>
  )
  
  set_target_properties(nlink_cli_shared PROPERTIES
    OUTPUT_NAME "nlink_cli"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  )
  
  # Link with necessary libraries
  target_link_libraries(nlink_cli_shared
    nlink_core_shared
    dl
    pthread
  )
endif()

# Link static library with dependencies
target_link_libraries(nlink_cli_static
  nlink_core_static
  dl
  pthread
)

# Add commands subdirectory if it has a CMakeLists.txt
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/commands/CMakeLists.txt")
  add_subdirectory(commands)
endif()

# Install CLI headers
install(
  FILES ${CLI_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlink/cli
  COMPONENT devel
)

# Install CLI library
install(
  TARGETS nlink_cli_static
  EXPORT nlink-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT devel
)

# Install CLI shared library if built
if(BUILD_SHARED_LIBS AND TARGET nlink_cli_shared)
  install(
    TARGETS nlink_cli_shared
    EXPORT nlink-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT runtime
  )
endif()

# Add CLI to all components target
add_dependencies(nlink_cli_components nlink_cli_static)
if(BUILD_SHARED_LIBS AND TARGET nlink_cli_shared)
  add_dependencies(nlink_cli_components nlink_cli_shared)
endif()

# Define compile-time flags for CLI support
target_compile_definitions(nlink_cli_objects PRIVATE NLINK_ENABLE_CLI)
target_compile_definitions(nlink_commands_objects PRIVATE NLINK_ENABLE_CLI)
target_compile_definitions(nlink_cli_static PRIVATE NLINK_ENABLE_CLI)
if(BUILD_SHARED_LIBS AND TARGET nlink_cli_shared)
  target_compile_definitions(nlink_cli_shared PRIVATE NLINK_ENABLE_CLI)
endif()

message(STATUS "CLI components configured")