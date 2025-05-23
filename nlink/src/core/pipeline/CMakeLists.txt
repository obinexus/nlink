# Pipeline component CMakeLists.txt

# Define pipeline sources
set(PIPELINE_SOURCES
    nlink_pipeline.c
    pipeline_executor.c
    pipeline_optimizer.c
    pipeline_registry.c
    pipeline_scheduler.c
)

# Define pipeline headers
set(PIPELINE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/nlink_pipeline.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/pipeline_executor.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/pipeline_optimizer.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/pipeline_registry.h
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/pipeline_scheduler.h
)

# Create pipeline library
add_library(nexus_pipeline SHARED ${PIPELINE_SOURCES} ${PIPELINE_HEADERS})

# Setup include directories
target_include_directories(nexus_pipeline
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Add dependencies
target_link_libraries(nexus_pipeline
    PUBLIC
        nexus_common
    PRIVATE
        nexus_minimizer
        ${CMAKE_DL_LIBS}  # For dlopen/dlclose
        pthread  # For thread safety
)

# Set properties for shared library
set_target_properties(nexus_pipeline PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden  # Hide internal symbols
    POSITION_INDEPENDENT_CODE ON
)

# Define preprocessor macros
target_compile_definitions(nexus_pipeline PRIVATE
    NLINK_BUILDING_PIPELINE_LIB  # For proper symbol export
)

# Install headers
install(FILES ${PIPELINE_HEADERS}
        DESTINATION include/nlink/core/pipeline)

# Install library
install(TARGETS nexus_pipeline
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Add to export targets
export(TARGETS nexus_pipeline
       APPEND FILE ${CMAKE_BINARY_DIR}/NexusLinkTargets.cmake)

# Pipeline Detector module for NexusLink
# Provides pipeline detection and automatic optimization functionality

# Define source files for the pipeline detector module
set(PIPELINE_DETECTOR_SOURCES
    pipeline_detector.c
)

# Define header files (for IDE integration)
set(PIPELINE_DETECTOR_HEADERS
    ${CMAKE_SOURCE_DIR}/include/nlink/core/pipeline/pipeline_detect.h
)

# Create the pipeline detector library
add_library(nexus_pipeline_detector ${PIPELINE_DETECTOR_SOURCES} ${PIPELINE_DETECTOR_HEADERS})

# Set include directories
target_include_directories(nexus_pipeline_detector
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Add dependencies
target_link_libraries(nexus_pipeline_detector
    PUBLIC
        nexus_core
        nexus_common
    PRIVATE
        nexus_minimizer
        nexus_pipeline
        nexus_spsystem
        nexus_mpsystem
)

# Set compiler options
target_compile_options(nexus_pipeline_detector PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g>
)

# Install headers
install(FILES ${PIPELINE_DETECTOR_HEADERS}
        DESTINATION include/nlink/core/pipeline)

# Install library
install(TARGETS nexus_pipeline_detector
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Add to export targets
export(TARGETS nexus_pipeline_detector
       APPEND FILE ${CMAKE_BINARY_DIR}/NexusLinkTargets.cmake)