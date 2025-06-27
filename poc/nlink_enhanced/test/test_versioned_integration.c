#include <stdio.h>
#include <stdlib.h>
#include "../include/nexus_enhanced_metadata.h"
#include "../include/nexus_lazy_versioned.h"
#include "../include/nexus_versioned_symbols.h"
#include "../include/nexus_semver.h"

// Mock implementations of various versions of a function
int calculate_v1(int x) { return x; }                 // Basic implementation (v1.0.0)
int calculate_v2(int x) { return x * 2; }             // Enhanced implementation (v2.0.0)
int calculate_v3(int x) { return x * 2 + 1; }         // Advanced implementation (v3.0.0)
int calculate_v2_1(int x) { return x * 2 + (x % 2); } // Patched v2 implementation (v2.1.0)

// Global versioned symbol registry
VersionedSymbolRegistry *registry = NULL;

// Function is already defined in nexus_lazy_versioned.c, no need to redeclare it here
// The header file should provide the declaration
// Setup test environment
void setup_test_environment()
{
    printf("Setting up test environment...\n");

    // Create registry and initialize tables
    registry = nexus_versioned_registry_create();

    // Register different versions of the calculate function
    versioned_symbol_table_add(&registry->exported, "calculate", "1.0.0",
                               (void *)calculate_v1, VSYMBOL_FUNCTION, "math_lib_v1", 10);

    versioned_symbol_table_add(&registry->exported, "calculate", "2.0.0",
                               (void *)calculate_v2, VSYMBOL_FUNCTION, "math_lib_v2", 20);

    versioned_symbol_table_add(&registry->exported, "calculate", "2.1.0",
                               (void *)calculate_v2_1, VSYMBOL_FUNCTION, "math_lib_v2_patch", 25);

    versioned_symbol_table_add(&registry->exported, "calculate", "3.0.0",
                               (void *)calculate_v3, VSYMBOL_FUNCTION, "math_lib_v3", 30);

    // Add dependency relationships
    nexus_add_component_dependency(registry, "app_v1", "math_lib_v1", "^1.0.0", false);
    nexus_add_component_dependency(registry, "app_v2", "math_lib_v2", "^2.0.0", false);
    nexus_add_component_dependency(registry, "app_v3", "math_lib_v3", "^3.0.0", false);
    nexus_add_component_dependency(registry, "app_compatible", "math_lib_v2", ">=2.0.0", false);

    // Configure lazy loading
    NexusVersionedLazyConfig config = {
        .library_path = "./dummy_path.so", // Not actually used in this test
        .component_id = "app_v2",          // Default calling context
        .version = "1.0.0",                // Client version
        .version_constraint = "^2.0.0",    // Default version constraint
        .auto_unload = true,
        .unload_timeout_sec = 60,
        .registry = registry};

    nexus_set_versioned_lazy_config(&config);

    printf("Test environment ready.\n\n");
}

// Clean up test environment
void cleanup_test_environment()
{
    printf("\nCleaning up test environment...\n");

    // Free registry
    if (registry)
    {
        nexus_versioned_registry_free(registry);
        registry = NULL;
    }

    printf("Test environment cleaned up.\n");
}

// Create mock metadata for testing
EnhancedComponentMetadata **create_test_components()
{
    EnhancedComponentMetadata **components = malloc(4 * sizeof(EnhancedComponentMetadata *));

    // Math library v1
    components[0] = nexus_enhanced_metadata_create("math_lib_v1", "1.0.0",
                                                   "Basic math library");
    nexus_enhanced_metadata_add_exported_symbol(components[0], "calculate", "1.0.0", 0);

    // Math library v2
    components[1] = nexus_enhanced_metadata_create("math_lib_v2", "2.0.0",
                                                   "Enhanced math library");
    nexus_enhanced_metadata_add_exported_symbol(components[1], "calculate", "2.0.0", 0);

    // Math library v2 patch
    components[2] = nexus_enhanced_metadata_create("math_lib_v2_patch", "2.1.0",
                                                   "Patched math library");
    nexus_enhanced_metadata_add_exported_symbol(components[2], "calculate", "2.1.0", 0);

    // Math library v3
    components[3] = nexus_enhanced_metadata_create("math_lib_v3", "3.0.0",
                                                   "Advanced math library");
    nexus_enhanced_metadata_add_exported_symbol(components[3], "calculate", "3.0.0", 0);

    return components;
}

// Test version-aware symbol resolution
void test_versioned_resolution()
{
    printf("Test: Version-aware symbol resolution\n");
    printf("-------------------------------------\n");

    // Test resolution with specific contexts
    void *sym1 = nexus_resolve_versioned_symbol(registry, "calculate", "^1.0.0", "app_v1");
    void *sym2 = nexus_resolve_versioned_symbol(registry, "calculate", "^2.0.0", "app_v2");
    void *sym3 = nexus_resolve_versioned_symbol(registry, "calculate", "^3.0.0", "app_v3");
    void *sym_compatible = nexus_resolve_versioned_symbol(registry,
                                                          "calculate",
                                                          ">=2.0.0",
                                                          "app_compatible");

    if (sym1 && sym2 && sym3 && sym_compatible)
    {
        int (*calc1)(int) = (int (*)(int))sym1;
        int (*calc2)(int) = (int (*)(int))sym2;
        int (*calc3)(int) = (int (*)(int))sym3;
        int (*calc_compat)(int) = (int (*)(int))sym_compatible;

        printf("Result from app_v1 context: calculate(10) = %d\n", calc1(10));
        printf("Result from app_v2 context: calculate(10) = %d\n", calc2(10));
        printf("Result from app_v3 context: calculate(10) = %d\n", calc3(10));
        printf("Result from app_compatible context: calculate(10) = %d\n", calc_compat(10));

        // Verify which version was chosen for the compatible app
        if (calc_compat == calculate_v3)
        {
            printf("app_compatible resolved to v3.0.0 (highest available)\n");
        }
        else if (calc_compat == calculate_v2_1)
        {
            printf("app_compatible resolved to v2.1.0 (latest v2.x)\n");
        }
        else if (calc_compat == calculate_v2)
        {
            printf("app_compatible resolved to v2.0.0\n");
        }
        else
        {
            printf("Unexpected resolution for app_compatible\n");
        }
    }
    else
    {
        printf("One or more symbols could not be resolved\n");
    }

    printf("\n");
}

