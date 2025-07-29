# NexusLink Integration Tests
# CMake configuration for integration tests

# Diamond dependency tests
add_executable(integration_test_diamond_dependency test_diamond_dependency.c)
target_link_libraries(integration_test_diamond_dependency 
    test_common 
    nexus_core 
    nexus_symbols 
    nexus_versioning 
    ${DL_LIBRARY}
)
add_test(NAME DiamondDependency COMMAND integration_test_diamond_dependency)

# Component lifecycle tests
add_executable(integration_test_component_lifecycle test_component_lifecycle.c)
target_link_libraries(integration_test_component_lifecycle 
    test_common 
    nexus_core 
    nexus_loader 
    nexus_symbols 
    ${DL_LIBRARY}
)
add_test(NAME ComponentLifecycle COMMAND integration_test_component_lifecycle)

# Symbol resolution integration test
add_executable(integration_test_symbol_resolution test_symbol_resolution.c)
target_link_libraries(integration_test_symbol_resolution
    test_common
    nexus_core
    nexus_symbols
    ${DL_LIBRARY}
)
add_test(NAME IntegrationSymbolResolution COMMAND integration_test_symbol_resolution)

# Version constraints integration test
add_executable(integration_test_version_constraints test_version_constraints.c)
target_link_libraries(integration_test_version_constraints
    test_common
    nexus_core
    nexus_versioning
    ${DL_LIBRARY}
)
add_test(NAME IntegrationVersionConstraints COMMAND integration_test_version_constraints)

# Versioned integration test
add_executable(integration_test_versioned_integration test_versioned_integration.c)
target_link_libraries(integration_test_versioned_integration
    test_common
    nexus_core
    nexus_versioning
    nexus_symbols
    ${DL_LIBRARY}
)
add_test(NAME IntegrationVersionedIntegration COMMAND integration_test_versioned_integration)

# Full system tests
add_executable(integration_test_full_system test_full_system.c)
target_link_libraries(integration_test_full_system 
    test_common 
    nexus_core 
    nexus_loader 
    nexus_symbols 
    nexus_versioning 
    nexus_metadata 
    ${DL_LIBRARY}
)
add_test(NAME FullSystem COMMAND integration_test_full_system)