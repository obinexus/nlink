# Pattern matcher implementation for NexusLink
set(PATTERN_SOURCES
	matcher.c
	regex_matcher.c
)

# Create a library target for the pattern matcher
add_library(nlink_pattern ${PATTERN_SOURCES})

# Add include directories
target_include_directories(nlink_pattern
	PUBLIC
		${CMAKE_SOURCE_DIR}/include
)

# Link with system libraries (needed for regex functions)
target_link_libraries(nlink_pattern
	PRIVATE
		m  # Math library
)

# Set properties on the target library
set_target_properties(nlink_pattern PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
)
