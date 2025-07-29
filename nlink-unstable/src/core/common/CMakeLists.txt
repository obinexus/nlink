# Common component CMakeLists.txt

set(COMMON_SOURCES
    result.c
    types.c
)

# Create common library
add_library(nexus_common ${COMMON_SOURCES})

# Set up include directories for this target and dependent targets
target_include_directories(nexus_common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/nlink
    INTERFACE
        ${CMAKE_SOURCE_DIR}/include/nlink
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/nlink/core/common/
        DESTINATION include/nlink/core/common
        FILES_MATCHING PATTERN "*.h")

# Install library
install(TARGETS nexus_common
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)