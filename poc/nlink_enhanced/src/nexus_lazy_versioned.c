// src/nexus_lazy_versioned.c
// Implementation of version-aware lazy loading system
// Author: Implementation Team

#include "../include/nexus_lazy_versioned.h"
#include "../include/nexus_lazy.h"  // For handle registry functions
#include <string.h>
#include <stdio.h>  // For printf


// External declaration for nexus_find_handle and nexus_register_handle
extern void* nexus_find_handle(const char* path);
extern void nexus_register_handle(void* handle, const char* path);

// Check for unused libraries and potentially unload them
void nexus_check_unused_versioned_libraries(VersionedSymbolRegistry* registry) {
    if (!registry) return;
    
    // Get handle registry information (assuming we have access to it)
    extern NexusHandleRegistry* nexus_handle_registry;
    if (!nexus_handle_registry) return;
    
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    // This is a simplified implementation
    printf("Checking for unused libraries in versioned context...\n");
    
    // In a real implementation, we would:
    // 1. Identify components that haven't been used for a while
    // 2. Check if any symbols from those components are still referenced
    // 3. Unload any components that are no longer needed
    
    // For this demo, we just print information
    printf("NexusLink would unload components that haven't been used for %d seconds\n", 
           nexus_versioned_lazy_config.unload_timeout_sec);
    printf("Currently tracking %zu libraries\n", nexus_handle_registry->count);
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
}

// Utility to print version information for a symbol
void nexus_print_symbol_version_info(const char* symbol_name, const VersionInfo* info) {
    if (!symbol_name || !info) {
        printf("No version information available\n");
        return;
    }
    
    printf("Version info for symbol '%s':\n", symbol_name);
    printf("  Resolved version: %s\n", info->resolved_version ? info->resolved_version : "unknown");
    printf("  Provided by: %s\n", info->providing_component ? info->providing_component : "unknown");
    printf("  Exact match: %s\n", info->is_exact_match ? "yes" : "no");
}

// Helper function to get component-specific version constraint for a symbol
// Returns default constraint if no specific constraint is found
const char* nexus_get_component_version_constraint(
    VersionedSymbolRegistry* registry,
    const char* symbol_name,
    const char* component_id,
    const char* default_constraint
) {
    if (!registry || !symbol_name || !component_id) 
        return default_constraint;
    
    // In a real implementation, we would check the component's metadata
    // to see if it has a specific version constraint for this symbol.
    // For this demo, we'll just return the default constraint.
    
    return default_constraint;
}

// Enhance existing symbol tracking with version information
void nexus_track_symbol_usage_versioned(
    VersionedSymbolRegistry* registry,
    const char* symbol_name,
    const char* version,
    const char* using_component
) {
    if (!registry || !symbol_name || !using_component) return;
    
    printf("[VERSIONED SYMBOL USAGE] Component '%s' is using symbol '%s'", 
           using_component, symbol_name);
    
    if (version) {
        printf(" version '%s'", version);
    }
    
    printf("\n");
}