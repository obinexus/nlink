cmake_minimum_required(VERSION 3.12)

# Define unit tests

# Symbol table test
add_executable(test_symbols test_symbols.c)
target_link_libraries(test_symbols PRIVATE nexus_core)
add_test(NAME SymbolsTest COMMAND test_symbols)

# Add additional unit tests as they are implemented
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_diamond_dependency.c")
    add_executable(test_diamond_dependency test_diamond_dependency.c)
    target_link_libraries(test_diamond_dependency PRIVATE nexus_core)
    add_test(NAME DiamondDependencyTest COMMAND test_diamond_dependency)
endif()