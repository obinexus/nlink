
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was NexusLinkConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(Threads REQUIRED)

# Our library dependencies (contains definitions for IMPORTED targets)
include("${CMAKE_CURRENT_LIST_DIR}/NexusLinkTargets.cmake")

# Define the component targets
set(NEXUS_LINK_COMPONENTS
    nlink
    nexus_core
    nexus_common
    nexus_symbols
    nexus_versioning
    nexus_minimizer
    nexus_pattern
    nexus_cli
)

# Check if SPS/MPS components are available
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/NexusLinkSPSTargets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/NexusLinkSPSTargets.cmake")
    list(APPEND NEXUS_LINK_COMPONENTS nexus_sps)
    set(NexusLink_SPS_FOUND TRUE)
else()
    set(NexusLink_SPS_FOUND FALSE)
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/NexusLinkMPSTargets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/NexusLinkMPSTargets.cmake")
    list(APPEND NEXUS_LINK_COMPONENTS nexus_mps)
    set(NexusLink_MPS_FOUND TRUE)
else()
    set(NexusLink_MPS_FOUND FALSE)
endif()

# Create imported targets for all components
foreach(_comp ${NEXUS_LINK_COMPONENTS})
    if(NOT TARGET ${_comp})
        add_library(${_comp} UNKNOWN IMPORTED)
        set_target_properties(${_comp} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PACKAGE_PREFIX_DIR}/include"
        )
    endif()
endforeach()

# Check if all requested components are found
check_required_components(NexusLink)

# Helper macro to check if a component is available
macro(nexus_check_component _comp _found_var)
    list(FIND NEXUS_LINK_COMPONENTS ${_comp} _index)
    if(_index GREATER -1)
        set(${_found_var} TRUE)
    else()
        set(${_found_var} FALSE)
    endif()
endmacro()

# Set version variables
set(NexusLink_VERSION 1.0.0)
set(NexusLink_VERSION_MAJOR 1)
set(NexusLink_VERSION_MINOR 0)
set(NexusLink_VERSION_PATCH 0)

# Export variables for schema and data directories
set(NexusLink_SCHEMA_DIR "${PACKAGE_PREFIX_DIR}/share/nlink/schema")

# Log configuration
if(NOT NexusLink_FIND_QUIETLY)
    message(STATUS "Found NexusLink version ${NexusLink_VERSION}")
    message(STATUS "  Components:")
    foreach(_comp ${NEXUS_LINK_COMPONENTS})
        message(STATUS "    * ${_comp}")
    endforeach()
    message(STATUS "  SPS system: ${NexusLink_SPS_FOUND}")
    message(STATUS "  MPS system: ${NexusLink_MPS_FOUND}")
endif()

####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was NexusLinkConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(Threads REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/nlink-targets.cmake")

# Define exported component variables
set(NLINK_LIBRARIES nlink_core)
set(NLINK_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include/nlink")

# Check that components are available
set(_NLINK_COMPONENT_NAMES
    common
    symbols
    versioning
    minimizer
    pipeline
)

foreach(_comp ${_NLINK_COMPONENT_NAMES})
    if(NOT TARGET nlink::nlink_${_comp}_static)
        set(NLINK_FOUND FALSE)
        set(NLINK_NOT_FOUND_MESSAGE "Required NexusLink component '${_comp}' is missing.")
        return()
    endif()
endforeach()

# Set found flag
set(NLINK_FOUND TRUE)
