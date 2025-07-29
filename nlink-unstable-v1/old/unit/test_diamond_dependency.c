// test_diamond_dependency.c - Test for diamond dependency resolution
// Author: Nnamdi Michael Okpala
#include "../../include/nexus_core.h"
#include "../../include/nexus_symbols.h"
#include "../../include/nexus_loader.h"
#include "../../include/nexus_version.h"
#include <stdio.h>
#include <assert.h>

/*
 * This test verifies the diamond dependency resolution capability of NexusLink.
 * It simulates the following dependency graph:
 *
 *     App
 *    /   \
 *   v     v
 * Lib1   Lib2
 *   \     /
 *    v   v
 *   Common
 *
 * Where:
 * - Lib1 depends on Common v1.0.0
 * - Lib2 depends on Common v2.0.0
 * - App depends on both Lib1 and Lib2
 *
 * This creates a "diamond dependency" problem where Common is required
 * in two different versions.
 */

// Mock function definitions for different versions
void common_v1_function(void) {
    printf("Common v1.0.0 function called\n");
}

void common_v2_function(void) {
    printf("Common v2.0.0 function called\n");
}

void lib1_function(void) {
    printf("Lib1 function called, using Common v1.0.0\n");
    common_v1_function();
}

void lib2_function(void) {
    printf("Lib2 function called, using Common v2.0.0\n");
    common_v2_function();
}

// Test function for context-aware resolution
void test_context_aware_resolution(NexusSymbolRegistry* registry) {
    printf("\nTesting context-aware symbol resolution...\n");
    
    // Setting up the context-aware resolution
    NexusSymbol* result;
    
    // Add metadata for dependencies
    // Lib1 depends on Common v1.0.0
    nexus_add_dependency_constraint(registry, "Lib1", "Common", "1.0.0");
    
    // Lib2 depends on Common v2.0.0
    nexus_add_dependency_constraint(registry, "Lib2", "Common", "2.0.0");
    
    // When resolving in the context of Lib1, we should get Common v1.0.0
    void* common_fn_lib1_context = nexus_resolve_symbol_in_context(registry, "common_function", "Lib1");
    assert(common_fn_lib1_context == (void*)&common_v1_function);
    printf("In Lib1 context: resolved common_function to v1.0.0 ✓\n");
    
    // When resolving in the context of Lib2, we should get Common v2.0.0
    void* common_fn_lib2_context = nexus_resolve_symbol_in_context(registry, "common_function", "Lib2");
    assert(common_fn_lib2_context == (void*)&common_v2_function);
    printf("In Lib2 context: resolved common_function to v2.0.0 ✓\n");
    
    // Call the functions through the resolved pointers
    void (*common_fn_lib1)(void) = (void (*)(void))common_fn_lib1_context;
    void (*common_fn_lib2)(void) = (void (*)(void))common_fn_lib2_context;
    
    common_fn_lib1();
    common_fn_lib2();
    
    printf("Context-aware resolution test passed!\n");
}

// Test function for direct resolution (no context)
void test_direct_resolution(NexusSymbolRegistry* registry) {
    printf("\nTesting direct symbol resolution (no context)...\n");
    
    // Without context, we should get the highest version (v2.0.0)
    void* common_fn_direct = nexus_symbol_resolve(registry, "common_function");
    assert(common_fn_direct == (void*)&common_v2_function);
    printf("Direct resolution: resolved common_function to v2.0.0 (highest version) ✓\n");
    
    // Call the function through the resolved pointer
    void (*common_fn)(void) = (void (*)(void))common_fn_direct;
    common_fn();
    
    printf("Direct resolution test passed!\n");
}

// Test version constraint satisfaction
void test_version_constraints(void) {
    printf("\nTesting version constraint satisfaction...\n");
    
    // Test exact match
    assert(nexus_version_string_satisfies("1.2.3", "1.2.3") == true);
    assert(nexus_version_string_satisfies("1.2.3", "1.2.4") == false);
    printf("Exact match tests passed ✓\n");
    
    // Test range operators
    assert(nexus_version_string_satisfies("2.0.0", ">1.0.0") == true);
    assert(nexus_version_string_satisfies("1.0.0", ">1.0.0") == false);
    assert(nexus_version_string_satisfies("1.0.0", ">=1.0.0") == true);
    assert(nexus_version_string_satisfies("0.9.0", "<1.0.0") == true);
    assert(nexus_version_string_satisfies("1.0.0", "<1.0.0") == false);
    assert(nexus_version_string_satisfies("1.0.0", "<=1.0.0") == true);
    printf("Range operator tests passed ✓\n");
    
    // Test caret operator (^)
    assert(nexus_version_string_satisfies("1.2.3", "^1.2.0") == true);
    assert(nexus_version_string_satisfies("1.3.0", "^1.2.0") == true);
    assert(nexus_version_string_satisfies("2.0.0", "^1.2.0") == false);
    printf("Caret operator tests passed ✓\n");
    
    // Test tilde operator (~)
    assert(nexus_version_string_satisfies("1.2.3", "~1.2.0") == true);
    assert(nexus_version_string_satisfies("1.2.9", "~1.2.0") == true);
    assert(nexus_version_string_satisfies("1.3.0", "~1.2.0") == false);
    printf("Tilde operator tests passed ✓\n");
    
    // Test wildcard
    assert(nexus_version_string_satisfies("1.2.3", "*") == true);
    assert(nexus_version_string_satisfies("2.0.0", "*") == true);
    printf("Wildcard tests passed ✓\n");
    
    printf("All version constraint tests passed!\n");
}

// Main test function
int main(void) {
    printf("NexusLink Diamond Dependency Resolution Test\n");
    printf("===========================================\n");
    
    // Initialize NexusLink
    NexusResult result = nexus_initialize();
    assert(result == NEXUS_SUCCESS);
    
    // Create a symbol registry
    NexusSymbolRegistry* registry = nexus_symbol_registry_create();
    
    // Add Common v1.0.0 symbols
    nexus_symbol_add(&registry->exported, "common_function", "1.0.0", 
                   (void*)&common_v1_function, NEXUS_SYMBOL_FUNCTION, "Common");
    
    // Add Common v2.0.0 symbols
    nexus_symbol_add(&registry->exported, "common_function", "2.0.0", 
                   (void*)&common_v2_function, NEXUS_SYMBOL_FUNCTION, "Common");
    
    // Add Lib1 symbols
    nexus_symbol_add(&registry->exported, "lib1_function", "1.0.0", 
                   (void*)&lib1_function, NEXUS_SYMBOL_FUNCTION, "Lib1");
    
    // Add Lib2 symbols
    nexus_symbol_add(&registry->exported, "lib2_function", "1.0.0", 
                   (void*)&lib2_function, NEXUS_SYMBOL_FUNCTION, "Lib2");
    
    // Run test cases
    test_version_constraints();
    test_direct_resolution(registry);
    test_context_aware_resolution(registry);
    
    // Clean up
    nexus_symbol_registry_free(registry);
    nexus_shutdown();
    
    printf("\nAll diamond dependency resolution tests passed successfully!\n");
    return 0;
}