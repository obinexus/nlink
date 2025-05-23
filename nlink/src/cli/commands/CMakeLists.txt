# Commands component CMakeLists.txt

# Collect command sources
set(COMMAND_SOURCES
	load.c
	minimal.c
	minimize.c
	version.c
)

# Create commands library
add_library(nexus_commands OBJECT ${COMMAND_SOURCES})

# Add include directories
target_include_directories(nexus_commands PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Set compile options
target_compile_options(nexus_commands PRIVATE
	-Wall 
	-Wextra
	-Wno-unused-parameter
	-fPIC
)

# Set position independent code for the object library
set_target_properties(nexus_commands PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)

# Link against the CLI library
target_link_libraries(nexus_commands PRIVATE nexus_cli)
