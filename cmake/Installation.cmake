# Installation configuration

include(GNUInstallDirs)

# Library installation
install(TARGETS nlink nlink_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Header installation
install(DIRECTORY include/nlink
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Binary installation
install(TARGETS nlink_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CMake config installation
install(FILES cmake/NexusLinkConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NexusLink
)
