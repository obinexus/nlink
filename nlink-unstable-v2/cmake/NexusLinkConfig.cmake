# NexusLink CMake Configuration Export

@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/NexusLinkTargets.cmake")

# Provide old-style variables
set(NEXUSLINK_LIBRARIES nlink)
set(NEXUSLINK_INCLUDE_DIRS "@PACKAGE_CMAKE_INSTALL_INCLUDEDIR@")

check_required_components(NexusLink)
