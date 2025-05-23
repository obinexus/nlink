// src/nexus_enhanced_metadata.c
// Implementation of enhanced metadata system with version support
// Author: Implementation Team

#define NEXUS_JSON_EXTERN

// Helper function to parse a symbol definition from JSON
static void parse_symbol_definition(NexusJsonValue* symbol_json, SymbolDefinition* symbol) {
    // Check if symbol is a string (legacy format) or object (new format)
    if (symbol_json->type == NEXUS_JSON_STRING) {
        // Legacy format: just the name
        symbol->name = strdup(symbol_json->data.string);
        symbol->version = strdup("1.0.0");  // Default version
        symbol->type = 0;  // Default to function
    } else if (symbol_json->type == NEXUS_JSON_OBJECT) {
        // New format: {name, version, type}
        const char* name = nexus_json_object_get_string(symbol_json, "name", NULL);
        const char* version = nexus_json_object_get_string(symbol_json, "version", "1.0.0");
        int type = (int)nexus_json_object_get_number(symbol_json, "type", 0);
        
        symbol->name = name ? strdup(name) : strdup("");
        symbol->version = version ? strdup(version) : strdup("1.0.0");
        symbol->type = type;
    } else {
        // Invalid format
        symbol->name = strdup("");
        symbol->version = strdup("1.0.0");
        symbol->type = 0;
    }
}


// Load enhanced component metadata from a JSON file
EnhancedComponentMetadata* nexus_load_enhanced_metadata(const char* metadata_path) {
    // Allocate the metadata structure
    EnhancedComponentMetadata* metadata = (EnhancedComponentMetadata*)malloc(sizeof(EnhancedComponentMetadata));
    memset(metadata, 0, sizeof(EnhancedComponentMetadata));
    
    // Parse the JSON file
    NexusJsonValue* root = nexus_json_parse_file(metadata_path);
    if (!root) {
        fprintf(stderr, "Error: Could not parse metadata file: %s\n", metadata_path);
        free(metadata);
        return NULL;
    }
    
    // Extract basic information
    const char* id = nexus_json_object_get_string(root, "id", NULL);
    if (id) metadata->id = strdup(id);
    
    const char* version = nexus_json_object_get_string(root, "version", NULL);
    if (version) {
        metadata->version = strdup(version);
        metadata->parsed_version = semver_parse(version);
    } else {
        metadata->version = strdup("1.0.0");
        metadata->parsed_version = semver_parse("1.0.0");
    }
    
    const char* description = nexus_json_object_get_string(root, "description", NULL);
    if (description) metadata->description = strdup(description);
    
    // Process dependencies with version requirements
    NexusJsonValue* dependencies = nexus_json_object_get(root, "dependencies");
    if (dependencies && dependencies->type == NEXUS_JSON_ARRAY) {
        metadata->dependencies_count = dependencies->data.array.count;
        metadata->dependencies = (EnhancedDependency*)malloc(metadata->dependencies_count * sizeof(EnhancedDependency));
        
        for (size_t i = 0; i < dependencies->data.array.count; i++) {
            NexusJsonValue* dep = dependencies->data.array.items[i];
            
            if (dep->type == NEXUS_JSON_OBJECT) {
                // Initialize with defaults
                metadata->dependencies[i].id = NULL;
                metadata->dependencies[i].version_req = NULL;
                metadata->dependencies[i].optional = false;
                metadata->dependencies[i].resolved_version = NULL;
                
                // Extract values
                const char* dep_id = nexus_json_object_get_string(dep, "id", NULL);
                if (dep_id) metadata->dependencies[i].id = strdup(dep_id);
                
                // Check for version_req (new format) or version (legacy format)
                const char* dep_version_req = nexus_json_object_get_string(dep, "version_req", NULL);
                if (!dep_version_req) {
                    dep_version_req = nexus_json_object_get_string(dep, "version", NULL);
                }
                if (dep_version_req) metadata->dependencies[i].version_req = strdup(dep_version_req);
                
                NexusJsonValue* optional = nexus_json_object_get(dep, "optional");
                if (optional && optional->type == NEXUS_JSON_BOOL) {
                    metadata->dependencies[i].optional = optional->data.boolean;
                }
            }
        }
    }
    
    // Process exported symbols
    NexusJsonValue* exported = nexus_json_object_get(root, "exported_symbols");
    if (exported && exported->type == NEXUS_JSON_ARRAY) {
        metadata->exported_count = exported->data.array.count;
        metadata->exported_symbols = (SymbolDefinition*)malloc(metadata->exported_count * sizeof(SymbolDefinition));
        
        for (size_t i = 0; i < exported->data.array.count; i++) {
            NexusJsonValue* symbol = exported->data.array.items[i];
            parse_symbol_definition(symbol, &metadata->exported_symbols[i]);
        }
    }
    
    // Process imported symbols
    NexusJsonValue* imported = nexus_json_object_get(root, "imported_symbols");
    if (imported && imported->type == NEXUS_JSON_ARRAY) {
        metadata->imported_count = imported->data.array.count;
        metadata->imported_symbols = (SymbolDefinition*)malloc(metadata->imported_count * sizeof(SymbolDefinition));
        
        for (size_t i = 0; i < imported->data.array.count; i++) {
            NexusJsonValue* symbol = imported->data.array.items[i];
            parse_symbol_definition(symbol, &metadata->imported_symbols[i]);
        }
    }
    
    // Resource metrics
    NexusJsonValue* memory_footprint = nexus_json_object_get(root, "memory_footprint");
    if (memory_footprint && memory_footprint->type == NEXUS_JSON_NUMBER) {
        metadata->memory_footprint = (size_t)memory_footprint->data.number;
    }
    
    NexusJsonValue* load_time = nexus_json_object_get(root, "avg_load_time_ms");
    if (load_time && load_time->type == NEXUS_JSON_NUMBER) {
        metadata->avg_load_time_ms = load_time->data.number;
    }
    
    // Set initial usage values
    metadata->usage_count = 0;
    metadata->last_used = 0;
    metadata->loaded = false;
    
    // Clean up JSON object
    nexus_json_free(root);
    
    return metadata;
}

