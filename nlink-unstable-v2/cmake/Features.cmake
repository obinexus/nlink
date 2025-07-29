# Feature configuration for NexusLink

# Define available features
set(NLINK_FEATURES
    semverx
    parser
    schema
    minimizer
    etps
    symbols
    pipeline
    cli
    tatit
    mpsystem
    spsystem
)

# Feature source collection
foreach(feature ${NLINK_FEATURES})
    file(GLOB ${feature}_SOURCES 
        "${CMAKE_SOURCE_DIR}/src/core/${feature}/*.c"
        "${CMAKE_SOURCE_DIR}/src/core/${feature}/*.cpp"
    )
    
    file(GLOB ${feature}_HEADERS
        "${CMAKE_SOURCE_DIR}/include/nlink/core/${feature}/*.h"
    )
    
    if(${feature}_SOURCES)
        message(STATUS "Feature '${feature}': ${${feature}_SOURCES}")
    endif()
endforeach()

# Collect all sources
set(NLINK_SOURCES)
foreach(feature ${NLINK_FEATURES})
    list(APPEND NLINK_SOURCES ${${feature}_SOURCES})
endforeach()

# CLI sources
file(GLOB CLI_SOURCES "${CMAKE_SOURCE_DIR}/src/cli/*.c")
list(APPEND NLINK_SOURCES ${CLI_SOURCES})
