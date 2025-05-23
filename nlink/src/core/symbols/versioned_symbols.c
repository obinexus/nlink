// src/nexus_versioned_symbols.c
// Version-aware symbol management for NexusLink
// Author: Nnamdi Michael Okpala

#include "nlink/core/symbols/nexus_versioned_symbols.h"

// Initialize a versioned symbol table
void versioned_symbol_table_init(VersionedSymbolTable* table, size_t initial_capacity) {
    table->symbols = (VersionedSymbol*)malloc(initial_capacity * sizeof(VersionedSymbol));
    table->capacity = initial_capacity;
    table->size = 0;
}

// Create a new versioned symbol registry
VersionedSymbolRegistry* nexus_versioned_registry_create() {
    VersionedSymbolRegistry* registry = (VersionedSymbolRegistry*)malloc(sizeof(VersionedSymbolRegistry));
    
    // Initialize symbol tables
    versioned_symbol_table_init(&registry->global, 64);
    versioned_symbol_table_init(&registry->imported, 128);
    versioned_symbol_table_init(&registry->exported, 128);
    
    // Initialize dependency tracking
    registry->dependencies = NULL;
    registry->deps_count = 0;
    registry->deps_capacity = 0;
    
    return registry;
}

// Add a symbol to a versioned table
void versioned_symbol_table_add(VersionedSymbolTable* table, 
                               const char* name, 
                               const char* version,
                               void* address, 
                               VersionedSymbolType type, 
                               const char* component_id,
                               int priority) {
    // Resize if needed
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = (VersionedSymbol*)realloc(table->symbols, 
                                                 table->capacity * sizeof(VersionedSymbol));
    }
    
    // Add the new symbol
    VersionedSymbol* symbol = &table->symbols[table->size++];
    symbol->name = strdup(name);
    symbol->version = version ? strdup(version) : strdup("1.0.0"); // Default version
    symbol->address = address;
    symbol->type = type;
    symbol->component_id = strdup(component_id);
    symbol->priority = priority;
    symbol->ref_count = 0;
}

// Find all symbols with a given name in a table
size_t versioned_symbol_table_find_all(VersionedSymbolTable* table, 
                                      const char* name,
                                      VersionedSymbol*** results) {
    // Count matching symbols
    size_t count = 0;
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            count++;
        }
    }
    
    if (count == 0) {
        *results = NULL;
        return 0;
    }
    
    // Allocate result array
    *results = (VersionedSymbol**)malloc(count * sizeof(VersionedSymbol*));
    
    // Fill the array
    size_t index = 0;
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            (*results)[index++] = &table->symbols[i];
        }
    }
    
    return count;
}

// Add a component dependency relationship
void nexus_add_component_dependency(VersionedSymbolRegistry* registry,
                                   const char* component_id,
                                   const char* depends_on_id,
                                   const char* version_constraint,
                                   bool optional) {
    // Initialize or resize dependencies array if needed
    if (registry->dependencies == NULL) {
        registry->deps_capacity = 16;
        registry->dependencies = (ComponentDependency*)malloc(
            registry->deps_capacity * sizeof(ComponentDependency));
    } else if (registry->deps_count >= registry->deps_capacity) {
        registry->deps_capacity *= 2;
        registry->dependencies = (ComponentDependency*)realloc(
            registry->dependencies,
            registry->deps_capacity * sizeof(ComponentDependency));
    }
    
    // Add the dependency
    ComponentDependency* dep = &registry->dependencies[registry->deps_count++];
    dep->from_id = strdup(component_id);
    dep->to_id = strdup(depends_on_id);
    dep->version_req = version_constraint ? strdup(version_constraint) : strdup("*");
    dep->optional = optional;
}

// Get a component's dependencies
char** nexus_get_component_dependencies(VersionedSymbolRegistry* registry,
                                       const char* component_id,
                                       size_t* count) {
    // Count dependencies for this component
    size_t dep_count = 0;
    for (size_t i = 0; i < registry->deps_count; i++) {
        if (strcmp(registry->dependencies[i].from_id, component_id) == 0) {
            dep_count++;
        }
    }
    
    if (dep_count == 0) {
        *count = 0;
        return NULL;
    }
    
    // Allocate result array
    char** dependencies = (char**)malloc(dep_count * sizeof(char*));
    
    // Fill the array
    size_t index = 0;
    for (size_t i = 0; i < registry->deps_count; i++) {
        if (strcmp(registry->dependencies[i].from_id, component_id) == 0) {
            dependencies[index++] = strdup(registry->dependencies[i].to_id);
        }
    }
    
    *count = dep_count;
    return dependencies;
}

