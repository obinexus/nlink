/**
 * @file nexus_symbols.c
 * @brief Symbol registry implementation for NexusLink
 * 
 * Implements the core symbol registry functionality including three-tier
 * symbol management (global, imported, exported), reference counting,
 * and basic symbol resolution.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/symbols/nexus_symbols.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // Constants for symbol tables
 #define NEXUS_DEFAULT_TABLE_SIZE 64
 
 // Initialize a symbol table
 void nexus_symbol_table_init(NexusSymbolTable* table, size_t initial_capacity) {
     if (!table) {
         return;
     }
     
     table->symbols = (NexusSymbol*)malloc(initial_capacity * sizeof(NexusSymbol));
     if (!table->symbols) {
         // In case of allocation failure, set capacity to 0
         table->capacity = 0;
         table->size = 0;
         return;
     }
     
     table->capacity = initial_capacity;
     table->size = 0;
 }
 
 // Initialize a symbol registry
 NexusSymbolRegistry* nexus_init_symbol_registry(void) {
     NexusSymbolRegistry* registry = (NexusSymbolRegistry*)malloc(sizeof(NexusSymbolRegistry));
     if (!registry) {
         return NULL;
     }
     
     // Initialize all tables
     memset(registry, 0, sizeof(NexusSymbolRegistry));
     
     // Set initial capacity for each table
     nexus_symbol_table_init(&registry->global, NEXUS_DEFAULT_TABLE_SIZE);
     nexus_symbol_table_init(&registry->imported, NEXUS_DEFAULT_TABLE_SIZE);
     nexus_symbol_table_init(&registry->exported, NEXUS_DEFAULT_TABLE_SIZE);
     
     return registry;
 }
 
 // Add a symbol to a symbol table
 NexusResult nexus_symbol_table_add(NexusSymbolTable* table, 
                                   const char* name, 
                                   void* address, 
                                   NexusSymbolType type, 
                                   const char* component_id) {
     if (!table || !name || !component_id) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if we need to resize
     if (table->size >= table->capacity) {
         // Double the capacity
         size_t new_capacity = table->capacity * 2;
         if (new_capacity == 0) {
             new_capacity = NEXUS_DEFAULT_TABLE_SIZE;
         }
         
         NexusSymbol* new_symbols = (NexusSymbol*)realloc(table->symbols, 
                                                         new_capacity * sizeof(NexusSymbol));
         if (!new_symbols) {
             return NEXUS_OUT_OF_MEMORY;
         }
         
         table->symbols = new_symbols;
         table->capacity = new_capacity;
     }
     
     // Add the new symbol
     NexusSymbol* symbol = &table->symbols[table->size++];
     symbol->name = strdup(name);
     if (!symbol->name) {
         table->size--; // Revert the size increase
         return NEXUS_OUT_OF_MEMORY;
     }
     
     symbol->address = address;
     symbol->type = type;
     
     symbol->component_id = strdup(component_id);
     if (!symbol->component_id) {
         free(symbol->name);
         table->size--; // Revert the size increase
         return NEXUS_OUT_OF_MEMORY;
     }
     
     symbol->ref_count = 0;
     
     return NEXUS_SUCCESS;
 }
 
 // Find a symbol in a symbol table
 NexusSymbol* nexus_symbol_table_find(NexusSymbolTable* table, const char* name) {
     if (!table || !name) {
         return NULL;
     }
     
     for (size_t i = 0; i < table->size; i++) {
         if (strcmp(table->symbols[i].name, name) == 0) {
             return &table->symbols[i];
         }
     }
     
     return NULL;
 }
 
 // Resolve a symbol using the three-tier registry
 void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name) {
     if (!registry || !name) {
         return NULL;
     }
     
     // First check the exported table (highest priority)
     NexusSymbol* symbol = nexus_symbol_table_find(&registry->exported, name);
     if (symbol) {
         symbol->ref_count++; // Track usage
         return symbol->address;
     }
     
     // Then check the imported table
     symbol = nexus_symbol_table_find(&registry->imported, name);
     if (symbol) {
         symbol->ref_count++; // Track usage
         return symbol->address;
     }
     
     // Finally check the global table
     symbol = nexus_symbol_table_find(&registry->global, name);
     if (symbol) {
         symbol->ref_count++; // Track usage
         return symbol->address;
     }
     
     // Symbol not found
     return NULL;
 }
 
 // Remove a symbol from a table
 NexusResult nexus_symbol_table_remove(NexusSymbolTable* table, const char* name) {
     if (!table || !name) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     for (size_t i = 0; i < table->size; i++) {
         if (strcmp(table->symbols[i].name, name) == 0) {
             // Free allocated memory
             free(table->symbols[i].name);
             free(table->symbols[i].component_id);
             
             // Move the last element to this position (if not already the last)
             if (i < table->size - 1) {
                 table->symbols[i] = table->symbols[table->size - 1];
             }
             
             // Reduce size
             table->size--;
             
             return NEXUS_SUCCESS;
         }
     }
     
     return NEXUS_NOT_FOUND;
 }
 
 // Count used symbols in a table
 size_t nexus_symbol_table_count_used(NexusSymbolTable* table) {
     if (!table) {
         return 0;
     }
     
     size_t count = 0;
     for (size_t i = 0; i < table->size; i++) {
         if (table->symbols[i].ref_count > 0) {
             count++;
         }
     }
     
     return count;
 }
 
 // Print symbol statistics
 void nexus_symbol_print_stats(NexusSymbolRegistry* registry) {
     if (!registry) {
         printf("Invalid registry\n");
         return;
     }
     
     printf("Symbol Registry Statistics:\n");
     printf("---------------------------\n");
     
     printf("Global symbols: %zu/%zu (used: %zu)\n", 
             registry->global.size, 
             registry->global.capacity,
             nexus_symbol_table_count_used(&registry->global));
     
     printf("Imported symbols: %zu/%zu (used: %zu)\n", 
             registry->imported.size, 
             registry->imported.capacity,
             nexus_symbol_table_count_used(&registry->imported));
     
     printf("Exported symbols: %zu/%zu (used: %zu)\n", 
             registry->exported.size, 
             registry->exported.capacity,
             nexus_symbol_table_count_used(&registry->exported));
 }
 
 // Cleanup a symbol table
 void nexus_symbol_table_cleanup(NexusSymbolTable* table) {
     if (!table) {
         return;
     }
     
     // Free all symbol strings
     for (size_t i = 0; i < table->size; i++) {
         free(table->symbols[i].name);
         free(table->symbols[i].component_id);
     }
     
     // Free the symbols array
     free(table->symbols);
     
     // Reset table state
     table->symbols = NULL;
     table->size = 0;
     table->capacity = 0;
 }
 
 // Cleanup a symbol registry
 void nexus_cleanup_symbol_registry(NexusSymbolRegistry* registry) {
     if (!registry) {
         return;
     }
     
     // Cleanup all tables
     nexus_symbol_table_cleanup(&registry->global);
     nexus_symbol_table_cleanup(&registry->imported);
     nexus_symbol_table_cleanup(&registry->exported);
     
     // Free the registry
     free(registry);
 }
 
 // Type-aware symbol lookup
 void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, 
                                    const char* name,
                                    NexusSymbolType expected_type,
                                    const char* using_component) {
     if (!registry || !name || !using_component) {
         return NULL;
     }
     
     // First check exported table
     NexusSymbol* symbol = nexus_symbol_table_find(&registry->exported, name);
     if (symbol && symbol->type == expected_type) {
         symbol->ref_count++;
         return symbol->address;
     }
     
     // Then check imported table
     symbol = nexus_symbol_table_find(&registry->imported, name);
     if (symbol && symbol->type == expected_type) {
         symbol->ref_count++;
         return symbol->address;
     }
     
     // Finally check global table
     symbol = nexus_symbol_table_find(&registry->global, name);
     if (symbol && symbol->type == expected_type) {
         symbol->ref_count++;
         return symbol->address;
     }
     
     // Symbol not found or type mismatch
     return NULL;
 }
 
 // Context-aware symbol resolution
 void* nexus_context_aware_resolve(NexusSymbolRegistry* registry, 
                                  const char* name,
                                  const char* context,
                                  const char* using_component) {
     if (!registry || !name || !using_component) {
         return NULL;
     }
     
     // In a real implementation, this would use context information to
     // disambiguate between symbols with the same name from different components
     // For now, simply delegate to standard resolution
     return nexus_resolve_symbol(registry, name);
 }
 
 // Generate dependency graph
 void nexus_generate_symbol_dependency_graph(NexusSymbolRegistry* registry, const char* output_file) {
     if (!registry || !output_file) {
         return;
     }
     
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
         NexusSymbolTable* table;
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
                 if (!components) {
                     fprintf(stderr, "Error: Memory allocation failed for component list\n");
                     fclose(file);
                     return;
                 }
                 components[component_count - 1] = strdup(component_id);
             }
         }
     }
     
     // Write component nodes
     for (size_t i = 0; i < component_count; i++) {
         fprintf(file, "  \"%s\" [label=\"%s\"];\n", components[i], components[i]);
     }
     
     fprintf(file, "\n  // Symbol dependencies\n");
     
     // For each imported symbol, draw a link from the importing component to the exporting component
     for (size_t i = 0; i < registry->imported.size; i++) {
         NexusSymbol* import = &registry->imported.symbols[i];
         
         // Find the exporting component
         NexusSymbol* exported_symbol = nexus_symbol_table_find(&registry->exported, import->name);
         
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