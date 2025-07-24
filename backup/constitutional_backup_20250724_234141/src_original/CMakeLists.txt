# Source directory CMakeLists.txt

# Collect feature sources (defined in Features.cmake)
add_library(nlink SHARED ${NLINK_SOURCES})
add_library(nlink_static STATIC ${NLINK_SOURCES})

# Set target properties
set_target_properties(nlink PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${NLINK_HEADERS}"
)

set_target_properties(nlink_static PROPERTIES
    OUTPUT_NAME nlink
)

# Create CLI executable
add_executable(nlink_cli ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
target_link_libraries(nlink_cli nlink)
set_target_properties(nlink_cli PROPERTIES OUTPUT_NAME nlink)

# Feature-specific configurations
foreach(feature ${NLINK_FEATURES})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/core/${feature}/CMakeLists.txt)
        add_subdirectory(core/${feature})
    endif()
endforeach()