// Update component usage statistics
void nexus_enhanced_metadata_track_usage(EnhancedComponentMetadata* metadata) {
    if (!metadata) return;
    
    metadata->usage_count++;
    metadata->last_used = time(NULL);
}

// Check if component has been used recently
bool nexus_enhanced_metadata_recently_used(
    const EnhancedComponentMetadata* metadata, 
    time_t seconds_threshold
) {
    if (!metadata) return false;
    
    time_t now = time(NULL);
    return (now - metadata->last_used) < seconds_threshold;
}

// Check version compatibility between components
bool nexus_enhanced_metadata_check_version_compatibility(
    const EnhancedComponentMetadata* component,
    const EnhancedComponentMetadata* dependency
) {
    if (!component || !dependency || !component->parsed_version || !dependency->parsed_version)
        return false;
    
    // Find the dependency relationship
    const char* version_req = NULL;
    for (size_t i = 0; i < component->dependencies_count; i++) {
        if (strcmp(component->dependencies[i].id, dependency->id) == 0) {
            version_req = component->dependencies[i].version_req;
            break;
        }
    }
    
    if (!version_req) {
        // No explicit dependency relationship found
        return false;
    }
    
    // Check if dependency's version satisfies the requirement
    return semver_satisfies(dependency->version, version_req);
}

// Check if a component satisfies all its dependencies
bool nexus_enhanced_metadata_check_dependencies(
    const EnhancedComponentMetadata* metadata,
    const EnhancedComponentMetadata** available_components,
    size_t num_available_components,
    char** missing_dependency
) {
    if (!metadata) return true;  // No metadata means no dependencies to check
    
    for (size_t i = 0; i < metadata->dependencies_count; i++) {
        EnhancedDependency* dep = &metadata->dependencies[i];
        
        // Skip optional dependencies
        if (dep->optional) continue;
        
        bool found = false;
        
        // Find matching component
        for (size_t j = 0; j < num_available_components; j++) {
            const EnhancedComponentMetadata* comp = available_components[j];
            
            if (comp && comp->id && strcmp(comp->id, dep->id) == 0) {
                // Check version compatibility if specified
                if (dep->version_req && comp->version) {
                    if (semver_satisfies(comp->version, dep->version_req)) {
                        found = true;
                        break;
                    }
                } else {
                    // No version requirement or component has no version info
                    found = true;
                    break;
                }
            }
        }
        
        if (!found) {
            // Dependency not found or version not compatible
            if (missing_dependency) {
                *missing_dependency = strdup(dep->id);
            }
            return false;
        }
    }
    
    return true;
}

