/**
 * @file symbol_table.c
 * @brief Symbol Table Implementation
 */

#include "symtab/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUCKET_COUNT 256

// =============================================================================
// HASH FUNCTION IMPLEMENTATION
// =============================================================================

static uint32_t hash_function(const char *str, uint32_t bucket_count) {
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
    
    new_entry->name = strdup(name);
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
        default: new_entry->size = 0; break;
    }
    
    // Insert at head of chain
    new_entry->next = table->buckets[hash];
    table->buckets[hash] = new_entry;
    table->symbol_count++;
    
    return SYMTAB_SUCCESS;
}

symbol_entry_t* symtab_lookup(symbol_table_t *table, const char *name) {
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
    
    // Allow implicit conversions
    if ((from == SYM_TYPE_INT && to == SYM_TYPE_FLOAT) ||
        (from == SYM_TYPE_CHAR && to == SYM_TYPE_INT)) {
        return true;
    }
    
    return false;
}

void symtab_display(symbol_table_t *table) {
    printf("=== Symbol Table (Scope Level %d) ===\n", table->scope_level);
    printf("Total Symbols: %d\n", table->symbol_count);
    
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
                default: break;
            }
            
            printf("  %s: %s (line %d, scope %d)\n", 
                   entry->name, type_str, entry->line_number, entry->scope);
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
            free(entry->name);
            free(entry);
            entry = next;
        }
    }
    
    free(table->buckets);
    free(table);
}
