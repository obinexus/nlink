# Minimizer module CMakeLists.txt

# Create nexus_minimizer library
add_library(nexus_minimizer
    minimizer.c
    automaton/nexus_automaton.c
)

# Create okpala_minimizer library
add_library(okpala_minimizer
    ast/okpala_ast.c
    automaton/okpala_automaton.c
    automaton/okpala_automaton_minimizer.c
)

# Add dependencies
target_link_libraries(nexus_minimizer
    PRIVATE nexus_common
)
# Add another dependency to nexus_minimizer if needed
target_link_libraries(okpala_minimizer
    PRIVATE nexus_minimizer
    PRIVATE nexus_common
)

# Set up include directories for these targets
target_include_directories(nexus_minimizer
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/nlink
)

target_include_directories(okpala_minimizer
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/nlink
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/nlink/core/minimizer/
        DESTINATION include/nlink/core/minimizer
        FILES_MATCHING PATTERN "*.h")

# Install libraries
install(TARGETS nexus_minimizer okpala_minimizer
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)