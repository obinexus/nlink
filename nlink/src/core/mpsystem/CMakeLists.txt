# MPSystem - Multi-Pass System module for NexusLink
# Provides pipeline management with feedback loops and bidirectional data flows

# Source files for the mpsystem module
set(MPS_SOURCES
	mps_config.c
	mps_dependency.c
	mps_lifecycle.c
	mps_pipeline.c
	mps_stream.c
)

# Header files (for IDE integration)
set(MPS_HEADERS
	${CMAKE_SOURCE_DIR}/include/nlink/core/mpsystem/mps_config.h
	${CMAKE_SOURCE_DIR}/include/nlink/core/mpsystem/mps_dependency.h
	${CMAKE_SOURCE_DIR}/include/nlink/core/mpsystem/mps_lifecycle.h
	${CMAKE_SOURCE_DIR}/include/nlink/core/mpsystem/mps_pipeline.h
	${CMAKE_SOURCE_DIR}/include/nlink/core/mpsystem/mps_stream.h
)

# Create the mpsystem library
add_library(nlink_mpsystem ${MPS_SOURCES})

# Setup include directories
target_include_directories(nlink_mpsystem
	PUBLIC
		${CMAKE_SOURCE_DIR}/include
	PRIVATE
		${CMAKE_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(nlink_mpsystem
	PUBLIC
		nlink_core_common
)

# Installation rules
install(TARGETS nlink_mpsystem
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${MPS_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlink/core/mpsystem
)
