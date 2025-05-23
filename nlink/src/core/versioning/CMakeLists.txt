# Versioning component CMakeLists.txt

# Define source files for the versioning component
set(VERSIONING_SOURCES
    lazy_versioned.c
    missing_functions.c
    semver.c
)

# Define header files (for IDE integration)
set(VERSIONING_HEADERS
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/lazy_versioned.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/nexus_lazy_versioned.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/nexus_missing.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/nexus_version.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/semver.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/versioning/versioned_symbols.h
)

# Create library target
add_library(nexus_versioning ${VERSIONING_SOURCES} ${VERSIONING_HEADERS})

# Set include directories
target_include_directories(nexus_versioning
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set dependencies
target_link_libraries(nexus_versioning
    PUBLIC
        nexus_common
)

# Set properties
set_target_properties(nexus_versioning PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Conditional compile definitions
if(NEXUS_ENABLE_VERBOSE_LOGGING)
    target_compile_definitions(nexus_versioning PRIVATE NEXUS_VERBOSE_LOGGING=1)
endif()

if(NEXUS_USE_MEMORY_POOL)
    target_compile_definitions(nexus_versioning PRIVATE NEXUS_USE_MEMORY_POOL=1)
endif()

if(NEXUS_ENABLE_THREAD_SAFETY)
    target_compile_definitions(nexus_versioning PRIVATE NEXUS_THREAD_SAFETY=1)
endif()

# Install headers
install(FILES ${VERSIONING_HEADERS}
        DESTINATION include/nlink/core/versioning)

# Install library
install(TARGETS nexus_versioning
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Add to export targets
export(TARGETS nexus_versioning
       APPEND FILE ${CMAKE_BINARY_DIR}/NexusLinkTargets.cmake)