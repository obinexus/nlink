# Core library sources
set(NEXUS_CORE_SOURCES
    nlink.c
    common/result.c
    common/types.c
    common/nexus_core.c
    minimizer/nexus_minimizer.c
    minimizer/okpala_automaton.c
    symbols/nexus_symbols.c
    versioning/nexus_version.c
)

# Create core library
add_library(nexus_core SHARED ${NEXUS_CORE_SOURCES})

# Install library
install(TARGETS nexus_core
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

        # Core test sources
        set(CORE_TEST_SOURCES
            test_nlink.c
            test_minimizer.c
            test_symbols.c
            test_version.c
        )

        # Create test executables
        foreach(test_source ${CORE_TEST_SOURCES})
            # Extract test name from filename
            get_filename_component(test_name ${test_source} NAME_WE)
            
            # Create test executable
            add_executable(${test_name} ${test_source})
            
            # Link against core library
            target_link_libraries(${test_name} nexus_core)
            
            # Add test to CTest
            add_test(NAME ${test_name} COMMAND ${test_name})
        endforeach()