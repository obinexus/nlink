/**
 * @file registry.h
 * @brief Symbol registry management for NexusLink
 * 
 * This header defines the symbol registry functionality, which is responsible
 * for managing symbols across different components of the NexusLink system.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_SYMBOLS_REGISTRY_H
 #define NLINK_SYMBOLS_REGISTRY_H
 
 #include "nlink/core/common//types.h"
 #include "nlink/core/common//result.h"
 #include <stddef.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdbool.h>
    #include <time.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Symbol structure
  * 
  * This structure represents a symbol in the NexusLink system.
  */
 struct NexusSymbol {
     char* name;              /**< Symbol name */
     void* address;           /**< Memory address of the symbol */
     NexusSymbolType type;    /**< Symbol type */
     char* component_id;      /**< ID of the component that provides this symbol */
     int ref_count;           /**< Reference count for usage tracking */
 };
 
 /**
  * @brief Symbol table structure
  * 
  * This structure represents a table of symbols in the NexusLink system.
  */
 struct NexusSymbolTable {
     NexusSymbol* symbols;    /**< Array of symbols */
     size_t capacity;         /**< Capacity of the symbols array */
     size_t size;             /**< Number of symbols in the table */
 };
 
 /**
  * @brief Symbol registry structure
  * 
  * This structure represents the three-tier symbol registry in the NexusLink system.
  */
 struct NexusSymbolRegistry {
     NexusSymbolTable global;     /**< Global symbols available throughout application lifecycle */
     NexusSymbolTable imported;   /**< Symbols imported by currently loaded components */
     NexusSymbolTable exported;   /**< Symbols exported by currently loaded components */
 };
 
 /**
  * @brief Initialize a symbol registry
  * 
  * @return NexusSymbolRegistry* A newly allocated symbol registry, or NULL on failure
  */
 NexusSymbolRegistry* nexus_init_symbol_registry(void);
 
 /**
  * @brief Initialize a symbol table
  * 
  * @param table The table to initialize
  * @param initial_capacity The initial capacity of the table
  */
 void nexus_symbol_table_init(NexusSymbolTable* table, size_t initial_capacity);
 
 /**
  * @brief Add a symbol to a symbol table
  * 
  * @param table The table to add the symbol to
  * @param name The name of the symbol
  * @param address The memory address of the symbol
  * @param type The type of the symbol
  * @param component_id The ID of the component that provides the symbol
  * @return NexusResult The result of the operation
  */
 NexusResult nexus_symbol_table_add(NexusSymbolTable* table, 
                                    const char* name, 
                                    void* address, 
                                    NexusSymbolType type, 
                                    const char* component_id);
 
 /**
  * @brief Find a symbol in a symbol table
  * 
  * @param table The table to search
  * @param name The name of the symbol to find
  * @return NexusSymbol* The found symbol, or NULL if not found
  */
 NexusSymbol* nexus_symbol_table_find(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Resolve a symbol using the three-tier registry
  * 
  * @param registry The symbol registry
  * @param name The name of the symbol to resolve
  * @return void* The memory address of the symbol, or NULL if not found
  */
 void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name);
 
 /**
  * @brief Remove a symbol from a symbol table
  * 
  * @param table The table to remove the symbol from
  * @param name The name of the symbol to remove
  * @return NexusResult The result of the operation
  */
 NexusResult nexus_symbol_table_remove(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Count used symbols in a symbol table
  * 
  * @param table The table to count used symbols in
  * @return size_t The number of used symbols
  */
 size_t nexus_symbol_table_count_used(NexusSymbolTable* table);
 
 /**
  * @brief Print symbol statistics
  * 
  * @param registry The symbol registry
  */
 void nexus_symbol_print_stats(NexusSymbolRegistry* registry);
 
 /**
  * @brief Cleanup a symbol table
  * 
  * @param table The table to clean up
  */
 void nexus_symbol_table_cleanup(NexusSymbolTable* table);
 
 /**
  * @brief Cleanup a symbol registry
  * 
  * @param registry The registry to clean up
  */
 void nexus_cleanup_symbol_registry(NexusSymbolRegistry* registry);
 
 /**
  * @brief Type-safe symbol lookup
  * 
  * @param registry The symbol registry
  * @param name The name of the symbol to lookup
  * @param expected_type The expected type of the symbol
  * @param using_component The component that is using the symbol
  * @return void* The memory address of the symbol, or NULL if not found or type mismatch
  */
 void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, const char* name, 
                                     NexusSymbolType expected_type, const char* using_component);
 
 /**
  * @brief Track symbol usage
  * 
  * @param registry The symbol registry
  * @param symbol_name The name of the symbol being used
  * @param using_component The component that is using the symbol
  */
 void nexus_track_symbol_usage(NexusSymbolRegistry* registry, const char* symbol_name, 
                               const char* using_component);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_SYMBOLS_REGISTRY_H */