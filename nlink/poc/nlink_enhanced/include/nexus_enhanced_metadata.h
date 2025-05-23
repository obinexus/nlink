// include/nexus_enhanced_metadata.h
// Enhanced metadata system with version support for NexusLink
// Author: Implementation Team

#ifndef NEXUS_ENHANCED_METADATA_H
#define NEXUS_ENHANCED_METADATA_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "nexus_json.h"
#include "nexus_semver.h"

// Enhanced dependency structure with version requirements
typedef struct {
    char* id;                 // Component ID
    char* version_req;        // Version requirement (e.g., "^1.2.3")
    bool optional;            // Whether this dependency is optional
    char* resolved_version;   // The actual version that was resolved (NULL if not resolved)
} EnhancedDependency;

// Symbol definition structure
typedef struct {
    char* name;               // Symbol name
    char* version;            // Symbol version
    int type;                 // Symbol type (0=function, 1=variable, 2=type, 3=constant)
} SymbolDefinition;

// Enhanced component metadata structure
typedef struct {
    char* id;                 // Component ID
    char* version;            // Component version (semantic versioning)
    char* description;        // Component description
    
    // Dependencies
    EnhancedDependency* dependencies;
    size_t dependencies_count;
    
    // Exported symbols
    SymbolDefinition* exported_symbols;
    size_t exported_count;
    
    // Imported symbols (may include version requirements)
    SymbolDefinition* imported_symbols;
    size_t imported_count;
    
    // Resource usage metrics
    size_t memory_footprint;
    double avg_load_time_ms;
    
    // Usage tracking
    time_t last_used;
    int usage_count;
    bool loaded;
    
    // Version information
    SemVer* parsed_version;   // Parsed version for quick comparison
} EnhancedComponentMetadata;

// Load enhanced component metadata from a JSON file
EnhancedComponentMetadata* nexus_load_enhanced_metadata(const char* metadata_path);

// Free enhanced component metadata
void nexus_free_enhanced_metadata(EnhancedComponentMetadata* metadata);

// Create a new enhanced component metadata structure
EnhancedComponentMetadata* nexus_enhanced_metadata_create(
    const char* id, 
    const char* version, 
    const char* description
);

// Add a dependency with version requirement to component metadata
void nexus_enhanced_metadata_add_dependency(
    EnhancedComponentMetadata* metadata,
    const char* dep_id,
    const char* version_req,
    bool optional
);

// Add an exported symbol with version to component metadata
void nexus_enhanced_metadata_add_exported_symbol(
    EnhancedComponentMetadata* metadata,
    const char* name,
    const char* version,
    int type
);

// Add an imported symbol with version requirement to component metadata
void nexus_enhanced_metadata_add_imported_symbol(
    EnhancedComponentMetadata* metadata,
    const char* name,
    const char* version_req,
    int type
);

// Save enhanced metadata to a file in the new extended format
bool nexus_enhanced_metadata_save(
    const EnhancedComponentMetadata* metadata, 
    const char* output_path
);

// Update component usage statistics
void nexus_enhanced_metadata_track_usage(EnhancedComponentMetadata* metadata);

// Check if component has been used recently
bool nexus_enhanced_metadata_recently_used(
    const EnhancedComponentMetadata* metadata, 
    time_t seconds_threshold
);

// Check version compatibility between components
bool nexus_enhanced_metadata_check_version_compatibility(
    const EnhancedComponentMetadata* component,
    const EnhancedComponentMetadata* dependency
);

// Check if a component satisfies all its dependencies
bool nexus_enhanced_metadata_check_dependencies(
    const EnhancedComponentMetadata* metadata,
    const EnhancedComponentMetadata** available_components,
    size_t num_available_components,
    char** missing_dependency
);

// Resolve version constraints between components
// Returns the component with the best matching version, or NULL if none found
const EnhancedComponentMetadata* nexus_enhanced_metadata_resolve_version(
    const char* component_id,
    const char* version_constraint, 
    const EnhancedComponentMetadata** available_components,
    size_t num_available_components
);

#endif // NEXUS_ENHANCED_METADATA_H