/**
 * @file symbol_table.c
 * @brief Symbol Table Implementation (Corrected)
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * 
 * Corrected implementation with proper feature test macros and
 * systematic memory management following Aegis waterfall methodology.
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include "symtab/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUCKET_COUNT 256

// =============================================================================
// UTILITY FUNCTIONS FOR MEMORY MANAGEMENT
// =============================================================================

/**
 * @brief Safe string duplication with systematic error handling
 */
static char* safe_strdup(const char *str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    
    memcpy(copy, str, len + 1);
    return copy;
}

// =============================================================================
// HASH FUNCTION IMPLEMENTATION
// =============================================================================

static uint32_t hash_function(const char *str, uint32_t bucket_count) {
    if (!str) return 0;
    
    uint32_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash % bucket_count;
}

// =============================================================================
// CORE SYMBOL TABLE OPERATIONS
// =============================================================================

symbol_table_t* symtab_create(uint32_t bucket_count) {
    if (bucket_count == 0) {
        bucket_count = DEFAULT_BUCKET_COUNT;
    }
    
    symbol_table_t *table = malloc(sizeof(symbol_table_t));
    if (!table) return NULL;
    
    table->buckets = calloc(bucket_count, sizeof(symbol_entry_t*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    table->bucket_count = bucket_count;
    table->symbol_count = 0;
    table->scope_level = 0;
    table->parent = NULL;
    
    return table;
}

symtab_result_t symtab_add_symbol(symbol_table_t *table, 
                                 const char *name,
                                 symbol_type_t type,
                                 symbol_scope_t scope,
                                 uint32_t line, uint32_t column) {
    if (!table || !name) return SYMTAB_ERROR_SCOPE_INVALID;
    
    // Check for duplicate in current scope
    uint32_t hash = hash_function(name, table->bucket_count);
    symbol_entry_t *entry = table->buckets[hash];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0 && entry->scope == scope) {
            return SYMTAB_ERROR_DUPLICATE;
        }
        entry = entry->next;
    }
    
    // Create new symbol entry
    symbol_entry_t *new_entry = malloc(sizeof(symbol_entry_t));
    if (!new_entry) return SYMTAB_ERROR_MEMORY_ALLOCATION;
    
    // Use safe string duplication
    new_entry->name = safe_strdup(name);
    if (!new_entry->name) {
        free(new_entry);
        return SYMTAB_ERROR_MEMORY_ALLOCATION;
    }
    
    new_entry->type = type;
    new_entry->scope = scope;
    new_entry->line_number = line;
    new_entry->column = column;
    new_entry->is_initialized = false;
    new_entry->is_constant = false;
    
    // Calculate size based on type
    switch (type) {
        case SYM_TYPE_INT: new_entry->size = sizeof(int); break;
        case SYM_TYPE_FLOAT: new_entry->size = sizeof(float); break;
        case SYM_TYPE_CHAR: new_entry->size = sizeof(char); break;
        case SYM_TYPE_POINTER: new_entry->size = sizeof(void*); break;
        case SYM_TYPE_STRING: new_entry->size = 0; break; // Variable size
        default: new_entry->size = 0; break;
    }
    
    // Insert at head of chain
    new_entry->next = table->buckets[hash];
    table->buckets[hash] = new_entry;
    table->symbol_count++;
    
    return SYMTAB_SUCCESS;
}

symbol_entry_t* symtab_lookup(symbol_table_t *table, const char *name) {
    if (!name) return NULL;
    
    while (table) {
        uint32_t hash = hash_function(name, table->bucket_count);
        symbol_entry_t *entry = table->buckets[hash];
        
        while (entry) {
            if (strcmp(entry->name, name) == 0) {
                return entry;
            }
            entry = entry->next;
        }
        
        table = table->parent; // Search parent scope
    }
    
    return NULL;
}

bool symtab_check_type_compatibility(symbol_type_t from, symbol_type_t to) {
    if (from == to) return true;
    
    // Allow implicit conversions following C language rules
    switch (from) {
        case SYM_TYPE_CHAR:
            return (to == SYM_TYPE_INT || to == SYM_TYPE_FLOAT);
        case SYM_TYPE_INT:
            return (to == SYM_TYPE_FLOAT);
        default:
            return false;
    }
}

