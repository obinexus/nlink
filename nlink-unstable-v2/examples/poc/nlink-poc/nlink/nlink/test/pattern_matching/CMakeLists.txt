# CMakeLists.txt for NexusLink pattern matching tests
# Copyright © 2025 OBINexus Computing

# Add test executable
add_executable(test_pattern_matcher test_pattern_matcher.c)

# Link with pattern matching library
target_link_libraries(test_pattern_matcher PRIVATE
    nlink_pattern_matching
)

# Add test to CTest
add_test(NAME test_pattern_matcher COMMAND test_pattern_matcher)
