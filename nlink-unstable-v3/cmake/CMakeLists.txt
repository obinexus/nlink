# cmake/NexusLinkConfig.cmake.in
@PACKAGE_INIT@

include(CMakeFindDependencyMacro)
find_dependency(Threads REQUIRED)

# Define component imports
set(NLINK_LIBRARIES
    nlink::nlink_core
    nlink::nlink_cli_static
)

# Version information
set(NLINK_VERSION "@PROJECT_VERSION@")
set(NLINK_VERSION_MAJOR "@PROJECT_VERSION_MAJOR@")
set(NLINK_VERSION_MINOR "@PROJECT_VERSION_MINOR@")
set(NLINK_VERSION_PATCH "@PROJECT_VERSION_PATCH@")

# Component validation
include("${CMAKE_CURRENT_LIST_DIR}/nlink-targets.cmake")

# Verify core components
foreach(_comp nlink_core nlink_cli_static)
    if(NOT TARGET nlink::${_comp})
        set(NLINK_FOUND FALSE)
        set(NLINK_NOT_FOUND_MESSAGE "Required component '${_comp}' not found")
        return()
    endif()
endforeach()

set(NLINK_FOUND TRUE)