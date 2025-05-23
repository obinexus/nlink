# SPSystem - Single-Pass System module for NexusLink
# Provides pipeline management with linear data flow

# Source files for the spsystem module
set(SPS_SOURCES
    sps_config.c
    sps_dependency.c
    sps_lifecycle.c
    sps_pipeline.c
    sps_stream.c
)

# Header files (for IDE integration)
set(SPS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/sps_config.h
    ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/sps_dependency.h
    ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/sps_lifecycle.h
    ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/sps_pipeline.h
    ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/sps_stream.h
)

# Create the spsystem library
add_library(nexus_spsystem ${SPS_SOURCES})

# Setup include directories
target_include_directories(nexus_spsystem
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(nexus_spsystem
    PUBLIC
        nexus_common
    PRIVATE
        ${CMAKE_DL_LIBS}  # For dynamic loading
        pthread  # For thread safety
)

# Set properties for library
set_target_properties(nexus_spsystem PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden  # Hide internal symbols
    POSITION_INDEPENDENT_CODE ON
)

# Define preprocessor macros
target_compile_definitions(nexus_spsystem PRIVATE
    NLINK_BUILDING_SPSYSTEM_LIB  # For proper symbol export
)

# Installation rules
install(TARGETS nexus_spsystem
    EXPORT NexusLinkTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${SPS_HEADERS}
    DESTINATION include/nlink/spsystem
)

# Add to export targets
export(TARGETS nexus_spsystem
    APPEND FILE ${CMAKE_BINARY_DIR}/NexusLinkTargets.cmake
)