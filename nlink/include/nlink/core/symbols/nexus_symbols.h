/**
 * @file nexus_symbols.h
 * @brief Symbol registry system for NexusLink
 * 
 * Provides a structured approach to symbol management with three-tier
 * registry (global, imported, exported) and reference counting.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_SYMBOLS_H
 #define NEXUS_SYMBOLS_H
 
 #include "nlink/core/common/result.h"
    #include "nlink/core/common/types.h"
    
 #include <stddef.h>
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif

 
 /**
  * @brief Initialize a symbol registry
  * 
  * @return NexusSymbolRegistry* New registry or NULL on failure
  */
 NexusSymbolRegistry* nexus_init_symbol_registry(void);
 
 /**
  * @brief Initialize a symbol table
  * 
  * @param table Table to initialize
  * @param initial_capacity Initial capacity
  */
 void nexus_symbol_table_init(NexusSymbolTable* table, size_t initial_capacity);
 
 /**
  * @brief Add a symbol to a table
  * 
  * @param table Table to add to
  * @param name Symbol name
  * @param address Symbol address
  * @param type Symbol type
  * @param component_id Associated component ID
  * @return NexusResult Operation result
  */
 NexusResult nexus_symbol_table_add(NexusSymbolTable* table, 
                                   const char* name, 
                                   void* address, 
                                   NexusSymbolType type, 
                                   const char* component_id);
 
 /**
  * @brief Find a symbol in a table
  * 
  * @param table Table to search
  * @param name Symbol name to find
  * @return NexusSymbol* Found symbol or NULL
  */
 NexusSymbol* nexus_symbol_table_find(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Resolve a symbol using the registry
  * 
  * @param registry Symbol registry
  * @param name Symbol name to resolve
  * @return void* Symbol address or NULL if not found
  */
 void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name);
 
 /**
  * @brief Remove a symbol from a table
  * 
  * @param table Table to remove from
  * @param name Symbol name to remove
  * @return NexusResult Operation result
  */
 NexusResult nexus_symbol_table_remove(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Lookup a symbol with type checking
  * 
  * @param registry Symbol registry
  * @param name Symbol name to lookup
  * @param expected_type Expected symbol type
  * @param using_component Component that is using the symbol
  * @return void* Symbol address or NULL
  */
 void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, 
                                    const char* name, 
                                    NexusSymbolType expected_type, 
                                    const char* using_component);
 
 /**
  * @brief Context-aware symbol resolution
  * 
  * @param registry Symbol registry
  * @param name Symbol name
  * @param context Resolution context
  * @param using_component Component that is using the symbol
  * @return void* Symbol address or NULL
  */
 void* nexus_context_aware_resolve(NexusSymbolRegistry* registry, 
                                  const char* name, 
                                  const char* context, 
                                  const char* using_component);
 
 /**
  * @brief Count used symbols in a table
  * 
  * @param table Table to count in
  * @return size_t Number of symbols with ref_count > 0
  */
 size_t nexus_symbol_table_count_used(NexusSymbolTable* table);
 
 /**
  * @brief Print symbol registry statistics
  * 
  * @param registry Registry to analyze
  */
 void nexus_symbol_print_stats(NexusSymbolRegistry* registry);
 
 /**
  * @brief Cleanup a symbol table
  * 
  * @param table Table to cleanup
  */
 void nexus_symbol_table_cleanup(NexusSymbolTable* table);
 
 /**
  * @brief Cleanup a symbol registry
  * 
  * @param registry Registry to cleanup
  */
 void nexus_cleanup_symbol_registry(NexusSymbolRegistry* registry);
 
 /**
  * @brief Generate a symbol dependency graph
  * 
  * @param registry Registry to analyze
  * @param output_file File to write DOT graph to
  */
 void nexus_generate_symbol_dependency_graph(NexusSymbolRegistry* registry, const char* output_file);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NEXUS_SYMBOLS_H */