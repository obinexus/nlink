# NexusLink Unit Tests
# CMake configuration for unit tests

# Test helper library for creating mock components
add_library(test_components SHARED
    components/mock_component.c
    components/version_component.c
)

# Core system tests
add_executable(test_core_context core/test_core_context.c)
target_link_libraries(test_core_context test_common nexus_core)
add_test(NAME CoreContext COMMAND test_core_context)

add_executable(test_loader components/test_loader.c)
target_link_libraries(test_loader test_common nexus_core nexus_loader ${DL_LIBRARY})
add_test(NAME Loader COMMAND test_loader)

# Symbol management tests
add_executable(test_symbols ${CMAKE_CURRENT_SOURCE_DIR}/../common/test_symbol.c)
target_link_libraries(test_symbols test_common nexus_core nexus_symbols)
add_test(NAME Symbols COMMAND test_symbols)

add_executable(test_symbol_resolution core/test_symbol_resolution.c)
target_link_libraries(test_symbol_resolution test_common nexus_core nexus_symbols)
add_test(NAME SymbolResolution COMMAND test_symbol_resolution)

# Versioning tests
add_executable(test_semver ${CMAKE_CURRENT_SOURCE_DIR}/../common/test_semver.c)
target_link_libraries(test_semver test_common nexus_versioning)
add_test(NAME SemVer COMMAND test_semver)

add_executable(test_version_constraints core/test_version_constraints.c)
target_link_libraries(test_version_constraints test_common nexus_versioning)
add_test(NAME VersionConstraints COMMAND test_version_constraints)

# Lazy loading tests
add_executable(test_lazy_loading core/test_lazy_loading.c)
target_link_libraries(test_lazy_loading test_common nexus_core nexus_symbols ${DL_LIBRARY})
add_test(NAME LazyLoading COMMAND test_lazy_loading)

add_executable(test_versioned_lazy core/test_versioned_lazy.c)
target_link_libraries(test_versioned_lazy test_common nexus_core nexus_symbols nexus_versioning ${DL_LIBRARY})
add_test(NAME VersionedLazy COMMAND test_versioned_lazy)

# Add the versioned unload test that was missing
add_executable(test_versioned_unload core/test_versioned_unload.c)
target_link_libraries(test_versioned_unload test_common nexus_core nexus_symbols nexus_versioning ${DL_LIBRARY})
add_test(NAME VersionedUnload COMMAND test_versioned_unload)

# JSON handling tests
add_executable(test_json components/test_json.c)
target_link_libraries(test_json test_common nexus_core)
add_test(NAME Json COMMAND test_json)

# Component integration tests
add_executable(test_versioned_integration components/test_versioned_integration.c)
target_link_libraries(test_versioned_integration test_common nexus_core nexus_symbols nexus_versioning ${DL_LIBRARY})
add_test(NAME VersionedIntegration COMMAND test_versioned_integration)