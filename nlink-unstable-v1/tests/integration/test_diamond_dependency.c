/**
 * @file test_diamond_dependency.c
 * @brief Integration test for diamond dependency resolution
 * 
 * This test verifies that the NexusLink system correctly resolves
 * diamond dependencies (A depends on B and C, both of which depend on
 * different versions of D).
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_symbols.h"
 #include "nexus_versioned_symbols.h"
 #include "nexus_semver.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /*
  * Diamond dependency scenario:
  *
  *      A
  *     / \
  *    B   C
  *    |   |
  *    D1  D2
  *
  * Where:
  * - A depends on B and C
  * - B depends on D version 1.0.0
  * - C depends on D version 2.0.0
  * - D provides the same symbol in both versions
  *
  * The correct resolution depends on the context:
  * - When A accesses the symbol through B, it should get D1's version
  * - When A accesses the symbol through C, it should get D2's version
  */
 
 void test_setup_diamond_dependency() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add components A, B, C, D1, D2
     // Component D1 v1.0.0
     versioned_symbol_table_add(&registry->exported, "diamond_function", "1.0.0", 
                               (void*)0x1, VSYMBOL_FUNCTION, 
                               "D", 10);
     
     // Component D2 v2.0.0
     versioned_symbol_table_add(&registry->exported, "diamond_function", "2.0.0", 
                               (void*)0x2, VSYMBOL_FUNCTION, 
                               "D", 10);
     
     // Add dependencies
     // B depends on D v1.0.0
     nexus_add_component_dependency(registry, "B", "D", "1.0.0", false);
     
     // C depends on D v2.0.0
     nexus_add_component_dependency(registry, "C", "D", "2.0.0", false);
     
     // A depends on B and C
     nexus_add_component_dependency(registry, "A", "B", "^1.0.0", false);
     nexus_add_component_dependency(registry, "A", "C", "^1.0.0", false);
     
     // Context A > B > D: Should resolve to D v1.0.0
     void* addr1 = nexus_resolve_versioned_symbol(registry, "diamond_function", NULL, "B");
     test_assert("Symbol resolved through B path", addr1 == (void*)0x1);
     
     // Context A > C > D: Should resolve to D v2.0.0
     void* addr2 = nexus_resolve_versioned_symbol(registry, "diamond_function", NULL, "C");
     test_assert("Symbol resolved through C path", addr2 == (void*)0x2);
     
     // Context A directly: Should prioritize one path
     // Let's assume the implementation prioritizes direct dependencies,
     // and B was added first, so it gets priority.
     void* addr3 = nexus_resolve_versioned_symbol(registry, "diamond_function", NULL, "A");
     test_assert("Symbol resolved directly from A with context awareness", 
                 addr3 != NULL); // We don't assert which version to allow flexibility
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 void test_diamond_dependency_with_constraints() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add components A, B, C, D1, D2
     // Component D1 v1.0.0
     versioned_symbol_table_add(&registry->exported, "constrained_function", "1.0.0", 
                               (void*)0x1, VSYMBOL_FUNCTION, 
                               "D", 10);
     
     // Component D2 v2.0.0
     versioned_symbol_table_add(&registry->exported, "constrained_function", "2.0.0", 
                               (void*)0x2, VSYMBOL_FUNCTION, 
                               "D", 10);
     
     // Add dependencies
     // B depends on D v1.0.0
     nexus_add_component_dependency(registry, "B", "D", "1.0.0", false);
     
     // C depends on D v2.0.0
     nexus_add_component_dependency(registry, "C", "D", "2.0.0", false);
     
     // A depends on B and C
     nexus_add_component_dependency(registry, "A", "B", "^1.0.0", false);
     nexus_add_component_dependency(registry, "A", "C", "^1.0.0", false);
     
     // Now let's use explicit constraints
     
     // Request v1.0.0 explicitly
     void* addr1 = nexus_resolve_versioned_symbol(registry, "constrained_function", "1.0.0", "A");
     test_assert("Explicit request for v1.0.0", addr1 == (void*)0x1);
     
     // Request v2.0.0 explicitly
     void* addr2 = nexus_resolve_versioned_symbol(registry, "constrained_function", "2.0.0", "A");
     test_assert("Explicit request for v2.0.0", addr2 == (void*)0x2);
     
     // Request compatible with v1.0.0
     void* addr3 = nexus_resolve_versioned_symbol(registry, "constrained_function", "^1.0.0", "A");
     test_assert("Compatible request for ^1.0.0", addr3 == (void*)0x1);
     
     // Request compatible with v2.0.0
     void* addr4 = nexus_resolve_versioned_symbol(registry, "constrained_function", "^2.0.0", "A");
     test_assert("Compatible request for ^2.0.0", addr4 == (void*)0x2);
     
     // Request compatible with both, should prioritize based on context
     void* addr5 = nexus_resolve_versioned_symbol(registry, "constrained_function", ">=1.0.0", "A");
     test_assert("Compatible with both versions", addr5 != NULL);
     
     // Request incompatible with both
     void* addr6 = nexus_resolve_versioned_symbol(registry, "constrained_function", "3.0.0", "A");
     test_assert("Incompatible request returns NULL", addr6 == NULL);
     
     // Clean up
     nexus_versioned_registry_free(registry);
 }
 
 void test_diamond_dependency_conflicts() {
     // Initialize registry
     VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
     
     // Add components A, B, C, D1, D2
     // Component D1 v1.0.0
     versioned_symbol_table_add(&registry->exported, "conflict_function", "1.0.0", 
                               (void*)0x1, VSYMBOL_