symbol_table_t* symtab_enter_scope(symbol_table_t *parent) {
    symbol_table_t *new_scope = symtab_create(DEFAULT_BUCKET_COUNT / 4); // Smaller scope tables
    if (!new_scope) return NULL;
    
    new_scope->parent = parent;
    new_scope->scope_level = parent ? parent->scope_level + 1 : 0;
    
    return new_scope;
}

symbol_table_t* symtab_exit_scope(symbol_table_t *current) {
    if (!current) return NULL;
    
    symbol_table_t *parent = current->parent;
    symtab_destroy(current);
    return parent;
}

void symtab_display(symbol_table_t *table) {
    if (!table) {
        printf("=== NULL Symbol Table ===\n");
        return;
    }
    
    printf("=== Symbol Table (Scope Level %d) ===\n", table->scope_level);
    printf("Total Symbols: %d\n", table->symbol_count);
    
    if (table->symbol_count == 0) {
        printf("  (No symbols)\n");
        printf("===============================\n\n");
        return;
    }
    
    for (uint32_t i = 0; i < table->bucket_count; i++) {
        symbol_entry_t *entry = table->buckets[i];
        while (entry) {
            const char *type_str = "unknown";
            switch (entry->type) {
                case SYM_TYPE_INT: type_str = "int"; break;
                case SYM_TYPE_FLOAT: type_str = "float"; break;
                case SYM_TYPE_CHAR: type_str = "char"; break;
                case SYM_TYPE_STRING: type_str = "string"; break;
                case SYM_TYPE_POINTER: type_str = "pointer"; break;
                case SYM_TYPE_ARRAY: type_str = "array"; break;
                case SYM_TYPE_FUNCTION: type_str = "function"; break;
                case SYM_TYPE_STRUCT: type_str = "struct"; break;
                default: type_str = "unknown"; break;
            }
            
            printf("  %s: %s (line %d, col %d, scope %d, size %zu)\n", 
                   entry->name, type_str, entry->line_number, entry->column, 
                   entry->scope, entry->size);
            entry = entry->next;
        }
    }
    printf("===============================\n\n");
}

void symtab_destroy(symbol_table_t *table) {
    if (!table) return;
    
    for (uint32_t i = 0; i < table->bucket_count; i++) {
        symbol_entry_t *entry = table->buckets[i];
        while (entry) {
            symbol_entry_t *next = entry->next;
            if (entry->name) {
                free(entry->name);
            }
            free(entry);
            entry = next;
        }
    }
    
    free(table->buckets);
    free(table);
}

// =============================================================================
// ADDITIONAL UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Get symbol count for statistical analysis
 */
uint32_t symtab_get_symbol_count(symbol_table_t *table) {
    return table ? table->symbol_count : 0;
}

/**
 * @brief Calculate load factor for performance monitoring
 */
double symtab_get_load_factor(symbol_table_t *table) {
    if (!table || table->bucket_count == 0) return 0.0;
    return (double)table->symbol_count / (double)table->bucket_count;
}

/**
 * @brief Print performance statistics for optimization analysis
 */
void symtab_print_stats(symbol_table_t *table) {
    if (!table) return;
    
    printf("=== Symbol Table Statistics ===\n");
    printf("Symbols: %d\n", table->symbol_count);
    printf("Buckets: %d\n", table->bucket_count);
    printf("Load Factor: %.2f\n", symtab_get_load_factor(table));
    printf("Scope Level: %d\n", table->scope_level);
    
    // Calculate chain length distribution
    uint32_t max_chain = 0;
    uint32_t empty_buckets = 0;
    
    for (uint32_t i = 0; i < table->bucket_count; i++) {
        uint32_t chain_length = 0;
        symbol_entry_t *entry = table->buckets[i];
        
        if (!entry) {
            empty_buckets++;
        } else {
            while (entry) {
                chain_length++;
                entry = entry->next;
            }
            if (chain_length > max_chain) {
                max_chain = chain_length;
            }
        }
    }
    
    printf("Empty Buckets: %d (%.1f%%)\n", empty_buckets, 
           (double)empty_buckets / table->bucket_count * 100.0);
    printf("Max Chain Length: %d\n", max_chain);
    printf("==============================\n\n");
}
