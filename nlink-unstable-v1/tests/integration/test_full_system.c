/**
 * @file test_full_system.c
 * @brief Full system integration test for NexusLink
 * 
 * This test verifies the complete integration of all NexusLink subsystems:
 * - Core context and configuration
 * - Component loading and lifecycle management
 * - Symbol registry and resolution
 * - Version-aware symbol resolution
 * - Lazy loading with version constraints
 * - Metadata management
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_loader.h"
 #include "nexus_symbols.h"
 #include "nexus_versioned_symbols.h"
 #include "nexus_lazy_versioned.h"
 #include "nexus_semver.h"
 #include "nexus_enhanced_metadata.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 
 // Mock functions for component implementations
 static void mock_function_v1(int x) {
     printf("Version 1.0.0 function called with %d\n", x);
 }
 
 static void mock_function_v2(int x) {
     printf("Version 2.0.0 function called with %d\n", x);
 }
 
 // Test function for lazy loading
 NEXUS_LAZY_VERSIONED(full_system_test_function, void, "^1.0.0", int x) {
     // Implementation will be resolved from the component registry
     printf("Lazy loaded function called with %d\n", x);
 }
 
 void test_core_system_initialization() {
     // Initialize core system
     NexusConfig config = {
         .flags = NEXUS_FLAG_STRICT_VERSIONING | NEXUS_FLAG_VERBOSE_LOGGING,
         .log_level = NEXUS_LOG_DEBUG,
         .component_path = "./components"
     };
     
     NexusContext* ctx = nexus_create_context(&config);
     test_assert("Core context initialization", ctx != NULL);
     
     if (ctx) {
         test_assert("Log level configured", ctx->log_level == NEXUS_LOG_DEBUG);
         test_assert("Flags configured", 
                     ctx->flags == (NEXUS_FLAG_STRICT_VERSIONING | NEXUS_FLAG_VERBOSE_LOGGING));
         test_assert("Component path configured", 
                     strcmp(ctx->component_path, "./components") == 0);
         test_assert("Symbol registry initialized", ctx->symbols != NULL);
     }
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_versioned_registry_setup() {
     // Create specialized versioned symbol registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     test_assert("Versioned registry creation", registry != NULL);
     
     if (registry) {
         // Register symbols with different versions
         versioned_symbol_table_add(&registry->exported, "multi_version_func", "1.0.0", 
                                   (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                                   "component_v1", 10);
         
         versioned_symbol_table_add(&registry->exported, "multi_version_func", "2.0.0", 
                                   (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                                   "component_v2", 10);
         
         // Verify registrations
         VersionedSymbol** symbols = NULL;
         size_t count = versioned_symbol_table_find_all(&registry->exported, "multi_version_func", &symbols);
         
         test_assert("Symbol registered with multiple versions", count == 2);
         
         if (count > 0) {
             free(symbols);
         }
     }
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 void test_component_loading_and_resolution() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     test_assert("Context creation for component loading", ctx != NULL);
     
     if (ctx) {
         // Load components
         NexusComponent* component1 = nexus_load_component(ctx, "./libv1.so", "component_v1");
         test_assert("Load component v1", component1 != NULL);
         
         NexusComponent* component2 = nexus_load_component(ctx, "./libv2.so", "component_v2");
         test_assert("Load component v2", component2 != NULL);
         
         // Resolve symbols from components
         void* symbol1 = NULL;
         void* symbol2 = NULL;
         
         if (component1) {
             symbol1 = nexus_resolve_component_symbol(ctx, component1, "test_function");
             test_assert("Resolve symbol from component 1", symbol1 != NULL);
         }
         
         if (component2) {
             symbol2 = nexus_resolve_component_symbol(ctx, component2, "test_function");
             test_assert("Resolve symbol from component 2", symbol2 != NULL);
         }
         
         // Verify symbols in exported table
         if (symbol1 && symbol2) {
             NexusSymbol* exported1 = nexus_symbol_table_find(&ctx->symbols->exported, "test_function");
             test_assert("Symbol in exported table", exported1 != NULL);
             
             // Because both components export the same symbol name, the latest one registered will be found
             test_assert("Exported symbol address matches latest resolution", 
                         exported1->address == symbol2);
         }
         
         // Unload components
         if (component1) {
             NexusResult result = nexus_unload_component(ctx, component1);
             test_assert("Unload component 1", result == NEXUS_SUCCESS);
         }
         
         if (component2) {
             NexusResult result = nexus_unload_component(ctx, component2);
             test_assert("Unload component 2", result == NEXUS_SUCCESS);
         }
     }
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_integrated_version_resolution() {
     // Create context with version-aware registry
     NexusContext* ctx = nexus_create_context(NULL);
     test_assert("Context creation for integrated test", ctx != NULL);
     
     // Create versioned registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     test_assert("Versioned registry creation for integrated test", registry != NULL);
     
     if (ctx && registry) {
         // Register symbols with versions
         versioned_symbol_table_add(&registry->exported, "integrated_func", "1.0.0", 
                                   (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                                   "component_v1", 10);
         
         versioned_symbol_table_add(&registry->exported, "integrated_func", "2.0.0", 
                                   (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                                   "component_v2", 10);
         
         // Set up dependencies
         nexus_add_component_dependency(registry, "app", "component_v1", "^1.0.0", false);
         nexus_add_component_dependency(registry, "app", "component_v2", "^2.0.0", false);
         
         // Configure lazy loading to use the registry
         NexusVersionedLazyConfig config = {
             .library_path = "./libtest.so",
             .component_id = "app",
             .version = "1.0.0",
             .version_constraint = "^1.0.0",
             .auto_unload = false,
             .unload_timeout_sec = 300,
             .registry = registry
         };
         
         nexus_set_versioned_lazy_config(&config);
         
         // Initialize handle registry
         extern NexusHandleRegistry* nexus_handle_registry;
         nexus_init_handle_registry();
         
         // Register the test function
         versioned_symbol_table_add(&registry->exported, "full_system_test_function", "1.0.0", 
                                   (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                                   "component_v1", 10);
         
         versioned_symbol_table_add(&registry->exported, "full_system_test_function", "2.0.0", 
                                   (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                                   "component_v2", 10);
         
         // Call the lazy-loaded function with v1 constraint
         config.version_constraint = "^1.0.0";
         nexus_set_versioned_lazy_config(&config);
         full_system_test_function(1);
         
         // Call the lazy-loaded function with v2 constraint
         config.version_constraint = "^2.0.0";
         nexus_set_versioned_lazy_config(&config);
         full_system_test_function(2);
         
         // Clean up handle registry
         nexus_cleanup_handle_registry();
     }
     
     // Clean up
     if (registry) {
         nexus_versioned_registry_free(registry);
     }
     
     if (ctx) {
         nexus_destroy_context(ctx);
     }
 }
 
 void test_metadata_integration() {
     // Create metadata
     EnhancedComponentMetadata* metadata = nexus_enhanced_metadata_create(
         "test_component", "1.0.0", "Test component for integration testing");
     
     test_assert("Enhanced metadata creation", metadata != NULL);
     
     if (metadata) {
         // Add dependencies
         nexus_enhanced_metadata_add_dependency(metadata, "dep1", "^1.0.0", false);
         nexus_enhanced_metadata_add_dependency(metadata, "dep2", "^2.0.0", true); // Optional
         
         // Add exported symbols
         nexus_enhanced_metadata_add_exported_symbol(metadata, "test_func1", "1.0.0", 0);
         nexus_enhanced_metadata_add_exported_symbol(metadata, "test_func2", "1.0.0", 0);
         
         // Add imported symbols
         nexus_enhanced_metadata_add_imported_symbol(metadata, "dep_func1", "^1.0.0", 0);
         
         // Set resource metrics
         metadata->memory_footprint = 1024;  // 1 KB
         metadata->avg_load_time_ms = 2.5;   // 2.5 ms
         
         // Track usage
         nexus_enhanced_metadata_track_usage(metadata);
         test_assert("Metadata usage count after tracking", metadata->usage_count == 1);
         
         // Create versioned registry for dependency testing
         VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
         
         // Add available dependencies to registry
         EnhancedComponentMetadata* dep1 = nexus_enhanced_metadata_create(
             "dep1", "1.1.0", "Dependency 1");
         
         EnhancedComponentMetadata* dep3 = nexus_enhanced_metadata_create(
             "dep3", "3.0.0", "Dependency 3");
         
         // Convert to const for the function call
         const EnhancedComponentMetadata* available_deps[2] = { dep1, dep3 };
         
         // Check dependencies
         char* missing_dep = NULL;
         bool deps_ok = nexus_enhanced_metadata_check_dependencies(
             metadata, available_deps, 2, &missing_dep);
         
         test_assert("Dependency check with missing dep2 (optional)", deps_ok);
         test_assert("No missing dependency reported for optional deps", missing_dep == NULL);
         
         // Make dep1 required and remove it from available
         metadata->dependencies[0].optional = false;
         const EnhancedComponentMetadata* available_deps2[1] = { dep3 };
         
         deps_ok = nexus_enhanced_metadata_check_dependencies(
             metadata, available_deps2, 1, &missing_dep);
         
         test_assert("Dependency check with missing required dep", !deps_ok);
         test_assert("Missing dependency identified", 
                     missing_dep != NULL && strcmp(missing_dep, "dep1") == 0);
         
         if (missing_dep) {
             free(missing_dep);
         }
         
         // Free metadata
         nexus_enhanced_metadata_free(metadata);
         nexus_enhanced_metadata_free(dep1);
         nexus_enhanced_metadata_free(dep3);
         nexus_versioned_registry_free(registry);
     }
 }
 
 void test_full_system_workflow() {
     printf("\n=== Full System Workflow Test ===\n");
     printf("This test demonstrates the complete workflow of the NexusLink system.\n");
     
     // 1. Initialize context
     printf("\nStep 1: Initializing context...\n");
     NexusContext* ctx = nexus_create_context(NULL);
     test_assert("Context initialization", ctx != NULL);
     
     // 2. Create versioned registry
     printf("\nStep 2: Creating versioned registry...\n");
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     test_assert("Versioned registry creation", registry != NULL);
     
     // 3. Load component metadata
     printf("\nStep 3: Creating component metadata...\n");
     EnhancedComponentMetadata* c1_metadata = nexus_enhanced_metadata_create(
         "component_v1", "1.0.0", "Component v1 implementation");
     
     EnhancedComponentMetadata* c2_metadata = nexus_enhanced_metadata_create(
         "component_v2", "2.0.0", "Component v2 implementation");
     
     EnhancedComponentMetadata* app_metadata = nexus_enhanced_metadata_create(
         "app", "1.0.0", "Application component");
     
     test_assert("Metadata creation", 
                 c1_metadata != NULL && c2_metadata != NULL && app_metadata != NULL);
     
     // 4. Set up dependencies
     printf("\nStep 4: Setting up dependencies...\n");
     nexus_enhanced_metadata_add_dependency(app_metadata, "component_v1", "^1.0.0", false);
     nexus_enhanced_metadata_add_dependency(app_metadata, "component_v2", "^2.0.0", false);
     
     nexus_add_component_dependency(registry, "app", "component_v1", "^1.0.0", false);
     nexus_add_component_dependency(registry, "app", "component_v2", "^2.0.0", false);
     
     // 5. Register symbols with version information
     printf("\nStep 5: Registering symbols with version information...\n");
     versioned_symbol_table_add(&registry->exported, "workflow_function", "1.0.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "workflow_function", "2.0.0", 
                               (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // 6. Resolve symbols with version constraints
     printf("\nStep 6: Resolving symbols with version constraints...\n");
     void* v1_symbol = nexus_resolve_versioned_symbol(registry, "workflow_function", "^1.0.0", "app");
     void* v2_symbol = nexus_resolve_versioned_symbol(registry, "workflow_function", "^2.0.0", "app");
     
     test_assert("Symbol resolution v1", v1_symbol == (void*)mock_function_v1);
     test_assert("Symbol resolution v2", v2_symbol == (void*)mock_function_v2);
     
     // 7. Set up lazy loading
     printf("\nStep 7: Setting up lazy loading...\n");
     NexusVersionedLazyConfig config = {
         .library_path = "./libtest.so",
         .component_id = "app",
         .version = "1.0.0",
         .version_constraint = NULL,  // Will be set in the tests
         .auto_unload = true,
         .unload_timeout_sec = 60,
         .registry = registry
     };
     
     // Add to registry for lazy loading
     versioned_symbol_table_add(&registry->exported, "full_system_test_function", "1.0.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "full_system_test_function", "2.0.0", 
                               (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // 8. Call with different version constraints
     printf("\nStep 8: Testing lazy loading with different version constraints...\n");
     
     // Initialize handle registry
     extern NexusHandleRegistry* nexus_handle_registry;
     nexus_init_handle_registry();
     
     // Use v1 constraint
     printf("Using v1 constraint (^1.0.0)...\n");
     config.version_constraint = "^1.0.0";
     nexus_set_versioned_lazy_config(&config);
     full_system_test_function(1);
     
     // Use v2 constraint
     printf("Using v2 constraint (^2.0.0)...\n");
     config.version_constraint = "^2.0.0";
     nexus_set_versioned_lazy_config(&config);
     full_system_test_function(2);
     
     // 9. Check for unused libraries
     printf("\nStep 9: Checking for unused libraries...\n");
     nexus_check_unused_versioned_libraries(registry);
     
     // 10. Clean up
     printf("\nStep 10: Cleaning up...\n");
     nexus_cleanup_handle_registry();
     nexus_enhanced_metadata_free(c1_metadata);
     nexus_enhanced_metadata_free(c2_metadata);
     nexus_enhanced_metadata_free(app_metadata);
     nexus_versioned_registry_free(registry);
     nexus_destroy_context(ctx);
     
     printf("\nFull system workflow test completed successfully.\n");
 }
 
 int main() {
     test_init("Full System Integration");
     
     test_core_system_initialization();
     test_versioned_registry_setup();
     test_component_loading_and_resolution();
     test_integrated_version_resolution();
     test_metadata_integration();
     test_full_system_workflow();
     
     return test_finalize();
 }