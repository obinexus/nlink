// symbols.h - Symbol table management for NexusLink
#ifndef NEXUS_SYMBOLS_H
#define NEXUS_SYMBOLS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "nlink/core/common/result.h"
#include "nlink/core/common/types.h"


#ifdef __cplusplus
extern "C" {
#endif

// Symbol type enumeration
typedef enum {
    SYMBOL_FUNCTION,
    SYMBOL_VARIABLE,
    SYMBOL_TYPE,
    SYMBOL_CONSTANT,
    SYMBOL_ENUM,
    SYMBOL_STRUCT,
    SYMBOL_UNION,
    SYMBOL_CLASS,
    SYMBOL_INTERFACE,
    SYMBOL_ENUMERATOR,
    SYMBOL_MACRO,
    SYMBOL_TEMPLATE,
    SYMBOL_NAMESPACE,
    SYMBOL_MODULE,
    SYMBOL_PROPERTY,
    SYMBOL_METHOD,
    SYMBOL_FIELD,
    SYMBOL_EVENT,
    SYMBOL_DELEGATE,
    SYMBOL_SIGNAL,
    SYMBOL_SLOT,
    SYMBOL_UNKNOWN
} SymbolType;

// Symbol structure
typedef struct NexusSymbol {
    char* name;
    void* address;
    SymbolType type;
    char* component_id;
    int ref_count;
} NexusSymbol;

// Symbol table structure
typedef struct NexusSymbolTable {
    NexusSymbol* symbols;
    size_t capacity;
    size_t size;
} NexusSymbolTable;

// Symbol registry structure
typedef struct NexusSymbolRegistry {
    NexusSymbolTable global;
    NexusSymbolTable imported;
    NexusSymbolTable exported;
} NexusSymbolRegistry;

// Symbol table functions
void symbol_table_init(NexusSymbolTable* table, size_t initial_capacity);
NexusSymbol* symbol_table_find(NexusSymbolTable* table, const char* name);
NexusResult symbol_table_add(NexusSymbolTable* table, const char* name, void* address, 
                          SymbolType type, const char* component_id);
bool symbol_table_remove(NexusSymbolTable* table, const char* name);
bool symbol_table_update_address(NexusSymbolTable* table, const char* name, void* new_address);
void symbol_table_copy_symbols(NexusSymbolTable* dest, const NexusSymbolTable* src, 
                             const char* component_filter);
size_t symbol_table_get_component_symbols(NexusSymbolTable* table, const char* component_id, 
                                       NexusSymbol*** symbols_out);
void symbol_table_free(NexusSymbolTable* table);
void symbol_table_print_stats(const NexusSymbolTable* table, const char* table_name);

// Symbol registry functions
NexusSymbolRegistry* nexus_symbol_registry_create(void);
void nexus_symbol_registry_free(NexusSymbolRegistry* registry);
void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name);
void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, const char* name, 
                                  SymbolType expected_type, const char* using_component);
void* nexus_context_aware_resolve(NexusSymbolRegistry* registry, const char* name, 
                                const char* context, const char* using_component);
void nexus_track_symbol_usage(NexusSymbolRegistry* registry, const char* symbol_name, 
                            const char* using_component);
void nexus_generate_symbol_dependency_graph(NexusSymbolRegistry* registry, const char* output_file);

#ifdef __cplusplus
}
#endif

#endif // NEXUS_SYMBOLS_H// symbols.h - Symbol table management for NexusLink
#ifndef NEXUS_SYMBOLS_H
#define NEXUS_SYMBOLS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "nlink/core/common/result.h"

#ifdef __cplusplus
extern "C" {
#endif

// Symbol type enumeration
typedef enum {
    SYMBOL_FUNCTION,
    SYMBOL_VARIABLE,
    SYMBOL_TYPE,
    SYMBOL_CONSTANT,
    SYMBOL_ENUM,
    SYMBOL_STRUCT,
    SYMBOL_UNION,
    SYMBOL_CLASS,
    SYMBOL_INTERFACE,
    SYMBOL_ENUMERATOR,
    SYMBOL_MACRO,
    SYMBOL_TEMPLATE,
    SYMBOL_NAMESPACE,
    SYMBOL_MODULE,
    SYMBOL_PROPERTY,
    SYMBOL_METHOD,
    SYMBOL_FIELD,
    SYMBOL_EVENT,
    SYMBOL_DELEGATE,
    SYMBOL_SIGNAL,
    SYMBOL_SLOT,
    SYMBOL_UNKNOWN
} SymbolType;

// Symbol structure
typedef struct NexusSymbol {
    char* name;
    void* address;
    SymbolType type;
    char* component_id;
    int ref_count;
} NexusSymbol;

// Symbol table structure
typedef struct NexusSymbolTable {
    NexusSymbol* symbols;
    size_t capacity;
    size_t size;
} NexusSymbolTable;

// Symbol registry structure
typedef struct NexusSymbolRegistry {
    NexusSymbolTable global;
    NexusSymbolTable imported;
    NexusSymbolTable exported;
} NexusSymbolRegistry;

// Symbol table functions
void symbol_table_init(NexusSymbolTable* table, size_t initial_capacity);
NexusSymbol* symbol_table_find(NexusSymbolTable* table, const char* name);
NexusResult symbol_table_add(NexusSymbolTable* table, const char* name, void* address, 
                          SymbolType type, const char* component_id);
bool symbol_table_remove(NexusSymbolTable* table, const char* name);
bool symbol_table_update_address(NexusSymbolTable* table, const char* name, void* new_address);
void symbol_table_copy_symbols(NexusSymbolTable* dest, const NexusSymbolTable* src, 
                             const char* component_filter);
size_t symbol_table_get_component_symbols(NexusSymbolTable* table, const char* component_id, 
                                       NexusSymbol*** symbols_out);
void symbol_table_free(NexusSymbolTable* table);
void symbol_table_print_stats(const NexusSymbolTable* table, const char* table_name);

// Symbol registry functions
NexusSymbolRegistry* nexus_symbol_registry_create(void);
void nexus_symbol_registry_free(NexusSymbolRegistry* registry);
void* nexus_resolve_symbol(NexusSymbolRegistry* registry, const char* name);
void* nexus_lookup_symbol_with_type(NexusSymbolRegistry* registry, const char* name, 
                                  SymbolType expected_type, const char* using_component);
void* nexus_context_aware_resolve(NexusSymbolRegistry* registry, const char* name, 
                                const char* context, const char* using_component);
void nexus_track_symbol_usage(NexusSymbolRegistry* registry, const char* symbol_name, 
                            const char* using_component);
void nexus_generate_symbol_dependency_graph(NexusSymbolRegistry* registry, const char* output_file);

#ifdef __cplusplus
}
#endif

#endif // NEXUS_SYMBOLS_H