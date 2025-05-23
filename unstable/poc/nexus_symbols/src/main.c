// main.c - Revised Implementation
#include <stdio.h>
#include "../include/nexus_json.h"
#include "../include/nexus_symbols.h"
#include "../include/nexus_metadata.h"
#include "../include/nexus_lazy.h"

// Global symbol registry
NexusSymbolRegistry* registry = NULL;

// Function prototype
void cold_function(int x);

// Declare the lazy-loaded function
NEXUS_LAZY_DECLARE(cold_function, void, int);

// Define the actual function implementation
void cold_function(int x) {
    // Ensure function is loaded
    load_cold_function();
    
    // Track usage if registry exists
    if (registry) {
        Symbol* symbol = symbol_table_find(&registry->imported, "cold_function");
        if (symbol) {
            symbol->ref_count++;
        }
    }
    
    // Call the implementation
    cold_function_impl(x);
}



// Setup function to initialize the NexusLink environment
void setup_nexus_environment() {
    // Initialize symbol registry
    registry = nexus_symbol_registry_create();
    
    // Add some global symbols for demonstration
    symbol_table_add(&registry->global, "printf", (void*)printf, SYMBOL_FUNCTION, "libc");
    
    // Track the cold_function import
    symbol_table_add(&registry->imported, "cold_function", NULL, SYMBOL_FUNCTION, "cold_component");
    
    // Output some debug info
    printf("NexusLink environment initialized\n");
    printf("Symbol tables created with capacities: global=%zu, imported=%zu, exported=%zu\n", 
           registry->global.capacity, registry->imported.capacity, registry->exported.capacity);
}

// Clean up function to free resources
void cleanup_nexus_environment() {
    if (registry) {
        nexus_symbol_registry_free(registry);
        registry = NULL;
    }
    printf("NexusLink environment cleaned up\n");
}

// Function to demonstrate metadata usage
void demo_metadata() {
    printf("\n=== Metadata System Demonstration ===\n");

    // Create a new metadata object for our cold component
    ComponentMetadata* metadata = nexus_metadata_create("cold_component", "0.1.0", 
                                                       "Demonstration of lazy-loaded cold function component");
    
    // Add some dependencies
    nexus_metadata_add_dependency(metadata, "libc", "2.31", false);  // Required dependency
    nexus_metadata_add_dependency(metadata, "libmath", "1.0", true); // Optional dependency
    
    // Add exported symbols
    nexus_metadata_add_exported_symbol(metadata, "cold_function");
    
    // Add imported symbols
    nexus_metadata_add_imported_symbol(metadata, "printf");
    
    // Set resource metrics
    metadata->memory_footprint = 2048;  // 2 KB
    metadata->avg_load_time_ms = 1.5;   // 1.5 ms
    
    // Save to a file
    printf("Creating metadata file...\n");
    if (nexus_metadata_save(metadata, "cold_component.json")) {
        printf("Successfully saved metadata to cold_component.json\n");
    } else {
        printf("Failed to save metadata\n");
    }
    
    // Track usage
    nexus_metadata_track_usage(metadata);
    printf("Component usage count: %d\n", metadata->usage_count);
    
    // Free the first metadata instance
    nexus_free_metadata(metadata);
    
    // Now load it back
    printf("\nLoading metadata from file...\n");
    metadata = nexus_load_metadata("cold_component.json");
    if (metadata) {
        printf("Loaded metadata for component: %s (version %s)\n", 
               metadata->id, metadata->version);
        printf("Description: %s\n", metadata->description);
        printf("Exported symbols: %zu\n", metadata->exported_count);
        
        // Display dependencies
        printf("Dependencies:\n");
        for (size_t i = 0; i < metadata->dependencies_count; i++) {
            printf("  - %s (version %s, %s)\n", 
                   metadata->dependencies[i].id,
                   metadata->dependencies[i].version,
                   metadata->dependencies[i].optional ? "optional" : "required");
        }
        
        // Update usage information
        nexus_metadata_track_usage(metadata);
        printf("Component usage count: %d\n", metadata->usage_count);
        
        // Check for recent usage
        if (nexus_metadata_recently_used(metadata, 60)) {
            printf("Component was used recently (within last minute)\n");
        }
        
        // Cleanup
        nexus_free_metadata(metadata);
    } else {
        printf("Failed to load metadata\n");
    }
    
    printf("=== End of Metadata Demonstration ===\n\n");
}

