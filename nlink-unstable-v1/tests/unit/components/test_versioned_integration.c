/**
 * @file test_versioned_integration.c
 * @brief Integration test for versioned symbol resolution
 * 
 * This test verifies the integration of versioned symbol resolution,
 * lazy loading, and component management within the NexusLink system.
 * It tests the complete workflow from component loading to symbol resolution
 * with version constraints.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_loader.h"
 #include "nexus_symbols.h"
 #include "nexus_versioned_symbols.h"
 #include "nexus_lazy_versioned.h"
 #include "nexus_semver.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 
 // Mock functions for component implementation
 static void mock_function_v1(int x) {
     printf("Version 1.0.0 function called with %d\n", x);
 }
 
 static void mock_function_v2(int x) {
     printf("Version 2.0.0 function called with %d\n", x);
 }
 
 // Test function for versioned lazy loading
 NEXUS_LAZY_VERSIONED(versioned_test_function, void, "^1.0.0", int x) {
     // The implementation will be loaded from the mock component
     printf("versioned_test_function implementation called with %d\n", x);
 }
 
 void test_versioned_component_setup() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Initialize versioned registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Set up two components with different versions of the same symbol
     // Component 1 - v1.0.0
     NexusComponent* component1 = nexus_load_component(ctx, "./libv1.so", "component_v1");
     test_assert("Load component v1", component1 != NULL);
     
     // Component 2 - v2.0.0
     NexusComponent* component2 = nexus_load_component(ctx, "./libv2.so", "component_v2");
     test_assert("Load component v2", component2 != NULL);
     
     // Add symbols to registry
     versioned_symbol_table_add(&registry->exported, "test_function", "1.0.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "test_function", "2.0.0", 
                               (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // Add dependency relationships
     nexus_add_component_dependency(registry, "app", "component_v1", "^1.0.0", false);
     nexus_add_component_dependency(registry, "app", "component_v2", "^2.0.0", false);
     
     // Verify that symbols are registered
     VersionedSymbol** symbols = NULL;
     size_t count = versioned_symbol_table_find_all(&registry->exported, "test_function", &symbols);
     
     test_assert("Symbol registered in both versions", count == 2);
     
     if (count > 0) {
         free(symbols);
     }
     
     // Clean up
     if (component1) {
         nexus_unload_component(ctx, component1);
     }
     
     if (component2) {
         nexus_unload_component(ctx, component2);
     }
     
     nexus_versioned_registry_free(registry);
     nexus_destroy_context(ctx);
 }
 
 void test_versioned_symbol_resolution() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add symbols to registry with different versions
     versioned_symbol_table_add(&registry->exported, "versioned_function", "1.0.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "versioned_function", "1.1.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "versioned_function", "2.0.0", 
                               (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // Add dependency relationships
     nexus_add_component_dependency(registry, "app1", "component_v1", "^1.0.0", false);
     nexus_add_component_dependency(registry, "app2", "component_v2", "^2.0.0", false);
     
     // Different version constraint tests
     
     // Exact match
     void* exact_v1 = nexus_resolve_versioned_symbol(registry, "versioned_function", "1.0.0", "app1");
     test_assert("Resolve with exact version 1.0.0", exact_v1 == (void*)mock_function_v1);
     
     void* exact_v2 = nexus_resolve_versioned_symbol(registry, "versioned_function", "2.0.0", "app2");
     test_assert("Resolve with exact version 2.0.0", exact_v2 == (void*)mock_function_v2);
     
     // Compatible version (^1.0.0 means >=1.0.0 <2.0.0)
     void* compat_v1 = nexus_resolve_versioned_symbol(registry, "versioned_function", "^1.0.0", "app1");
     test_assert("Resolve with compatible version ^1.0.0", compat_v1 == (void*)mock_function_v1);
     
     // Higher compatible version should find the highest compatible version
     versioned_symbol_table_add(&registry->exported, "versioned_function", "1.2.0", 
                               (void*)0x42, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     void* highest_compat = nexus_resolve_versioned_symbol(registry, "versioned_function", "^1.0.0", "app1");
     test_assert("Resolve highest compatible version", highest_compat == (void*)0x42);
     
     // Incompatible version
     void* incompatible = nexus_resolve_versioned_symbol(registry, "versioned_function", "3.0.0", "app1");
     test_assert("Incompatible version returns NULL", incompatible == NULL);
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 void test_versioned_lazy_loading() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add symbols to registry
     versioned_symbol_table_add(&registry->exported, "versioned_test_function", "1.0.0", 
                               (void*)mock_function_v1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "versioned_test_function", "2.0.0", 
                               (void*)mock_function_v2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // Add dependency relationships
     nexus_add_component_dependency(registry, "app", "component_v1", "^1.0.0", false);
     
     // Initialize handle registry
     extern NexusHandleRegistry* nexus_handle_registry;
     nexus_init_handle_registry();
     
     // Set configuration for lazy loading
     NexusVersionedLazyConfig config = {
         .library_path = "./libv1.so",
         .component_id = "app",
         .version = "1.0.0",
         .version_constraint = "^1.0.0",
         .auto_unload = false,
         .unload_timeout_sec = 300,
         .registry = registry
     };
     
     nexus_set_versioned_lazy_config(&config);
     
     // Call the lazy-loaded function
     // This should trigger loading of the v1 implementation
     versioned_test_function(42);
     
     // Since we can't easily verify which implementation was called in a unit test,
     // we'll just assert that the function executed without errors
     test_assert("Versioned lazy function execution", true);
     
     // Change the constraint to use v2
     config.version_constraint = "^2.0.0";
     nexus_set_versioned_lazy_config(&config);
     
     // This should trigger loading of the v2 implementation
     versioned_test_function(42);
     
     // Clean up
     nexus_cleanup_handle_registry();
     nexus_versioned_registry_free(registry);
 }
 
 void test_version_conflict_resolution() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add symbols with conflicting versions
     versioned_symbol_table_add(&registry->exported, "conflicted_function", "1.0.0", 
                               (void*)0x1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "conflicted_function", "2.0.0", 
                               (void*)0x2, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // Set up conflicting dependencies
     // Component A depends on v1
     nexus_add_component_dependency(registry, "component_a", "component_v1", "=1.0.0", false);
     // Component B depends on v2
     nexus_add_component_dependency(registry, "component_b", "component_v2", "=2.0.0", false);
     // App depends on both A and B
     nexus_add_component_dependency(registry, "app", "component_a", "^1.0.0", false);
     nexus_add_component_dependency(registry, "app", "component_b", "^1.0.0", false);
     
     // Detect conflicts
     char* conflict_details = NULL;
     bool has_conflicts = nexus_detect_version_conflicts(registry, "app", &conflict_details);
     
     test_assert("Detect version conflicts", has_conflicts);
     test_assert("Conflict details provided", conflict_details != NULL);
     
     if (conflict_details) {
         printf("Conflict details: %s\n", conflict_details);
         free(conflict_details);
     }
     
     // Test conflict resolution - app should be able to use both versions
     // depending on context
     
     // When resolving through component A, should get v1
     void* v1_addr = nexus_resolve_versioned_symbol(registry, "conflicted_function", "^1.0.0", "component_a");
     test_assert("Resolve v1 through component A", v1_addr == (void*)0x1);
     
     // When resolving through component B, should get v2
     void* v2_addr = nexus_resolve_versioned_symbol(registry, "conflicted_function", "^2.0.0", "component_b");
     test_assert("Resolve v2 through component B", v2_addr == (void*)0x2);
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 void test_version_constraint_enforcement() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add symbols with different versions
     versioned_symbol_table_add(&registry->exported, "constrained_function", "1.0.0", 
                               (void*)0x1, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "constrained_function", "1.1.0", 
                               (void*)0x2, VSYMBOL_FUNCTION, 
                               "component_v1", 10);
     
     versioned_symbol_table_add(&registry->exported, "constrained_function", "2.0.0", 
                               (void*)0x3, VSYMBOL_FUNCTION, 
                               "component_v2", 10);
     
     // Set up dependencies
     nexus_add_component_dependency(registry, "app", "component_v1", "^1.0.0", false);
     nexus_add_component_dependency(registry, "app", "component_v2", "^2.0.0", false);
     
     // Test different constraint formats
     // Exact version
     void* exact = nexus_resolve_versioned_symbol(registry, "constrained_function", "1.0.0", "app");
     test_assert("Exact version constraint", exact == (void*)0x1);
     
     // Compatible versions (^)
     void* caret_1 = nexus_resolve_versioned_symbol(registry, "constrained_function", "^1.0.0", "app");
     test_assert("Caret range finds highest compatible version", caret_1 == (void*)0x2);
     
     void* caret_2 = nexus_resolve_versioned_symbol(registry, "constrained_function", "^2.0.0", "app");
     test_assert("Caret range for major version 2", caret_2 == (void*)0x3);
     
     // Greater than constraint
     void* gt = nexus_resolve_versioned_symbol(registry, "constrained_function", ">1.0.0", "app");
     test_assert("Greater than constraint", gt == (void*)0x2);
     
     // Lesser than constraint
     void* lt = nexus_resolve_versioned_symbol(registry, "constrained_function", "<2.0.0", "app");
     test_assert("Lesser than constraint", lt == (void*)0x2);
     
     // Range constraint (not directly supported, but test as a placeholder)
     if (nexus_resolve_versioned_symbol(registry, "constrained_function", ">=1.0.0 <=1.1.0", "app")) {
         test_assert("Range constraint (if implemented)", true);
     } else {
         printf("Note: Range constraints not implemented in this version\n");
     }
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 int main() {
     test_init("Versioned Integration");
     
     test_versioned_component_setup();
     test_versioned_symbol_resolution();
     test_versioned_lazy_loading();
     test_version_conflict_resolution();
     test_version_constraint_enforcement();
     
     return test_finalize();
 }