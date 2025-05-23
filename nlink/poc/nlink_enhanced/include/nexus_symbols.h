// nexus_symbols.h - Symbol table management for NexusLink
#ifndef NEXUS_SYMBOLS_H
#define NEXUS_SYMBOLS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// Symbol types
typedef enum {
    SYMBOL_FUNCTION,
    SYMBOL_VARIABLE,
    SYMBOL_TYPE
} SymbolType;

// Symbol structure
typedef struct {
    char* name;
    void* address;
    SymbolType type;
    char* component_id;  // Which component provides this symbol
    int ref_count;       // For usage tracking
} Symbol;

// Symbol table structure
typedef struct SymbolTable {
    Symbol* symbols;
    size_t capacity;
    size_t size;
} SymbolTable;

// The three-tier symbol table system
typedef struct {
    SymbolTable global;     // Global symbols available throughout application lifecycle
    SymbolTable imported;   // Symbols required by currently loaded components
    SymbolTable exported;   // Symbols provided by currently loaded components
} NexusSymbolRegistry;

// Initialize a symbol table
void symbol_table_init(SymbolTable* table, size_t initial_capacity) {
    table->symbols = (Symbol*)malloc(initial_capacity * sizeof(Symbol));
    table->capacity = initial_capacity;
    table->size = 0;
}

// Initialize the symbol registry
NexusSymbolRegistry* nexus_symbol_registry_create() {
    NexusSymbolRegistry* registry = (NexusSymbolRegistry*)malloc(sizeof(NexusSymbolRegistry));
    
    // Initialize all three tables
    symbol_table_init(&registry->global, 64);
    symbol_table_init(&registry->imported, 128);
    symbol_table_init(&registry->exported, 128);
    
    return registry;
}

// Add a symbol to a table
void symbol_table_add(SymbolTable* table, const char* name, void* address, 
                      SymbolType type, const char* component_id) {
    // Check if we need to resize
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol*)realloc(table->symbols, table->capacity * sizeof(Symbol));
    }
    
    // Add the new symbol
    Symbol* symbol = &table->symbols[table->size++];
    symbol->name = strdup(name);
    symbol->address = address;
    symbol->type = type;
    symbol->component_id = strdup(component_id);
    symbol->ref_count = 0;
}

// Find a symbol in a table
Symbol* symbol_table_find(SymbolTable* table, const char* name) {
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

// Resolve a symbol across all tables (prioritizes local over global)
void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name) {
    // First check the exported table (highest priority)
    Symbol* symbol = symbol_table_find(&registry->exported, name);
    if (symbol) {
        symbol->ref_count++; // Track usage
        return symbol->address;
    }
    
    // Then check the imported table
    symbol = symbol_table_find(&registry->imported, name);
    if (symbol) {
        symbol->ref_count++; // Track usage
        return symbol->address;
    }
    
    // Finally check the global table
    symbol = symbol_table_find(&registry->global, name);
    if (symbol) {
        symbol->ref_count++; // Track usage
        return symbol->address;
    }
    
    // Symbol not found
    return NULL;
}

// Track which components are using which symbols
// Note: This function intentionally doesn't use registry parameter to avoid usage tracking in global registry
// for specialized use cases. Registry information can be derived from symbol->component_id.
void nexus_track_symbol_usage(NexusSymbolRegistry* registry, const char* symbol_name, 
                             const char* using_component) {
    (void)registry; // Silence unused parameter warning
    
    // This is a simplified version that just logs the usage
    // In a production environment, this would record in a usage graph
    printf("[SYMBOL USAGE] Component '%s' is using symbol '%s'\n", using_component, symbol_name);
}