// Function to demonstrate dependency resolution
void demo_dependency_resolution() {
    printf("\n=== Dependency Resolution Demonstration ===\n");
    
    // Create a few metadata objects to simulate available components
    ComponentMetadata* components[3];
    components[0] = nexus_metadata_create("libc", "2.31", "C standard library");
    components[1] = nexus_metadata_create("libui", "1.0", "UI library");
    components[2] = nexus_metadata_create("libnet", "0.9", "Networking library");
    
    // Create a component with dependencies
    ComponentMetadata* app = nexus_metadata_create("myapp", "1.0", "Application with dependencies");
    nexus_metadata_add_dependency(app, "libc", "2.31", false);    // Required, available
    nexus_metadata_add_dependency(app, "libui", "1.0", false);    // Required, available
    nexus_metadata_add_dependency(app, "libdb", "1.2", false);    // Required, not available
    nexus_metadata_add_dependency(app, "libopt", "0.5", true);    // Optional, not available
    
    // Check dependencies
    char* missing = NULL;
    bool deps_ok = nexus_metadata_check_dependencies(app, (const ComponentMetadata**)components, 3, &missing);
    
    if (deps_ok) {
        printf("All required dependencies are satisfied\n");
    } else {
        printf("Missing dependency: %s\n", missing);
        free(missing);
    }
    
    // Cleanup
    for (int i = 0; i < 3; i++) {
        nexus_free_metadata(components[i]);
    }
    nexus_free_metadata(app);
    
    printf("=== End of Dependency Resolution Demonstration ===\n\n");
}

// Function to demonstrate symbol tracking
void demo_symbol_tracking() {
    printf("\n=== Symbol Tracking Demonstration ===\n");
    
    // Add some symbols for tracking
    symbol_table_add(&registry->exported, "init_module", (void*)0x1234, SYMBOL_FUNCTION, "main_component");
    symbol_table_add(&registry->exported, "cleanup_module", (void*)0x5678, SYMBOL_FUNCTION, "main_component");
    
    // Simulate symbol usage
    Symbol* init_symbol = symbol_table_find(&registry->exported, "init_module");
    if (init_symbol) {
        init_symbol->ref_count += 5;  // Simulated 5 calls
        printf("Symbol '%s' from component '%s' has been used %d times\n",
               init_symbol->name, init_symbol->component_id, init_symbol->ref_count);
    }
    
    Symbol* cleanup_symbol = symbol_table_find(&registry->exported, "cleanup_module");
    if (cleanup_symbol) {
        cleanup_symbol->ref_count += 2;  // Simulated 2 calls
        printf("Symbol '%s' from component '%s' has been used %d times\n",
               cleanup_symbol->name, cleanup_symbol->component_id, cleanup_symbol->ref_count);
    }
    
    // Find unused symbols (a real implementation would use this for pruning)
    size_t total_symbols = registry->exported.size;
    size_t unused_symbols = 0;
    
    for (size_t i = 0; i < registry->exported.size; i++) {
        if (registry->exported.symbols[i].ref_count == 0) {
            unused_symbols++;
            printf("Symbol '%s' is unused and could be pruned\n", 
                   registry->exported.symbols[i].name);
        }
    }
    
    printf("Found %zu unused symbols out of %zu total (%zu%%)\n", 
           unused_symbols, total_symbols, 
           total_symbols > 0 ? (unused_symbols * 100) / total_symbols : 0);
    
    printf("=== End of Symbol Tracking Demonstration ===\n\n");
}

int main() {
    printf("NexusLink POC Demo\n");
    
    // Setup NexusLink environment
    setup_nexus_environment();
    
    // Call the lazy-loaded function
    cold_function(42);
    
    // Call it again (should use cached implementation)
    cold_function(1337);
    
    // Let's check the ref count
    Symbol* cold_symbol = symbol_table_find(&registry->imported, "cold_function");
    if (cold_symbol) {
        printf("Cold function has been called %d times\n", cold_symbol->ref_count);
    }
    
    // Clean up
    cleanup_nexus_environment();
    
    return 0;
}