// Resolve version constraints between components
// Returns the component with the best matching version, or NULL if none found
const EnhancedComponentMetadata* nexus_enhanced_metadata_resolve_version(
    const char* component_id,
    const char* version_constraint, 
    const EnhancedComponentMetadata** available_components,
    size_t num_available_components
) {
    if (!component_id || !version_constraint || !available_components || num_available_components == 0)
        return NULL;
    
    const EnhancedComponentMetadata* best_match = NULL;
    int best_match_score = -1;
    
    // Find all components matching the ID
    for (size_t i = 0; i < num_available_components; i++) {
        const EnhancedComponentMetadata* comp = available_components[i];
        
        if (!comp || !comp->id || strcmp(comp->id, component_id) != 0)
            continue;
        
        // Check version constraint
        if (!comp->version || !semver_satisfies(comp->version, version_constraint))
            continue;
        
        // Calculate a score for this match (higher is better)
        // Here we use a simple scoring system, but in a real implementation,
        // this would consider various factors like recency, exactness of match, etc.
        int score = 100;  // Base score
        
        // Prefer exact version matches over general constraints
        if (strcmp(comp->version, version_constraint) == 0)
            score += 100;
        
        // Higher version numbers get higher scores (for tie-breaking)
        // This assumes parsed_version can be compared numerically
        if (comp->parsed_version) {
            score += comp->parsed_version->major * 10000 + 
                    comp->parsed_version->minor * 100 + 
                    comp->parsed_version->patch;
        }
        
        // Update best match if this one is better
        if (score > best_match_score) {
            best_match = comp;
            best_match_score = score;
        }
    }
    
    return best_match;
}

// Free enhanced component metadata
void nexus_free_enhanced_metadata(EnhancedComponentMetadata* metadata) {
    if (!metadata) return;
    
    // Free strings
    free(metadata->id);
    free(metadata->version);
    free(metadata->description);
    
    // Free dependencies
    for (size_t i = 0; i < metadata->dependencies_count; i++) {
        free(metadata->dependencies[i].id);
        free(metadata->dependencies[i].version_req);
        free(metadata->dependencies[i].resolved_version);
    }
    free(metadata->dependencies);
    
    // Free exported symbols
    for (size_t i = 0; i < metadata->exported_count; i++) {
        free(metadata->exported_symbols[i].name);
        free(metadata->exported_symbols[i].version);
    }
    free(metadata->exported_symbols);
    
    // Free imported symbols
    for (size_t i = 0; i < metadata->imported_count; i++) {
        free(metadata->imported_symbols[i].name);
        free(metadata->imported_symbols[i].version);
    }
    free(metadata->imported_symbols);
    
    // Free parsed version
    if (metadata->parsed_version) {
        semver_free(metadata->parsed_version);
    }
    
    // Free the metadata structure itself
    free(metadata);
}

// Create a new enhanced component metadata structure
EnhancedComponentMetadata* nexus_enhanced_metadata_create(
    const char* id, 
    const char* version, 
    const char* description
) {
    EnhancedComponentMetadata* metadata = (EnhancedComponentMetadata*)malloc(sizeof(EnhancedComponentMetadata));
    memset(metadata, 0, sizeof(EnhancedComponentMetadata));
    
    if (id) metadata->id = strdup(id);
    if (version) {
        metadata->version = strdup(version);
        metadata->parsed_version = semver_parse(version);
    } else {
        metadata->version = strdup("1.0.0");
        metadata->parsed_version = semver_parse("1.0.0");
    }
    if (description) metadata->description = strdup(description);
    
    return metadata;
}

// Add a dependency with version requirement to component metadata
void nexus_enhanced_metadata_add_dependency(
    EnhancedComponentMetadata* metadata,
    const char* dep_id,
    const char* version_req,
    bool optional
) {
    if (!metadata || !dep_id) return;
    
    // Allocate or resize dependencies array
    if (metadata->dependencies == NULL) {
        metadata->dependencies = (EnhancedDependency*)malloc(sizeof(EnhancedDependency));
    } else {
        metadata->dependencies = (EnhancedDependency*)realloc(
            metadata->dependencies, 
            (metadata->dependencies_count + 1) * sizeof(EnhancedDependency)
        );
    }
    
    // Initialize new dependency
    EnhancedDependency* dep = &metadata->dependencies[metadata->dependencies_count];
    dep->id = strdup(dep_id);
    dep->version_req = version_req ? strdup(version_req) : strdup("*");
    dep->optional = optional;
    dep->resolved_version = NULL;
    
    metadata->dependencies_count++;
}

// Add an exported symbol with version to component metadata
void nexus_enhanced_metadata_add_exported_symbol(
    EnhancedComponentMetadata* metadata,
    const char* name,
    const char* version,
    int type
) {
    if (!metadata || !name) return;
    
    // Allocate or resize exported symbols array
    if (metadata->exported_symbols == NULL) {
        metadata->exported_symbols = (SymbolDefinition*)malloc(sizeof(SymbolDefinition));
    } else {
        metadata->exported_symbols = (SymbolDefinition*)realloc(
            metadata->exported_symbols, 
            (metadata->exported_count + 1) * sizeof(SymbolDefinition)
        );
    }
    
    // Add the symbol
    SymbolDefinition* symbol = &metadata->exported_symbols[metadata->exported_count];
    symbol->name = strdup(name);
    symbol->version = version ? strdup(version) : strdup(metadata->version);
    symbol->type = type;
    
    metadata->exported_count++;
}