// Enhanced symbol lookup that provides type safety
void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, const char* name, 
                                   SymbolType expected_type, const char* using_component) {
    void* address = NULL;
    
    // Check exported table
    Symbol* symbol = symbol_table_find(&registry->exported, name);
    if (symbol && symbol->type == expected_type) {
        symbol->ref_count++;
        address = symbol->address;
        nexus_track_symbol_usage(registry, name, using_component);
        return address;
    }
    
    // Check imported table
    symbol = symbol_table_find(&registry->imported, name);
    if (symbol && symbol->type == expected_type) {
        symbol->ref_count++;
        address = symbol->address;
        nexus_track_symbol_usage(registry, name, using_component);
        return address;
    }
    
    // Check global table
    symbol = symbol_table_find(&registry->global, name);
    if (symbol && symbol->type == expected_type) {
        symbol->ref_count++;
        address = symbol->address;
        nexus_track_symbol_usage(registry, name, using_component);
        return address;
    }
    
    // Symbol not found or type mismatch
    return NULL;
}

// Handle the diamond dependency problem by implementing context-specific symbol resolution
void* nexus_context_aware_resolve(NexusSymbolRegistry* registry, const char* name, 
                                 const char* context, const char* using_component) {
    // In a real implementation, this would use the context to determine which
    // symbol to return in case of conflicts (e.g., multiple versions)
    
    // For now, just track usage and delegate to standard resolution
    printf("[CONTEXT RESOLUTION] Resolving '%s' in context '%s' for component '%s'\n", 
           name, context, using_component);
    
    return nexus_resolve_symbol(registry, name);
}

// Remove a symbol from a table
bool symbol_table_remove(SymbolTable* table, const char* name) {
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            // Free memory
            free(table->symbols[i].name);
            free(table->symbols[i].component_id);
            
            // Move the last element to this position (if not already the last)
            if (i < table->size - 1) {
                table->symbols[i] = table->symbols[table->size - 1];
            }
            
            // Decrease size
            table->size--;
            return true;
        }
    }
    return false;
}

// Update a symbol's address
bool symbol_table_update_address(SymbolTable* table, const char* name, void* new_address) {
    Symbol* symbol = symbol_table_find(table, name);
    if (symbol) {
        symbol->address = new_address;
        return true;
    }
    return false;
}

// Copy symbols between tables
void symbol_table_copy_symbols(SymbolTable* dest, const SymbolTable* src, const char* component_filter) {
    for (size_t i = 0; i < src->size; i++) {
        Symbol* symbol = &src->symbols[i];
        
        // If a component filter is specified, only copy symbols from that component
        if (component_filter && strcmp(symbol->component_id, component_filter) != 0) {
            continue;
        }
        
        // Add symbol to destination table
        symbol_table_add(dest, symbol->name, symbol->address, symbol->type, symbol->component_id);
        
        // Copy reference count
        Symbol* new_symbol = symbol_table_find(dest, symbol->name);
        if (new_symbol) {
            new_symbol->ref_count = symbol->ref_count;
        }
    }
}

// Get all symbols for a specific component
size_t symbol_table_get_component_symbols(SymbolTable* table, const char* component_id, 
                                         Symbol*** symbols_out) {
    // Count symbols for this component
    size_t count = 0;
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].component_id, component_id) == 0) {
            count++;
        }
    }
    
    if (count == 0) {
        *symbols_out = NULL;
        return 0;
    }
    
    // Allocate result array
    *symbols_out = (Symbol**)malloc(count * sizeof(Symbol*));
    
    // Fill result array
    size_t j = 0;
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].component_id, component_id) == 0) {
            (*symbols_out)[j++] = &table->symbols[i];
        }
    }
    
    return count;
}

// Free a symbol table
void symbol_table_free(SymbolTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        free(table->symbols[i].name);
        free(table->symbols[i].component_id);
    }
    free(table->symbols);
    table->symbols = NULL;
    table->size = 0;
    table->capacity = 0;
}

// Free the symbol registry
void nexus_symbol_registry_free(NexusSymbolRegistry* registry) {
    symbol_table_free(&registry->global);
    symbol_table_free(&registry->imported);
    symbol_table_free(&registry->exported);
    free(registry);
}