// Check if a component directly depends on another
bool nexus_is_direct_dependency(VersionedSymbolRegistry* registry,
                               const char* component_id,
                               const char* potential_dependency) {
    for (size_t i = 0; i < registry->deps_count; i++) {
        ComponentDependency* dep = &registry->dependencies[i];
        if (strcmp(dep->from_id, component_id) == 0 && 
            strcmp(dep->to_id, potential_dependency) == 0) {
            return true;
        }
    }
    return false;
}

// Find the version constraint for a dependency relationship
const char* find_version_constraint(VersionedSymbolRegistry* registry,
                                   const char* component_id,
                                   const char* dependency_id) {
    for (size_t i = 0; i < registry->deps_count; i++) {
        ComponentDependency* dep = &registry->dependencies[i];
        if (strcmp(dep->from_id, component_id) == 0 && 
            strcmp(dep->to_id, dependency_id) == 0) {
            return dep->version_req;
        }
    }
    return NULL;
}

// The core context-aware symbol resolution function
void* nexus_resolve_versioned_symbol(VersionedSymbolRegistry* registry,
                                    const char* name,
                                    const char* version_constraint,
                                    const char* requesting_component) {
    VersionedSymbol* best_match = NULL;
    int best_priority = -1;
    
    // First check the exported table (usually highest priority)
    VersionedSymbol** exported_symbols;
    size_t exported_count = versioned_symbol_table_find_all(&registry->exported, 
                                                          name, 
                                                          &exported_symbols);
    
    // Filter and find best match from exported symbols
    for (size_t i = 0; i < exported_count; i++) {
        VersionedSymbol* symbol = exported_symbols[i];
        
        // Check version constraint if specified
        if (version_constraint && !semver_satisfies(symbol->version, version_constraint)) {
            continue;
        }
        
        // Priority calculation:
        // 1. Direct dependency gets highest priority
        // 2. Then consider symbol's own priority
        int effective_priority = symbol->priority;
        
        if (nexus_is_direct_dependency(registry, requesting_component, symbol->component_id)) {
            effective_priority += 1000; // Big boost for direct dependencies
        }
        
        // If we have a direct dependency with a version constraint, check that
        const char* specific_constraint = find_version_constraint(registry, 
                                                                requesting_component, 
                                                                symbol->component_id);
        if (specific_constraint && !semver_satisfies(symbol->version, specific_constraint)) {
            continue; // Skip this symbol if it doesn't satisfy the specific constraint
        }
        
        if (effective_priority > best_priority) {
            best_match = symbol;
            best_priority = effective_priority;
        }
    }
    
    // Free the results array
    if (exported_count > 0) {
        free(exported_symbols);
    }
    
    // If we found a match in exported, use it
    if (best_match) {
        best_match->ref_count++; // Track usage
        
        // Add to imported table for the requesting component if not already there
        bool already_imported = false;
        for (size_t i = 0; i < registry->imported.size; i++) {
            VersionedSymbol* sym = &registry->imported.symbols[i];
            if (strcmp(sym->name, name) == 0 && 
                strcmp(sym->component_id, requesting_component) == 0) {
                already_imported = true;
                break;
            }
        }
        
        if (!already_imported) {
            versioned_symbol_table_add(&registry->imported, name, best_match->version, 
                                      best_match->address, best_match->type, 
                                      requesting_component, 0);
        }
        
        printf("Resolved '%s' version '%s' from component '%s' (priority: %d)\n",
               name, best_match->version, best_match->component_id, best_priority);
        
        return best_match->address;
    }
    
    // Check the global table as fallback
    VersionedSymbol** global_symbols;
    size_t global_count = versioned_symbol_table_find_all(&registry->global, 
                                                        name, 
                                                        &global_symbols);
    
    // Find best match from global symbols
    best_match = NULL;
    best_priority = -1;
    
    for (size_t i = 0; i < global_count; i++) {
        VersionedSymbol* symbol = global_symbols[i];
        
        // Check version constraint if specified
        if (version_constraint && !semver_satisfies(symbol->version, version_constraint)) {
            continue;
        }
        
        if (symbol->priority > best_priority) {
            best_match = symbol;
            best_priority = symbol->priority;
        }
    }
    
    // Free the results array
    if (global_count > 0) {
        free(global_symbols);
    }
    
    // If we found a match in global, use it
    if (best_match) {
        best_match->ref_count++; // Track usage
        
        printf("Resolved '%s' version '%s' from global table (priority: %d)\n",
               name, best_match->version, best_priority);
        
        return best_match->address;
    }
    
    // Symbol not found with the given constraints
    printf("Failed to resolve symbol '%s' with constraint '%s' for component '%s'\n", 
           name, version_constraint ? version_constraint : "any", requesting_component);
    
    return NULL;
}