// Test metadata integration
void test_metadata_integration()
{
    printf("Test: Metadata integration with versioning\n");
    printf("-----------------------------------------\n");

    // Create test components
    EnhancedComponentMetadata **components = create_test_components();

    // Test version resolution
    const EnhancedComponentMetadata *resolved = nexus_enhanced_metadata_resolve_version(
        "math_lib_v2", "^2.0.0", (const EnhancedComponentMetadata **)components, 4);

    if (resolved)
    {
        printf("Resolved math_lib_v2 ^2.0.0 to version %s\n", resolved->version);
    }
    else
    {
        printf("Failed to resolve math_lib_v2 ^2.0.0\n");
    }

    // Test version compatibility checking
    EnhancedComponentMetadata *app = nexus_enhanced_metadata_create("app_v2", "1.0.0",
                                                                    "Application version 2");
    nexus_enhanced_metadata_add_dependency(app, "math_lib_v2", "^2.0.0", false);

    bool compatible = nexus_enhanced_metadata_check_version_compatibility(app, components[1]);
    printf("app_v2 is %scompatible with math_lib_v2 v2.0.0\n",
           compatible ? "" : "not ");

    compatible = nexus_enhanced_metadata_check_version_compatibility(app, components[2]);
    printf("app_v2 is %scompatible with math_lib_v2_patch v2.1.0\n",
           compatible ? "" : "not ");

    compatible = nexus_enhanced_metadata_check_version_compatibility(app, components[3]);
    printf("app_v2 is %scompatible with math_lib_v3 v3.0.0\n",
           compatible ? "" : "not ");

    // Clean up
    for (int i = 0; i < 4; i++)
    {
        nexus_free_enhanced_metadata(components[i]);
    }
    free(components);
    nexus_free_enhanced_metadata(app);

    printf("\n");
}

// Test conflict detection
void test_conflict_detection()
{
    printf("Test: Version conflict detection\n");
    printf("-------------------------------\n");

    // Create a diamond dependency scenario:
    // app_diamond -> lib_a (requires math_lib ^1.0.0)
    //             -> lib_b (requires math_lib ^2.0.0)

    nexus_add_component_dependency(registry, "app_diamond", "lib_a", "^1.0.0", false);
    nexus_add_component_dependency(registry, "app_diamond", "lib_b", "^1.0.0", false);
    nexus_add_component_dependency(registry, "lib_a", "math_lib_v1", "^1.0.0", false);
    nexus_add_component_dependency(registry, "lib_b", "math_lib_v2", "^2.0.0", false);

    // Detect conflicts
    char *conflict_details = NULL;
    bool has_conflicts = nexus_detect_version_conflicts(registry, "app_diamond", &conflict_details);

    if (has_conflicts && conflict_details)
    {
        printf("Conflicts detected for app_diamond:\n%s\n", conflict_details);
        free(conflict_details);
    }
    else
    {
        printf("No conflicts detected for app_diamond\n");
    }

    // Generate dependency graph
    char *graph = nexus_generate_dependency_graph(registry);
    if (graph)
    {
        printf("Dependency graph (truncated):\n%.120s...\n", graph);
        free(graph);
    }
    else
    {
        printf("Failed to generate dependency graph\n");
    }

    printf("\n");
}

// Demonstration of the NEXUS_LAZY_VERSIONED macro
// Note: This would normally be used with actual shared libraries
// For this demo, we're just showing the macro expansion
void demo_lazy_versioned_macro()
{
    printf("Demo: NEXUS_LAZY_VERSIONED macro\n");
    printf("-------------------------------\n");

    // We can't actually use the macro directly in this test since we're not
    // loading real shared libraries. However, we can show what it would expand to.

    printf("The NEXUS_LAZY_VERSIONED macro would expand to create:\n");
    printf("1. A function type declaration\n");
    printf("2. An implementation pointer\n");
    printf("3. A library handle pointer\n");
    printf("4. Version information tracking\n");
    printf("5. A load function\n");
    printf("6. The actual function implementation\n\n");

    printf("Example usage:\n");
    printf("NEXUS_LAZY_VERSIONED(calculate, int, \"^2.0.0\", int x);\n\n");

    printf("This would define a function 'calculate' that:\n");
    printf("- Automatically loads the shared library when first called\n");
    printf("- Uses the version constraint \"^2.0.0\" when resolving the symbol\n");
    printf("- Tracks usage information for potential unloading\n");
    printf("- Maintains version metadata about the resolved symbol\n\n");

    printf("Caller code would simply use: result = calculate(10);\n");
    printf("The lazy loading mechanics are completely hidden from the caller.\n");
}

int main()
{
    printf("NexusLink Versioned Integration Test\n");
    printf("===================================\n\n");

    setup_test_environment();

    test_versioned_resolution();
    test_metadata_integration();
    test_conflict_detection();
    demo_lazy_versioned_macro();

    cleanup_test_environment();

    return 0;
}