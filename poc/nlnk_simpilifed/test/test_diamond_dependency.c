// test/test_diamond_dependency.c
// Test for NexusLink's diamond dependency resolution
// Author: Nnamdi Michael Okpala

#include <stdio.h>
#include "../include/nexus_versioned_symbols.h"
/**
 * @file test_diamond_dependency.c
 * @brief Test case demonstrating resolution of diamond dependency problem in nlink.
 *
 * This file tests how nlink handles the "diamond dependency problem" where an application
 * depends on two libraries that both depend on different versions of a common library.
 *
 * Example diamond dependency structure:
 *         App
 *        /   \
 *       /     \
 * LibMath1    LibStats
 *       \     /
 *        \   /
 *       LibCore
 *
 * In this scenario, LibMath1 depends on LibCore v1.0 and LibStats depends on
 * LibCore v2.0. Without proper version management, this would lead to symbol
 * conflicts and potential runtime errors.
 *
 * The test uses mock implementations of log_base10 function to simulate different
 * versions of a library function, demonstrating how nlink properly resolves
 * which implementation to use based on dependency requirements.
 */

// Mock library functions (different versions)
int log_base10_v1(double x) { return (int)x; }  // Simplified mock version 1.0
int log_base10_v2(double x) { return (int)(x * 2); }  // Enhanced mock version 2.0

// Mock dependency graph:
//
//         App
//        /   \
//       /     \
// LibMath1    LibStats
//       \     /
//        \   /
//       LibCore
//
// Diamond dependency: LibMath1 and LibStats both depend on different versions of LibCore

int main() {
    printf("NexusLink Diamond Dependency Test\n");
    printf("=================================\n\n");
    
    // Create versioned symbol registry
    VersionedSymbolRegistry* registry = nexus_versioned_registry_create();
    
    printf("Setting up test dependency graph...\n");
    
    // Add component dependencies
    nexus_add_component_dependency(registry, "App", "LibMath1", "^1.0.0", false);
    nexus_add_component_dependency(registry, "App", "LibStats", "^1.0.0", false);
    nexus_add_component_dependency(registry, "LibMath1", "LibCore", "^1.0.0", false);
    nexus_add_component_dependency(registry, "LibStats", "LibCore", "^2.0.0", false);
    
    printf("Adding symbols to registry...\n");
    
    // LibCore v1.0.0 provides log_base10
    versioned_symbol_table_add(&registry->exported, "log_base10", "1.0.0", 
                              (void*)log_base10_v1, VSYMBOL_FUNCTION, "LibCore", 10);
    
    // LibCore v2.0.0 also provides log_base10 (different implementation)
    versioned_symbol_table_add(&registry->exported, "log_base10", "2.0.0", 
                              (void*)log_base10_v2, VSYMBOL_FUNCTION, "LibCore", 10);
    
    printf("\nTest scenario ready.\n");
    printf("LibCore provides log_base10 in two versions:\n");
    printf("  - Version 1.0.0 (used by LibMath1)\n");
    printf("  - Version 2.0.0 (used by LibStats)\n\n");
    
    // Test 1: App accesses log_base10 through LibMath1
    printf("Test 1: App accessing log_base10 in context of LibMath1\n");
    void* func1 = nexus_resolve_versioned_symbol(registry, "log_base10", NULL, "LibMath1");
    if (func1) {
        int (*log_func1)(double) = (int (*)(double))func1;
        int result1 = log_func1(10.0);
        printf("Result from LibMath1's log_base10(10.0): %d\n", result1);
    } else {
        printf("Failed to resolve log_base10 for LibMath1\n");
    }
    
    // Test 2: App accesses log_base10 through LibStats
    printf("\nTest 2: App accessing log_base10 in context of LibStats\n");
    void* func2 = nexus_resolve_versioned_symbol(registry, "log_base10", NULL, "LibStats");
    if (func2) {
        int (*log_func2)(double) = (int (*)(double))func2;
        int result2 = log_func2(10.0);
        printf("Result from LibStats's log_base10(10.0): %d\n", result2);
    } else {
        printf("Failed to resolve log_base10 for LibStats\n");
    }
    
    // Test 3: App directly accesses log_base10 (should use highest available version)
    printf("\nTest 3: App directly accessing log_base10 (ambiguous context)\n");
    void* func3 = nexus_resolve_versioned_symbol(registry, "log_base10", NULL, "App");
    if (func3) {
        int (*log_func3)(double) = (int (*)(double))func3;
        int result3 = log_func3(10.0);
        printf("Result from App's log_base10(10.0): %d\n", result3);
    } else {
        printf("Failed to resolve log_base10 for App\n");
    }
    
    // Test 4: Check for version conflicts
    printf("\nTest 4: Checking for version conflicts\n");
    char* conflict_details = NULL;
    bool has_conflicts = nexus_detect_version_conflicts(registry, "App", &conflict_details);
    
    if (has_conflicts) {
        printf("Version conflicts detected:\n%s\n", conflict_details);
        free(conflict_details);
    } else {
        printf("No version conflicts detected\n");
    }
    
    // Test 5: Generate dependency graph
    printf("\nTest 5: Generating dependency graph\n");
    char* graph = nexus_generate_dependency_graph(registry);
    printf("Dependency graph in DOT format:\n\n%s\n", graph);
    free(graph);
    
    // Clean up
    nexus_versioned_registry_free(registry);
    
    printf("\nTest completed.\n");
    return 0;
}