// Same as above but with additional type safety
void* nexus_resolve_versioned_symbol_typed(VersionedSymbolRegistry* registry,
                                          const char* name,
                                          const char* version_constraint,
                                          VersionedSymbolType expected_type,
                                          const char* requesting_component) {
    // First resolve the symbol
    void* address = nexus_resolve_versioned_symbol(registry, name, version_constraint, 
                                                 requesting_component);
    
    if (!address) {
        return NULL; // Symbol not found
    }
    
    // Now verify the type
    VersionedSymbol** exported_symbols;
    size_t exported_count = versioned_symbol_table_find_all(&registry->exported, 
                                                          name, 
                                                          &exported_symbols);
    
    for (size_t i = 0; i < exported_count; i++) {
        VersionedSymbol* symbol = exported_symbols[i];
        if (symbol->address == address) {
            if (symbol->type != expected_type) {
                printf("Type mismatch for symbol '%s': expected %d, got %d\n",
                       name, expected_type, symbol->type);
                free(exported_symbols);
                return NULL;
            }
            free(exported_symbols);
            return address;
        }
    }
    
    // If not found in exported, check global
    if (exported_count > 0) {
        free(exported_symbols);
    }
    
    VersionedSymbol** global_symbols;
    size_t global_count = versioned_symbol_table_find_all(&registry->global, 
                                                        name, 
                                                        &global_symbols);
    
    for (size_t i = 0; i < global_count; i++) {
        VersionedSymbol* symbol = global_symbols[i];
        if (symbol->address == address) {
            if (symbol->type != expected_type) {
                printf("Type mismatch for symbol '%s': expected %d, got %d\n",
                       name, expected_type, symbol->type);
                free(global_symbols);
                return NULL;
            }
            free(global_symbols);
            return address;
        }
    }
    
    if (global_count > 0) {
        free(global_symbols);
    }
    
    // This should not happen if the symbol was found
    return address;
}

// Detect version conflicts in dependencies
bool nexus_detect_version_conflicts(VersionedSymbolRegistry* registry,
                                   const char* component_id,
                                   char** conflict_details) {
    (void)component_id; // Silence unused parameter warning
    bool conflicts_found = false;
    char conflict_str[1024] = {0};
    
    // Find all symbols that have multiple versions
    for (size_t i = 0; i < registry->exported.size; i++) {
        VersionedSymbol* symbol = &registry->exported.symbols[i];
        
        // Skip already processed symbols
        bool already_processed = false;
        for (size_t j = 0; j < i; j++) {
            if (strcmp(registry->exported.symbols[j].name, symbol->name) == 0) {
                already_processed = true;
                break;
            }
        }
        if (already_processed) continue;
        
        // Find all instances of this symbol
        VersionedSymbol** symbols;
        size_t count = versioned_symbol_table_find_all(&registry->exported, 
                                                     symbol->name, 
                                                     &symbols);
        
        // Check if there are multiple versions
        if (count > 1) {
            // Count unique versions
            char** versions = (char**)malloc(count * sizeof(char*));
            size_t unique_count = 0;
            
            for (size_t j = 0; j < count; j++) {
                // Check if version is already in the list
                bool already_counted = false;
                for (size_t k = 0; k < unique_count; k++) {
                    if (strcmp(versions[k], symbols[j]->version) == 0) {
                        already_counted = true;
                        break;
                    }
                }
                
                if (!already_counted) {
                    versions[unique_count++] = symbols[j]->version;
                }
            }
            
            // If there are multiple unique versions, we have a conflict
            if (unique_count > 1) {
                conflicts_found = true;
                
                // Build conflict description
                char conflict_desc[512];
                snprintf(conflict_desc, sizeof(conflict_desc), 
                         "Symbol '%s' has %zu versions: ", symbol->name, unique_count);
                
                for (size_t j = 0; j < unique_count; j++) {
                    char version_info[128];
                    snprintf(version_info, sizeof(version_info), 
                             "%s%s", versions[j], (j < unique_count - 1) ? ", " : "");
                    strcat(conflict_desc, version_info);
                }
                
                // Add components providing each version
                strcat(conflict_desc, " (provided by: ");
                for (size_t j = 0; j < count; j++) {
                    char comp_info[128];
                    snprintf(comp_info, sizeof(comp_info), 
                             "%s@%s%s", symbols[j]->component_id, symbols[j]->version,
                             (j < count - 1) ? ", " : ")");
                    strcat(conflict_desc, comp_info);
                }
                
                // Append to main conflict string
                if (strlen(conflict_str) + strlen(conflict_desc) + 2 < sizeof(conflict_str)) {
                    if (strlen(conflict_str) > 0) {
                        strcat(conflict_str, "\n");
                    }
                    strcat(conflict_str, conflict_desc);
                }
            }
            
            free(versions);
        }
        
        if (count > 0) {
            free(symbols);
        }
    }
    
    if (conflicts_found && conflict_details) {
        *conflict_details = strdup(conflict_str);
    }
    
    return conflicts_found;
}

