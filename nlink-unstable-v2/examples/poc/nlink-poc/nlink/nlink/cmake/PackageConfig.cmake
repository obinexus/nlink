# PackageConfig.cmake
# Package configuration and installation utilities for NexusLink
#
# This module provides functions for configuring and installing
# NexusLink components and generating package files.

if(DEFINED NLINK_PACKAGE_CONFIG_INCLUDED)
  return()
endif()
set(NLINK_PACKAGE_CONFIG_INCLUDED TRUE)

include(CMakeParseArguments)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Configure package installation paths
set(NLINK_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
set(NLINK_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
set(NLINK_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/nlink")
set(NLINK_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/nlink")

# Initialize package configuration
function(nlink_init_package_config)
  cmake_parse_arguments(
    PKG_INIT
    ""
    "VERSION;DESCRIPTION;VENDOR"
    "COMPONENTS"
    ${ARGN}
  )
  
  # Set default version if not provided
  if(NOT PKG_INIT_VERSION)
    set(PKG_INIT_VERSION "1.0.0")
  endif()
  
  # Set default description if not provided
  if(NOT PKG_INIT_DESCRIPTION)
    set(PKG_INIT_DESCRIPTION "NexusLink: Dynamic Component Linkage System")
  endif()
  
  # Set default vendor if not provided
  if(NOT PKG_INIT_VENDOR)
    set(PKG_INIT_VENDOR "NexusLink Team")
  endif()
  
  # Set default components if not provided
  if(NOT PKG_INIT_COMPONENTS)
    get_property(PKG_INIT_COMPONENTS GLOBAL PROPERTY NLINK_REGISTERED_COMPONENTS)
  endif()
  
  # Configure CPack variables
  set(CPACK_PACKAGE_NAME "nlink" PARENT_SCOPE)
  set(CPACK_PACKAGE_VENDOR "${PKG_INIT_VENDOR}" PARENT_SCOPE)
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PKG_INIT_DESCRIPTION}" PARENT_SCOPE)
  set(CPACK_PACKAGE_VERSION "${PKG_INIT_VERSION}" PARENT_SCOPE)
  
  # Extract version components
  if(PKG_INIT_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
    set(PKG_INIT_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(PKG_INIT_VERSION_MINOR ${CMAKE_MATCH_2})
    set(PKG_INIT_VERSION_PATCH ${CMAKE_MATCH_3})
  else()
    set(PKG_INIT_VERSION_MAJOR 1)
    set(PKG_INIT_VERSION_MINOR 0)
    set(PKG_INIT_VERSION_PATCH 0)
  endif()
  
  set(CPACK_PACKAGE_VERSION_MAJOR "${PKG_INIT_VERSION_MAJOR}" PARENT_SCOPE)
  set(CPACK_PACKAGE_VERSION_MINOR "${PKG_INIT_VERSION_MINOR}" PARENT_SCOPE)
  set(CPACK_PACKAGE_VERSION_PATCH "${PKG_INIT_VERSION_PATCH}" PARENT_SCOPE)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "nlink ${PKG_INIT_VERSION}" PARENT_SCOPE)
  
  # Configure binary package generators
  set(CPACK_GENERATOR "TGZ;ZIP" PARENT_SCOPE)
  
  # Configure source package generators
  set(CPACK_SOURCE_GENERATOR "TGZ;ZIP" PARENT_SCOPE)
  set(CPACK_SOURCE_IGNORE_FILES
      "/\\\\.git/"
      "/\\\\.github/"
      "/build/"
      "/\\\\.vscode/"
      "/\\\\.idea/"
      "/obj/"
      "\\\\.o$"
      "\\\\.a$"
      "\\\\.so$"
      PARENT_SCOPE
  )
  
  # Include CPack module
  include(CPack)
  
  # Register install targets for each component
  foreach(COMPONENT ${PKG_INIT_COMPONENTS})
    nlink_register_component_install(${COMPONENT})
  endforeach()
  
  # Configure package config file
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NexusLinkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NexusLinkConfig.cmake"
    INSTALL_DESTINATION "${NLINK_INSTALL_CMAKEDIR}"
    PATH_VARS
      NLINK_INSTALL_INCLUDEDIR
      NLINK_INSTALL_LIBDIR
  )
  
  # Create version file
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NexusLinkConfigVersion.cmake"
    VERSION "${PKG_INIT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )
  
  # Install package configuration files
  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/NexusLinkConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/NexusLinkConfigVersion.cmake"
    DESTINATION "${NLINK_INSTALL_CMAKEDIR}"
    COMPONENT devel
  )
  
  # Install targets export
  install(
    EXPORT nlink-targets
    NAMESPACE nlink::
    DESTINATION "${NLINK_INSTALL_CMAKEDIR}"
    COMPONENT devel
  )
  
  nlink_log(
    STATUS
    MESSAGE "Package configuration initialized with version ${PKG_INIT_VERSION}"
  )
endfunction()

# Register component for installation
function(nlink_register_component_install COMPONENT_NAME)
  # Ensure component is valid
  get_target_property(COMPONENT_SOURCES "nlink_${COMPONENT_NAME}" COMPONENT_SOURCES)
  if(NOT COMPONENT_SOURCES)
    nlink_log(
      WARNING
      MESSAGE "Cannot register component ${COMPONENT_NAME} for installation: Component not found"
    )
    return()
  endif()
  
  # Component include directory
  set(COMPONENT_INCLUDE_DIR "${NLINK_INCLUDE_DIR}/nlink/core/${COMPONENT_NAME}")
  
  # Install headers
  install(
    DIRECTORY "${COMPONENT_INCLUDE_DIR}/"
    DESTINATION "${NLINK_INSTALL_INCLUDEDIR}/core/${COMPONENT_NAME}"
    FILES_MATCHING PATTERN "*.h"
    COMPONENT devel
  )
  
  # Install library target
  if(TARGET nlink_${COMPONENT_NAME}_static)
    install(
      TARGETS nlink_${COMPONENT_NAME}_static
      EXPORT nlink-targets
      ARCHIVE DESTINATION "${NLINK_INSTALL_LIBDIR}"
      LIBRARY DESTINATION "${NLINK_INSTALL_LIBDIR}"
      RUNTIME DESTINATION "${NLINK_INSTALL_BINDIR}"
      INCLUDES DESTINATION "${NLINK_INSTALL_INCLUDEDIR}"
      COMPONENT devel
    )
  endif()
  
  nlink_log(
    STATUS
    MESSAGE "Registered component ${COMPONENT_NAME} for installation"
  )
endfunction()

# Create a release package
function(nlink_create_package)
  cmake_parse_arguments(
    PKG
    "SOURCE"
    "OUTPUT_DIR"
    "COMPONENTS"
    ${ARGN}
  )
  
  # Set default output directory if not provided
  if(NOT PKG_OUTPUT_DIR)
    set(PKG_OUTPUT_DIR "${NLINK_BUILD_DIR}/packages")
  endif()
  
  # Create output directory if it doesn't exist
  nlink_ensure_directory(${PKG_OUTPUT_DIR})
  
  # Build package command
  set(PACKAGE_CMD "${CMAKE_COMMAND}")
  list(APPEND PACKAGE_CMD "--build" "${CMAKE_BINARY_DIR}")
  
  if(PKG_SOURCE)
    list(APPEND PACKAGE_CMD "--target" "package_source")
  else()
    list(APPEND PACKAGE_CMD "--target" "package")
  endif()
  
  # Execute package command
  execute_process(
    COMMAND ${PACKAGE_CMD}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    RESULT_VARIABLE PACKAGE_RESULT
  )
  
  if(NOT PACKAGE_RESULT EQUAL 0)
    nlink_log(
      ERROR
      MESSAGE "Failed to create package"
    )
    return()
  endif()
  
  # Move package files to the output directory
  file(GLOB PACKAGE_FILES "${CMAKE_BINARY_DIR}/nlink-*.tar.gz" "${CMAKE_BINARY_DIR}/nlink-*.zip")
  
  foreach(PACKAGE_FILE ${PACKAGE_FILES})
    get_filename_component(PACKAGE_FILENAME "${PACKAGE_FILE}" NAME)
    file(COPY "${PACKAGE_FILE}" DESTINATION "${PKG_OUTPUT_DIR}")
    nlink_log(
      STATUS
      MESSAGE "Created package: ${PKG_OUTPUT_DIR}/${PACKAGE_FILENAME}"
    )
  endforeach()
endfunction()

# Install unified header
function(nlink_install_unified_header)
  cmake_parse_arguments(
    HEADER
    ""
    "SOURCE"
    ""
    ${ARGN}
  )
  
  # Set default source if not provided
  if(NOT HEADER_SOURCE)
    set(HEADER_SOURCE "${NLINK_BUILD_DIR}/include/nlink.h")
  endif()
  
  # Install the header
  install(
    FILES "${HEADER_SOURCE}"
    DESTINATION "${NLINK_INSTALL_INCLUDEDIR}"
    COMPONENT devel
  )
  
  # Also install at the root include directory
  install(
    FILES "${HEADER_SOURCE}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT devel
  )
  
  nlink_log(
    STATUS
    MESSAGE "Registered unified header for installation"
  )
endfunction()

# Install development components
function(nlink_install_development_components)
  # Install all headers
  install(
    DIRECTORY "${NLINK_INCLUDE_DIR}/"
    DESTINATION "${NLINK_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
    COMPONENT devel
  )
  
  # Install static library
  if(TARGET nlink_static)
    install(
      TARGETS nlink_static
      EXPORT nlink-targets
      ARCHIVE DESTINATION "${NLINK_INSTALL_LIBDIR}"
      LIBRARY DESTINATION "${NLINK_INSTALL_LIBDIR}"
      RUNTIME DESTINATION "${NLINK_INSTALL_BINDIR}"
      INCLUDES DESTINATION "${NLINK_INSTALL_INCLUDEDIR}"
      COMPONENT devel
    )
  endif()
  
  # Install shared library
  if(TARGET nlink_shared)
    install(
      TARGETS nlink_shared
      EXPORT nlink-targets
      ARCHIVE DESTINATION "${NLINK_INSTALL_LIBDIR}"
      LIBRARY DESTINATION "${NLINK_INSTALL_LIBDIR}"
      RUNTIME DESTINATION "${NLINK_INSTALL_BINDIR}"
      INCLUDES DESTINATION "${NLINK_INSTALL_INCLUDEDIR}"
      COMPONENT devel
    )
  endif()
  
  nlink_log(
    STATUS
    MESSAGE "Registered development components for installation"
  )
endfunction()

# Install executable
function(nlink_install_executable)
  cmake_parse_arguments(
    EXEC
    ""
    "TARGET"
    ""
    ${ARGN}
  )
  
  # Set default target if not provided
  if(NOT EXEC_TARGET)
    set(EXEC_TARGET "nlink")
  endif()
  
  # Install executable
  install(
    TARGETS ${EXEC_TARGET}
    EXPORT nlink-targets
    RUNTIME DESTINATION "${NLINK_INSTALL_BINDIR}"
    COMPONENT runtime
  )
  
  nlink_log(
    STATUS
    MESSAGE "Registered executable ${EXEC_TARGET} for installation"
  )
endfunction()