// Add an imported symbol with version requirement to component metadata
void nexus_enhanced_metadata_add_imported_symbol(
    EnhancedComponentMetadata* metadata,
    const char* name,
    const char* version_req,
    int type
) {
    if (!metadata || !name) return;
    
    // Allocate or resize imported symbols array
    if (metadata->imported_symbols == NULL) {
        metadata->imported_symbols = (SymbolDefinition*)malloc(sizeof(SymbolDefinition));
    } else {
        metadata->imported_symbols = (SymbolDefinition*)realloc(
            metadata->imported_symbols, 
            (metadata->imported_count + 1) * sizeof(SymbolDefinition)
        );
    }
    
    // Add the symbol
    SymbolDefinition* symbol = &metadata->imported_symbols[metadata->imported_count];
    symbol->name = strdup(name);
    symbol->version = version_req ? strdup(version_req) : strdup("*");
    symbol->type = type;
    
    metadata->imported_count++;
}

// Helper function to create a JSON symbol definition
static NexusJsonValue* create_symbol_json(const SymbolDefinition* symbol) {
    NexusJsonValue* obj = nexus_json_object();
    nexus_json_object_add(obj, "name", nexus_json_string(symbol->name));
    nexus_json_object_add(obj, "version", nexus_json_string(symbol->version));
    nexus_json_object_add(obj, "type", nexus_json_number(symbol->type));
    return obj;
}

// Save enhanced metadata to a file in the new extended format
bool nexus_enhanced_metadata_save(
    const EnhancedComponentMetadata* metadata, 
    const char* output_path
) {
    if (!metadata || !output_path) return false;
    
    NexusJsonValue* root = nexus_json_object();
    
    // Basic info
    if (metadata->id) 
        nexus_json_object_add(root, "id", nexus_json_string(metadata->id));
    
    if (metadata->version) 
        nexus_json_object_add(root, "version", nexus_json_string(metadata->version));
    
    if (metadata->description) 
        nexus_json_object_add(root, "description", nexus_json_string(metadata->description));
    
    // Dependencies with version requirements
    NexusJsonValue* deps = nexus_json_array();
    for (size_t i = 0; i < metadata->dependencies_count; i++) {
        EnhancedDependency* dep = &metadata->dependencies[i];
        NexusJsonValue* dep_obj = nexus_json_object();
        
        if (dep->id)
            nexus_json_object_add(dep_obj, "id", nexus_json_string(dep->id));
        
        if (dep->version_req)
            nexus_json_object_add(dep_obj, "version_req", nexus_json_string(dep->version_req));
        
        nexus_json_object_add(dep_obj, "optional", nexus_json_bool(dep->optional));
        
        // Include resolved version if available
        if (dep->resolved_version)
            nexus_json_object_add(dep_obj, "resolved_version", 
                                nexus_json_string(dep->resolved_version));
        
        nexus_json_array_add(deps, dep_obj);
    }
    nexus_json_object_add(root, "dependencies", deps);
    
    // Exported symbols with version information
    NexusJsonValue* exported = nexus_json_array();
    for (size_t i = 0; i < metadata->exported_count; i++) {
        SymbolDefinition* symbol = &metadata->exported_symbols[i];
        if (symbol->name) {
            NexusJsonValue* sym_obj = create_symbol_json(symbol);
            nexus_json_array_add(exported, sym_obj);
        }
    }
    nexus_json_object_add(root, "exported_symbols", exported);
    
    // Imported symbols with version requirements
    NexusJsonValue* imported = nexus_json_array();
    for (size_t i = 0; i < metadata->imported_count; i++) {
        SymbolDefinition* symbol = &metadata->imported_symbols[i];
        if (symbol->name) {
            NexusJsonValue* sym_obj = create_symbol_json(symbol);
            nexus_json_array_add(imported, sym_obj);
        }
    }
    nexus_json_object_add(root, "imported_symbols", imported);
    
    // Resource usage metrics
    nexus_json_object_add(root, "memory_footprint", nexus_json_number((double)metadata->memory_footprint));
    nexus_json_object_add(root, "avg_load_time_ms", nexus_json_number(metadata->avg_load_time_ms));
    
    // Usage statistics
    nexus_json_object_add(root, "usage_count", nexus_json_number(metadata->usage_count));
    nexus_json_object_add(root, "last_used", nexus_json_number((double)metadata->last_used));
    
    // Write to file with pretty formatting
    bool success = nexus_json_write_file(root, output_path, true);
    
    // Clean up
    nexus_json_free(root);
    
    return success;
}