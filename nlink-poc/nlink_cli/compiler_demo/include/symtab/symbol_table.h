/**
 * @file symbol_table.h
 * @brief Production-Grade Symbol Table Manager
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * 
 * Real-world compiler symbol table implementation with type checking,
 * scope management, and systematic error reporting integration.
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// TYPE DEFINITIONS AND ENUMERATIONS
// =============================================================================

typedef enum {
    SYM_TYPE_INT,
    SYM_TYPE_FLOAT,
    SYM_TYPE_CHAR,
    SYM_TYPE_STRING,
    SYM_TYPE_POINTER,
    SYM_TYPE_ARRAY,
    SYM_TYPE_FUNCTION,
    SYM_TYPE_STRUCT,
    SYM_TYPE_UNKNOWN
} symbol_type_t;

typedef enum {
    SYM_SCOPE_GLOBAL,
    SYM_SCOPE_LOCAL,
    SYM_SCOPE_PARAMETER,
    SYM_SCOPE_FUNCTION
} symbol_scope_t;

typedef struct symbol_entry {
    char *name;                    // Symbol identifier
    symbol_type_t type;           // Data type
    symbol_scope_t scope;         // Scope level
    uint32_t line_number;         // Declaration line
    uint32_t column;              // Declaration column
    size_t size;                  // Memory size
    bool is_initialized;          // Initialization status
    bool is_constant;             // Const qualifier
    struct symbol_entry *next;    // Hash chain
} symbol_entry_t;

typedef struct symbol_table {
    symbol_entry_t **buckets;     // Hash table buckets
    uint32_t bucket_count;        // Number of buckets
    uint32_t symbol_count;        // Total symbols
    uint32_t scope_level;         // Current scope depth
    struct symbol_table *parent;  // Parent scope
} symbol_table_t;

typedef enum {
    SYMTAB_SUCCESS,
    SYMTAB_ERROR_DUPLICATE,
    SYMTAB_ERROR_NOT_FOUND,
    SYMTAB_ERROR_TYPE_MISMATCH,
    SYMTAB_ERROR_SCOPE_INVALID,
    SYMTAB_ERROR_MEMORY_ALLOCATION
} symtab_result_t;

// =============================================================================
// CORE SYMBOL TABLE OPERATIONS
// =============================================================================

/**
 * @brief Initialize symbol table with specified capacity
 */
symbol_table_t* symtab_create(uint32_t bucket_count);

/**
 * @brief Add symbol to table with type checking
 */
symtab_result_t symtab_add_symbol(symbol_table_t *table, 
                                 const char *name,
                                 symbol_type_t type,
                                 symbol_scope_t scope,
                                 uint32_t line, uint32_t column);

/**
 * @brief Lookup symbol with scope resolution
 */
symbol_entry_t* symtab_lookup(symbol_table_t *table, const char *name);

/**
 * @brief Validate type compatibility for assignments
 */
bool symtab_check_type_compatibility(symbol_type_t from, symbol_type_t to);

/**
 * @brief Enter new scope level
 */
symbol_table_t* symtab_enter_scope(symbol_table_t *parent);

/**
 * @brief Exit current scope and cleanup
 */
symbol_table_t* symtab_exit_scope(symbol_table_t *current);

/**
 * @brief Display symbol table contents for debugging
 */
void symtab_display(symbol_table_t *table);

/**
 * @brief Cleanup symbol table and free memory
 */
void symtab_destroy(symbol_table_t *table);

#endif /* SYMBOL_TABLE_H */