// Print symbol table statistics
void symbol_table_print_stats(const SymbolTable* table, const char* table_name) {
    printf("Symbol Table: %s\n", table_name);
    printf("  Total symbols: %zu\n", table->size);
    printf("  Capacity: %zu\n", table->capacity);
    
    // Count by type
    size_t func_count = 0, var_count = 0, type_count = 0;
    for (size_t i = 0; i < table->size; i++) {
        switch (table->symbols[i].type) {
            case SYMBOL_FUNCTION: func_count++; break;
            case SYMBOL_VARIABLE: var_count++; break;
            case SYMBOL_TYPE: type_count++; break;
        }
    }
    
    printf("  Functions: %zu\n", func_count);
    printf("  Variables: %zu\n", var_count);
    printf("  Types: %zu\n", type_count);
    
    // Find most referenced symbol
    size_t max_refs = 0;
    const char* most_used = NULL;
    for (size_t i = 0; i < table->size; i++) {
        if ((size_t)table->symbols[i].ref_count > max_refs) {
            max_refs = table->symbols[i].ref_count;
            most_used = table->symbols[i].name;
        }
    }
    
    if (most_used) {
        printf("  Most referenced symbol: %s (%zu references)\n", most_used, max_refs);
    }
}

// Generate a dependency graph based on symbol usage
void nexus_generate_symbol_dependency_graph(NexusSymbolRegistry* registry, const char* output_file) {
    FILE* file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file for symbol dependency graph: %s\n", output_file);
        return;
    }
    
    // Write DOT format graph header
    fprintf(file, "digraph SymbolDependencies {\n");
    fprintf(file, "  rankdir=LR;\n");
    fprintf(file, "  node [shape=box, style=filled, fillcolor=lightblue];\n\n");
    
    // Create a set of unique component IDs
    char** components = NULL;
    size_t component_count = 0;
    
    // Collect components from all tables
    for (int table_idx = 0; table_idx < 3; table_idx++) {
        SymbolTable* table;
        switch (table_idx) {
            case 0: table = &registry->global; break;
            case 1: table = &registry->imported; break;
            case 2: table = &registry->exported; break;
            default: continue;
        }
        
        for (size_t i = 0; i < table->size; i++) {
            const char* component_id = table->symbols[i].component_id;
            bool found = false;
            
            // Check if we already have this component
            for (size_t j = 0; j < component_count; j++) {
            if (strcmp(components[j], component_id) == 0) {
                found = true;
                break;
            }
            }
            
            // Add new component
            if (!found) {
                component_count++;
                components = (char**)realloc(components, component_count * sizeof(char*));
                components[component_count - 1] = strdup(component_id);
            }
        }
    }
    
    // Write component nodes
    for (size_t i = 0; i < component_count; i++) {
        fprintf(file, "  \"%s\" [label=\"%s\"];\n", components[i], components[i]);
    }
    
    fprintf(file, "\n");
    
    // Write symbol dependencies
    // In a real implementation, this would use actual collected dependency data
    // For this demo, we'll just use the imported/exported tables
    
    // For each imported symbol, draw a link from the importing component to the exporting component
    for (size_t i = 0; i < registry->imported.size; i++) {
        Symbol* import = &registry->imported.symbols[i];
        
        // Find the exporting component
        Symbol* exported_symbol = symbol_table_find(&registry->exported, import->name);
        if (exported_symbol) {
            fprintf(file, "  \"%s\" -> \"%s\" [label=\"%s\"];\n", 
                   import->component_id, exported_symbol->component_id, import->name);
        }
    }
    
    // Close the graph
    fprintf(file, "}\n");
    fclose(file);
    
    // Free component list
    for (size_t i = 0; i < component_count; i++) {
        free(components[i]);
    }
    free(components);
    
printf("Symbol dependency graph written to %s\n", output_file);
}

// Declare the implementation as external since it's defined in cold.c
extern void cold_function_impl(int x);

// Declare the load function for cold_function
extern void load_cold_function(void);

#endif // NEXUS_SYMBOLS_H