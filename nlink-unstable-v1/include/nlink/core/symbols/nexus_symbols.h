/**
 * @file nexus_symbols.h
 * @brief Symbol management for NexusLink
 * 
 * This header defines the structures and functions for managing symbols in NexusLink.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_SYMBOLS_H
 #define NEXUS_SYMBOLS_H
 
 #include <stddef.h>
 #include "core/common/types.h"
 #include "core/common/result.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // Forward declarations for result codes
 #ifndef NEXUS_RESULT_DEFINED
 #define NEXUS_RESULT_DEFINED
 typedef enum NexusResult NexusResult;
 #endif
 
 // Default sizes
 #define NEXUS_DEFAULT_TABLE_SIZE 64
 
 /**
  * @brief Symbol types for NexusLink
  */
 typedef enum {
     NEXUS_SYMBOL_UNKNOWN,
     NEXUS_SYMBOL_FUNCTION,
     NEXUS_SYMBOL_VARIABLE,
     NEXUS_SYMBOL_TYPE
 } NexusSymbolType;
 
 /**
  * @brief Symbol structure for NexusLink
  */
 struct NexusSymbol {
     char* name;             /**< Symbol name */
     void* address;          /**< Symbol address */
     NexusSymbolType type;   /**< Symbol type */
     char* component_id;     /**< ID of the component that owns the symbol */
     int ref_count;          /**< Reference count */
 };
 
 /**
  * @brief Symbol table structure for NexusLink
  */
 struct NexusSymbolTable {
     NexusSymbol* symbols;  /**< Array of symbols */
     size_t size;           /**< Number of symbols in the table */
     size_t capacity;       /**< Capacity of the table */
 };
 
 /**
  * @brief Symbol registry structure for NexusLink
  * 
  * The registry contains three tables:
  * - global: Symbols available to all components
  * - imported: Symbols imported by components
  * - exported: Symbols exported by components
  */
 struct NexusSymbolRegistry {
     NexusSymbolTable global;    /**< Global symbol table */
     NexusSymbolTable imported;  /**< Imported symbol table */
     NexusSymbolTable exported;  /**< Exported symbol table */
 };
 
 /**
  * @brief Initialize a symbol registry
  * 
  * @return NexusSymbolRegistry* The new registry, or NULL on failure
  */
 NexusSymbolRegistry* nexus_init_symbol_registry(void);
 
 /**
  * @brief Initialize a symbol table
  * 
  * @param table The table to initialize
  * @param initial_capacity Initial capacity for the table
  */
 void nexus_symbol_table_init(NexusSymbolTable* table, size_t initial_capacity);
 
 /**
  * @brief Add a symbol to a symbol table
  * 
  * @param table The table to add to
  * @param name Symbol name
  * @param address Symbol address
  * @param type Symbol type
  * @param component_id ID of the component that owns the symbol
  * @return NexusResult Result code
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
  * @param name Symbol name to find
  * @return NexusSymbol* The symbol, or NULL if not found
  */
 NexusSymbol* nexus_symbol_table_find(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Resolve a symbol using the three-tier registry
  * 
  * @param registry The symbol registry
  * @param name Symbol name to resolve
  * @return void* The symbol address, or NULL if not found
  */
 void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name);
 
 /**
  * @brief Remove a symbol from a table
  * 
  * @param table The table to remove from
  * @param name Symbol name to remove
  * @return NexusResult Result code
  */
 NexusResult nexus_symbol_table_remove(NexusSymbolTable* table, const char* name);
 
 /**
  * @brief Count used symbols in a table
  * 
  * @param table The table to count from
  * @return size_t Number of symbols with ref_count > 0
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
  * @param table The table to cleanup
  */
 void nexus_symbol_table_cleanup(NexusSymbolTable* table);
 
 /**
  * @brief Cleanup a symbol registry
  * 
  * @param registry The registry to cleanup
  */
 void nexus_cleanup_symbol_registry(NexusSymbolRegistry* registry);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NEXUS_SYMBOLS_H */