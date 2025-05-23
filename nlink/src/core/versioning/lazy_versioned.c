// src/nexus_lazy_versioned.c
// Implementation of version-aware lazy loading system
// Author: Nnamdi Michael Okpala


#include "nlink/core/versioning/nexus_lazy_versioned.h"
#include "nlink/core/versioning/nexus_version.h"
#include "nlink/core/versioning/lazy_versioned.h"


// Enhanced implementation of nexus_check_unused_versioned_libraries
// This implementation properly tracks component usage and performs unloading
// when components have been unused for a specified timeout period

void nexus_check_unused_versioned_libraries(VersionedSymbolRegistry* registry) {
    if (!registry || !nexus_versioned_lazy_config.auto_unload) {
        return;  // Early return if registry is NULL or auto_unload is disabled
    }
    
    // Get current time for timeout calculations
    time_t now = time(NULL);
    
    // Get handle registry information (assuming we have access to it)
    extern NexusHandleRegistry* nexus_handle_registry;
    if (!nexus_handle_registry) {
        return;  // Early return if handle registry is not initialized
    }
    
    // Lock the handle registry for thread safety
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    printf("Checking for unused libraries in versioned context...\n");
    
    // Track libraries to unload (we can't modify the registry while iterating)
    void** libs_to_unload = NULL;
    char** paths_to_unload = NULL;
    size_t unload_count = 0;
    
    // First pass: identify libraries for potential unloading
    for (size_t i = 0; i < nexus_handle_registry->count; i++) {
        void* handle = nexus_handle_registry->handles[i];
        const char* path = nexus_handle_registry->paths[i];
        const char* component_id = nexus_handle_registry->components[i];
        bool can_unload = true;
        time_t last_used_time = 0;
        
        // Check if any symbols from this component are still in use
        for (size_t j = 0; j < registry->exported.size; j++) {
            VersionedSymbol* symbol = &registry->exported.symbols[j];
            
            if (strcmp(symbol->component_id, component_id) == 0) {
                // If symbol is still referenced, we can't unload
                if (symbol->ref_count > 0) {
                    can_unload = false;
                    break;
                }
                
                // Track the most recent usage time
                if (symbol->last_used > last_used_time) {
                    last_used_time = symbol->last_used;
                }
            }
        }
        
        // Check if component has been unused for longer than the timeout
        if (can_unload && 
            last_used_time > 0 && 
            (now - last_used_time) > nexus_versioned_lazy_config.unload_timeout_sec) {
            
            // Add to unload list
            unload_count++;
            libs_to_unload = (void**)realloc(libs_to_unload, unload_count * sizeof(void*));
            paths_to_unload = (char**)realloc(paths_to_unload, unload_count * sizeof(char*));
            
            if (!libs_to_unload || !paths_to_unload) {
                // Memory allocation failure
                free(libs_to_unload);
                free(paths_to_unload);
                pthread_mutex_unlock(&nexus_handle_registry->mutex);
                return;
            }
            
            libs_to_unload[unload_count - 1] = handle;
            paths_to_unload[unload_count - 1] = strdup(path);
            
            printf("Component '%s' will be unloaded (unused for %ld seconds)\n", 
                   component_id, (now - last_used_time));
        }
    }
    
    // Second pass: unload identified libraries
    if (unload_count > 0) {
        printf("Unloading %zu unused libraries\n", unload_count);
        
        for (size_t i = 0; i < unload_count; i++) {
            void* handle = libs_to_unload[i];
            
            // Find index in registry
            size_t registry_index = SIZE_MAX;
            for (size_t j = 0; j < nexus_handle_registry->count; j++) {
                if (nexus_handle_registry->handles[j] == handle) {
                    registry_index = j;
                    break;
                }
            }
            
            if (registry_index != SIZE_MAX) {
                // Get component ID before we remove it from registry
                const char* component_id = nexus_handle_registry->components[registry_index];
                
                // Remove symbols from this component in exported table
                for (size_t j = 0; j < registry->exported.size; j++) {
                    VersionedSymbol* symbol = &registry->exported.symbols[j];
                    
                    if (strcmp(symbol->component_id, component_id) == 0) {
                        // Mark for removal (will be cleaned up later)
                        // In a real implementation, we'd need to handle this more efficiently
                        free(symbol->name);
                        free(symbol->version);
                        free(symbol->component_id);
                        symbol->name = NULL;
                        symbol->address = NULL;
                    }
                }
                
                // Unload the library
                dlclose(handle);
                
                // Remove from handle registry by swapping with the last element
                free(nexus_handle_registry->paths[registry_index]);
                free(nexus_handle_registry->components[registry_index]);
                
                if (registry_index < nexus_handle_registry->count - 1) {
                    // Not the last element, swap with last
                    nexus_handle_registry->handles[registry_index] = 
                        nexus_handle_registry->handles[nexus_handle_registry->count - 1];
                    nexus_handle_registry->paths[registry_index] = 
                        nexus_handle_registry->paths[nexus_handle_registry->count - 1];
                    nexus_handle_registry->components[registry_index] = 
                        nexus_handle_registry->components[nexus_handle_registry->count - 1];
                }
                
                // Decrease registry count
                nexus_handle_registry->count--;
            }
            
            free(paths_to_unload[i]);
        }
        
        // Compact the exported symbols table to remove NULL entries
        size_t write_index = 0;
        for (size_t i = 0; i < registry->exported.size; i++) {
            if (registry->exported.symbols[i].name != NULL) {
                if (i != write_index) {
                    registry->exported.symbols[write_index] = registry->exported.symbols[i];
                }
                write_index++;
            }
        }
        registry->exported.size = write_index;
    }
    
    // Clean up temporary arrays
    free(libs_to_unload);
    free(paths_to_unload);
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
    
    printf("Library unloading complete. %zu libraries remain loaded.\n", 
           nexus_handle_registry->count);
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