// Generate a dependency graph in DOT format
char* nexus_generate_dependency_graph(VersionedSymbolRegistry* registry) {
    // Create a buffer for the graph
    size_t buffer_size = 4096;
    char* buffer = (char*)malloc(buffer_size);
    size_t pos = 0;
    
    // Write DOT header
    pos += snprintf(buffer + pos, buffer_size - pos, 
                   "digraph DependencyGraph {\n"
                   "  rankdir=LR;\n"
                   "  node [shape=box, style=filled, fillcolor=lightblue];\n\n");
    
    // Create a set of unique component IDs
    char** components = NULL;
    size_t component_count = 0;
    
    // Collect components from dependencies
    for (size_t i = 0; i < registry->deps_count; i++) {
        ComponentDependency* dep = &registry->dependencies[i];
        
        // Check if from_id is already in the list
        bool from_found = false;
        for (size_t j = 0; j < component_count; j++) {
            if (strcmp(components[j], dep->from_id) == 0) {
                from_found = true;
                break;
            }
        }
        
        // Add from_id if not found
        if (!from_found) {
            component_count++;
            components = (char**)realloc(components, component_count * sizeof(char*));
            components[component_count - 1] = strdup(dep->from_id);
        }
        
        // Check if to_id is already in the list
        bool to_found = false;
        for (size_t j = 0; j < component_count; j++) {
            if (strcmp(components[j], dep->to_id) == 0) {
                to_found = true;
                break;
            }
        }
        
        // Add to_id if not found
        if (!to_found) {
            component_count++;
            components = (char**)realloc(components, component_count * sizeof(char*));
            components[component_count - 1] = strdup(dep->to_id);
        }
    }
    
    // Also collect components from exported symbols
    for (size_t i = 0; i < registry->exported.size; i++) {
        VersionedSymbol* symbol = &registry->exported.symbols[i];
        
        // Check if component_id is already in the list
        bool found = false;
        for (size_t j = 0; j < component_count; j++) {
            if (strcmp(components[j], symbol->component_id) == 0) {
                found = true;
                break;
            }
        }
        
        // Add component_id if not found
        if (!found) {
            component_count++;
            components = (char**)realloc(components, component_count * sizeof(char*));
            components[component_count - 1] = strdup(symbol->component_id);
        }
    }
    
    // Write component nodes
    pos += snprintf(buffer + pos, buffer_size - pos, "  // Component nodes\n");
    for (size_t i = 0; i < component_count; i++) {
        pos += snprintf(buffer + pos, buffer_size - pos, 
                       "  \"%s\" [label=\"%s\"];\n", 
                       components[i], components[i]);
    }
    
    // Write dependency edges
    pos += snprintf(buffer + pos, buffer_size - pos, "\n  // Dependency edges\n");
    for (size_t i = 0; i < registry->deps_count; i++) {
        ComponentDependency* dep = &registry->dependencies[i];
        
        pos += snprintf(buffer + pos, buffer_size - pos, 
                       "  \"%s\" -> \"%s\" [label=\"%s%s\"];\n", 
                       dep->from_id, dep->to_id, dep->version_req,
                       dep->optional ? ", optional" : "");
    }
    
    // Write closing brace
    pos += snprintf(buffer + pos, buffer_size - pos, "}\n");
    
    // Free component list
    for (size_t i = 0; i < component_count; i++) {
        free(components[i]);
    }
    free(components);
    
    return buffer;
}

// Free a versioned symbol table
void versioned_symbol_table_free(VersionedSymbolTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        free(table->symbols[i].name);
        free(table->symbols[i].version);
        free(table->symbols[i].component_id);
    }
    
    free(table->symbols);
    table->symbols = NULL;
    table->size = 0;
    table->capacity = 0;
}

// Free a versioned symbol registry
void nexus_versioned_registry_free(VersionedSymbolRegistry* registry) {
    versioned_symbol_table_free(&registry->global);
    versioned_symbol_table_free(&registry->imported);
    versioned_symbol_table_free(&registry->exported);
    
    // Free dependencies
    for (size_t i = 0; i < registry->deps_count; i++) {
        free(registry->dependencies[i].from_id);
        free(registry->dependencies[i].to_id);
        free(registry->dependencies[i].version_req);
    }
    free(registry->dependencies);
    
    free(registry);
}