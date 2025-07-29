// main.c - Basic example of using NexusLink
// Author: Nnamdi Michael Okpala
#include <stdio.h>
#include "../../include/nexus_core.h"
#include "../../include/nexus_symbols.h"
#include "../../include/nexus_loader.h"

int main() {
    printf("NexusLink Basic Example\n");
    printf("=======================\n");
    
    // Initialize NexusLink
    NexusResult result = nexus_initialize();
    if (result != NEXUS_SUCCESS) {
        fprintf(stderr, "Failed to initialize NexusLink\n");
        return 1;
    }
    
    printf("NexusLink initialized successfully (version %s)\n", nexus_version());
    
    // Create a symbol registry
    NexusSymbolRegistry* registry = nexus_symbol_registry_create();
    printf("Symbol registry created\n");
    
    // Create a handle registry
    NexusHandleRegistry* handle_registry = nexus_handle_registry_create();
    printf("Handle registry created\n");
    
    // Add a symbol to the global table
    result = nexus_symbol_add(&registry->global, "printf", "1.0.0", 
                            (void*)&printf, NEXUS_SYMBOL_FUNCTION, "libc");
    if (result == NEXUS_SUCCESS) {
        printf("Added printf symbol to global table\n");
    }
    
    // Resolve the symbol
    void* printf_fn = nexus_symbol_resolve(registry, "printf");
    if (printf_fn) {
        printf("Resolved printf symbol successfully\n");
        
        // Call the function through the resolved pointer
        int (*printf_ptr)(const char*, ...) = (int (*)(const char*, ...))printf_fn;
        printf_ptr("This message is printed via the resolved symbol\n\n");
    }
    
    // Clean up
    nexus_symbol_registry_free(registry);
    nexus_handle_registry_free(handle_registry);
    nexus_shutdown();
    
    printf("NexusLink shutdown successfully\n");
    return 0;
}
