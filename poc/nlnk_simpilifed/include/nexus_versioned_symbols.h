// include/nexus_versioned_symbols.h
// Version-aware symbol management for NexusLink
// Author: Nnamdi Michael Okpala

#ifndef NEXUS_VERSIONED_SYMBOLS_H
#define NEXUS_VERSIONED_SYMBOLS_H

#include <stdlib.h>
#include <stdbool.h>

// Symbol types (compatible with original nexus_symbols.h)
typedef enum {
    VSYMBOL_FUNCTION,
    VSYMBOL_VARIABLE,
    VSYMBOL_TYPE,
    VSYMBOL_CONSTANT
} VersionedSymbolType;

// Enhanced symbol structure with version support
typedef struct {
    char* name;           // Symbol name
    char* version;        // Symbol version (semantic versioning)
    void* address;        // Memory address
    VersionedSymbolType type;  // Symbol type
    char* component_id;   // Component that provides this symbol
    int priority;         // Resolution priority (higher wins)
    int ref_count;        // Reference counting for usage tracking
} VersionedSymbol;

// Symbol table with version support
typedef struct {
    VersionedSymbol* symbols;  // Array of versioned symbols
    size_t size;               // Current number of symbols
    size_t capacity;           // Allocated capacity
} VersionedSymbolTable;

// Component dependency relationship
typedef struct {
    char* from_id;         // Dependent component
    char* to_id;           // Dependency component
    char* version_req;     // Version requirement
    bool optional;         // Whether dependency is optional
} ComponentDependency;

// Context-aware symbol registry
typedef struct {
    VersionedSymbolTable global;     // Global symbols (always available)
    VersionedSymbolTable imported;   // Imported symbols (required by components)
    VersionedSymbolTable exported;   // Exported symbols (provided by components)
    
    // Component dependency tracking
    ComponentDependency* dependencies;  // Array of component dependencies
    size_t deps_count;                  // Number of dependencies
    size_t deps_capacity;               // Allocated capacity for dependencies
} VersionedSymbolRegistry;

// Initialize a versioned symbol table
void versioned_symbol_table_init(VersionedSymbolTable* table, size_t initial_capacity);

// Create a new versioned symbol registry
VersionedSymbolRegistry* nexus_versioned_registry_create(void);

// Add a symbol to a versioned table
void versioned_symbol_table_add(VersionedSymbolTable* table, 
                               const char* name, 
                               const char* version,
                               void* address, 
                               VersionedSymbolType type, 
                               const char* component_id,
                               int priority);

// Find all symbols with a given name in a table
// Returns the number of matching symbols
// The caller is responsible for freeing the results array (but not the symbols themselves)
size_t versioned_symbol_table_find_all(VersionedSymbolTable* table, 
                                      const char* name,
                                      VersionedSymbol*** results);

// Add a component dependency relationship
void nexus_add_component_dependency(VersionedSymbolRegistry* registry,
                                   const char* component_id,
                                   const char* depends_on_id,
                                   const char* version_constraint,
                                   bool optional);

// Get a component's dependencies
// Returns an array of dependency IDs, caller must free the array
// *count will be set to the number of dependencies found
char** nexus_get_component_dependencies(VersionedSymbolRegistry* registry,
                                       const char* component_id,
                                       size_t* count);

// Check if a component directly depends on another
bool nexus_is_direct_dependency(VersionedSymbolRegistry* registry,
                               const char* component_id,
                               const char* potential_dependency);

// The core context-aware symbol resolution function
// This is the key function that handles the diamond dependency problem
// If version_constraint is NULL, any version is accepted
// The requesting_component is used for context-aware resolution
void* nexus_resolve_versioned_symbol(VersionedSymbolRegistry* registry,
                                    const char* name,
                                    const char* version_constraint,
                                    const char* requesting_component);

// Same as above but with additional type safety
void* nexus_resolve_versioned_symbol_typed(VersionedSymbolRegistry* registry,
                                          const char* name,
                                          const char* version_constraint,
                                          VersionedSymbolType expected_type,
                                          const char* requesting_component);

// Detect version conflicts in dependencies
// Returns true if conflicts were found
// conflict_details will contain a human-readable description of the conflict if found
bool nexus_detect_version_conflicts(VersionedSymbolRegistry* registry,
                                   const char* component_id,
                                   char** conflict_details);

// Generate a dependency graph in DOT format
// The caller is responsible for freeing the returned string
char* nexus_generate_dependency_graph(VersionedSymbolRegistry* registry);

// Free a versioned symbol table
void versioned_symbol_table_free(VersionedSymbolTable* table);

// Free a versioned symbol registry
void nexus_versioned_registry_free(VersionedSymbolRegistry* registry);

#endif // NEXUS_VERSIONED_SYMBOLS_H