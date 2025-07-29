# Core module CMakeLists.txt

set(CORE_SOURCES
    nexus_core.c
    nexus_loader.c
)

# Create core library
add_library(nexus_core ${CORE_SOURCES})

# Add dependencies
target_link_libraries(nexus_core
    PUBLIC nexus_common
)

# Set up include directories for this target
target_include_directories(nexus_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/nlink
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/nlink/core/
        DESTINATION include/nlink/core
        FILES_MATCHING PATTERN "*.h"
        PATTERN "common" EXCLUDE)

# Install library
install(TARGETS nexus_core
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)