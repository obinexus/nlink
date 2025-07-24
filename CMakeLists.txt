# CMakeLists.txt - OBINexus NLink Constitutional Build Orchestration
# Fractal Modularity Governance: DAG-Protected Dependency Resolution
cmake_minimum_required(VERSION 3.20)

project(nlink
    VERSION 3.7.0
    DESCRIPTION "OBINexus NLink: Constitutional Build Coordination Framework"
    LANGUAGES C
)

# Constitutional Compilation Parameters
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Sinphases Compliance Enforcement
set(SINPHASES_COMPLIANCE "0.5" CACHE STRING "Constitutional compliance threshold")
add_definitions(-DSINPHASES_COMPLIANCE=${SINPHASES_COMPLIANCE})
add_definitions(-DOBINEXUS_CONSTITUTIONAL_VALIDATION)

# Constitutional Security Flags
add_compile_options(
    -Wall -Wextra -Werror
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fno-omit-frame-pointer
    -grecord-gcc-switches
)

# Include Directory Hierarchy
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nlink
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nlink/core
)

# Core Module Registry: DAG-Protected Source Collection
set(CORE_SOURCES
    # Tier 1: Constitutional Foundation
    src/core/config/config_manager.c
    src/core/config/config_parser.c
    src/core/crypto/shannon_entropy.c
    src/core/marshal/marshal.c
    
    # Tier 2: Version Coordination  
    src/core/semverx/semver.c
    src/core/semverx/nexus_version.c
    src/core/semverx/lazy_versioned.c
    src/core/semverx/missing_functions.c
    
    # Tier 3: Monitoring Coordination
    src/core/etps/telemetry.c
    
    # Additional tiers would be added as implementation progresses
)

# Static Library Target: Constitutional Coordination Artifact
add_library(nlink_static STATIC ${CORE_SOURCES})
target_include_directories(nlink_static PUBLIC include)

# Shared Library Target: Polymorphic Coordination Protocol
add_library(nlink_shared SHARED ${CORE_SOURCES})
target_include_directories(nlink_shared PUBLIC include)
set_target_properties(nlink_shared PROPERTIES OUTPUT_NAME nlink)

# CLI Binary Target: User Interface Boundary Management
add_executable(nlink src/core/cli/main.c)
target_link_libraries(nlink nlink_static)

# Installation Protocol: Constitutional Distribution
install(TARGETS nlink_static nlink_shared nlink
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/nlink DESTINATION include)

# Enable Testing Framework
enable_testing()
add_subdirectory(tests OPTIONAL)
