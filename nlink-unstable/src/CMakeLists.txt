# NexusLink Source CMakeLists.txt
# Define source files by component

# Find required packages
find_package(Threads REQUIRED)

# Common library for shared code
add_library(nexus_common SHARED
    common/types.c
    common/result.c
)
target_include_directories(nexus_common PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Core functionality
add_library(nexus_core SHARED
    core/nexus_core.c
    core/nexus_loader.c
    core/json.c
)
target_link_libraries(nexus_core PUBLIC 
    nexus_common
    ${DL_LIBRARY}
)

# Symbol management
add_library(nexus_symbols SHARED
    symbols/nexus_symbols.c
    symbols/versioned_symbols.c
)
target_link_libraries(nexus_symbols PUBLIC 
    nexus_core
    nexus_common
)

# Versioning system
add_library(nexus_versioning SHARED
    versioning/semver.c
    versioning/lazy_versioned.c
)
target_link_libraries(nexus_versioning PUBLIC 
    nexus_common
    nexus_symbols
)

# Metadata handling
add_library(nexus_metadata SHARED
    metadata/enhanced_metadata.c
)
target_link_libraries(nexus_metadata PUBLIC 
    nexus_core
    nexus_versioning
)

# Minimizer and automaton functionality
add_library(nexus_minimizer SHARED
    minimizer/nexus_automaton.c
    minimizer/okpala_ast.c
    minimizer/okpala_automaton.c
)
target_link_libraries(nexus_minimizer PUBLIC 
    nexus_common
)

# Example cold component
add_library(cold SHARED
    core/cold/cold.c
)

# Main NexusLink library combining all components
add_library(nlink SHARED)
target_link_libraries(nlink PUBLIC
    nexus_common
    nexus_core
    nexus_symbols
    nexus_versioning
    nexus_metadata
    nexus_minimizer
    ${DL_LIBRARY}
    Threads::Threads
)

# Install libraries
install(
    TARGETS 
        nexus_common
        nexus_core
        nexus_symbols
        nexus_versioning
        nexus_metadata
        nexus_minimizer
        nlink
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)