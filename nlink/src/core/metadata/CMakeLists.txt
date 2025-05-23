# Metadata component build configuration

# Create library target for metadata component
add_library(nexus_metadata
	enhanced_metadata.c
)

# Set include directories
target_include_directories(nexus_metadata
	PUBLIC
		${CMAKE_SOURCE_DIR}/include
)

# Link with required libraries
target_link_libraries(nexus_metadata
	PUBLIC
		nexus_common
)

# Set compile definitions based on global options
if(NEXUS_ENABLE_VERBOSE_LOGGING)
	target_compile_definitions(nexus_metadata PRIVATE NEXUS_VERBOSE_LOGGING=1)
endif()

if(NEXUS_USE_MEMORY_POOL)
	target_compile_definitions(nexus_metadata PRIVATE NEXUS_USE_MEMORY_POOL=1)
endif()

if(NEXUS_ENABLE_THREAD_SAFETY)
	target_compile_definitions(nexus_metadata PRIVATE NEXUS_THREAD_SAFETY=1)
endif()

# Install the library
install(TARGETS nexus_metadata
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
