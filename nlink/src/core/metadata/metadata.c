// src/core/metadata/metadata.c
// Implementation of basic metadata system
// Author: Implementation Team

#include "nlink/core/metadata/metadata.h"

// Load component metadata from a JSON file
ComponentMetadata* nexus_load_metadata(const char* metadata_path) {
	// Allocate the metadata structure
	ComponentMetadata* metadata = (ComponentMetadata*)malloc(sizeof(ComponentMetadata));
	memset(metadata, 0, sizeof(ComponentMetadata));
	
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
	if (version) metadata->version = strdup(version);
	
	const char* description = nexus_json_object_get_string(root, "description", NULL);
	if (description) metadata->description = strdup(description);
	
	// Process dependencies
	NexusJsonValue* dependencies = nexus_json_object_get(root, "dependencies");
	if (dependencies && dependencies->type == NEXUS_JSON_ARRAY) {
		metadata->dependencies_count = dependencies->data.array.count;
		metadata->dependencies = (Dependency*)malloc(metadata->dependencies_count * sizeof(Dependency));
		
		for (size_t i = 0; i < dependencies->data.array.count; i++) {
			NexusJsonValue* dep = dependencies->data.array.items[i];
			
			if (dep->type == NEXUS_JSON_OBJECT) {
				// Initialize with defaults
				metadata->dependencies[i].id = NULL;
				metadata->dependencies[i].version = NULL;
				metadata->dependencies[i].optional = false;
				
				// Extract values
				const char* dep_id = nexus_json_object_get_string(dep, "id", NULL);
				if (dep_id) metadata->dependencies[i].id = strdup(dep_id);
				
				const char* dep_version = nexus_json_object_get_string(dep, "version", NULL);
				if (dep_version) metadata->dependencies[i].version = strdup(dep_version);
				
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
		metadata->exported_symbols = (char**)malloc(metadata->exported_count * sizeof(char*));
		
		for (size_t i = 0; i < exported->data.array.count; i++) {
			NexusJsonValue* symbol = exported->data.array.items[i];
			if (symbol->type == NEXUS_JSON_STRING) {
				metadata->exported_symbols[i] = strdup(symbol->data.string);
			} else {
				metadata->exported_symbols[i] = strdup("");  // Empty string as fallback
			}
		}
	}
	
	// Process imported symbols
	NexusJsonValue* imported = nexus_json_object_get(root, "imported_symbols");
	if (imported && imported->type == NEXUS_JSON_ARRAY) {
		metadata->imported_count = imported->data.array.count;
		metadata->imported_symbols = (char**)malloc(metadata->imported_count * sizeof(char*));
		
		for (size_t i = 0; i < imported->data.array.count; i++) {
			NexusJsonValue* symbol = imported->data.array.items[i];
			if (symbol->type == NEXUS_JSON_STRING) {
				metadata->imported_symbols[i] = strdup(symbol->data.string);
			} else {
				metadata->imported_symbols[i] = strdup("");  // Empty string as fallback
			}
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

// Free component metadata
void nexus_free_metadata(ComponentMetadata* metadata) {
	if (!metadata) return;
	
	// Free strings
	free(metadata->id);
	free(metadata->version);
	free(metadata->description);
	
	// Free dependencies
	for (size_t i = 0; i < metadata->dependencies_count; i++) {
		free(metadata->dependencies[i].id);
		free(metadata->dependencies[i].version);
	}
	free(metadata->dependencies);
	
	// Free exported symbols
	for (size_t i = 0; i < metadata->exported_count; i++) {
		free(metadata->exported_symbols[i]);
	}
	free(metadata->exported_symbols);
	
	// Free imported symbols
	for (size_t i = 0; i < metadata->imported_count; i++) {
		free(metadata->imported_symbols[i]);
	}
	free(metadata->imported_symbols);
	
	// Free the metadata structure itself
	free(metadata);
}

// Create a new component metadata structure
ComponentMetadata* nexus_metadata_create(const char* id, const char* version, const char* description) {
	ComponentMetadata* metadata = (ComponentMetadata*)malloc(sizeof(ComponentMetadata));
	memset(metadata, 0, sizeof(ComponentMetadata));
	
	if (id) metadata->id = strdup(id);
	if (version) metadata->version = strdup(version);
	if (description) metadata->description = strdup(description);
	
	return metadata;
}

// Update component usage statistics
void nexus_metadata_track_usage(ComponentMetadata* metadata) {
	if (!metadata) return;
	
	metadata->usage_count++;
	metadata->last_used = time(NULL);
}

// Check if component has been used recently
bool nexus_metadata_recently_used(const ComponentMetadata* metadata, time_t seconds_threshold) {
	if (!metadata) return false;
	
	time_t now = time(NULL);
	return (now - metadata->last_used) < seconds_threshold;
}

// Add a dependency to component metadata
void nexus_metadata_add_dependency(ComponentMetadata* metadata, const char* dep_id, const char* dep_version, bool optional) {
	if (!metadata || !dep_id) return;
	
	// Allocate or resize dependencies array
	if (metadata->dependencies == NULL) {
		metadata->dependencies = (Dependency*)malloc(sizeof(Dependency));
	} else {
		metadata->dependencies = (Dependency*)realloc(metadata->dependencies, 
													 (metadata->dependencies_count + 1) * sizeof(Dependency));
	}
	
	// Initialize new dependency
	Dependency* dep = &metadata->dependencies[metadata->dependencies_count];
	dep->id = strdup(dep_id);
	dep->version = dep_version ? strdup(dep_version) : NULL;
	dep->optional = optional;
	
	metadata->dependencies_count++;
}

// Add an exported symbol to component metadata
void nexus_metadata_add_exported_symbol(ComponentMetadata* metadata, const char* symbol) {
	if (!metadata || !symbol) return;
	
	// Allocate or resize exported symbols array
	if (metadata->exported_symbols == NULL) {
		metadata->exported_symbols = (char**)malloc(sizeof(char*));
	} else {
		metadata->exported_symbols = (char**)realloc(metadata->exported_symbols, 
													(metadata->exported_count + 1) * sizeof(char*));
	}
	
	// Add the symbol
	metadata->exported_symbols[metadata->exported_count] = strdup(symbol);
	metadata->exported_count++;
}

// Add an imported symbol to component metadata
void nexus_metadata_add_imported_symbol(ComponentMetadata* metadata, const char* symbol) {
	if (!metadata || !symbol) return;
	
	// Allocate or resize imported symbols array
	if (metadata->imported_symbols == NULL) {
		metadata->imported_symbols = (char**)malloc(sizeof(char*));
	} else {
		metadata->imported_symbols = (char**)realloc(metadata->imported_symbols, 
													(metadata->imported_count + 1) * sizeof(char*));
	}
	
	// Add the symbol
	metadata->imported_symbols[metadata->imported_count] = strdup(symbol);
	metadata->imported_count++;
}

// Save metadata to a file
bool nexus_metadata_save(const ComponentMetadata* metadata, const char* output_path) {
	if (!metadata || !output_path) return false;
	
	NexusJsonValue* root = nexus_json_object();
	
	// Basic info
	if (metadata->id) 
		nexus_json_object_add(root, "id", nexus_json_string(metadata->id));
	
	if (metadata->version) 
		nexus_json_object_add(root, "version", nexus_json_string(metadata->version));
	
	if (metadata->description) 
		nexus_json_object_add(root, "description", nexus_json_string(metadata->description));
	
	// Dependencies
	NexusJsonValue* deps = nexus_json_array();
	for (size_t i = 0; i < metadata->dependencies_count; i++) {
		Dependency* dep = &metadata->dependencies[i];
		NexusJsonValue* dep_obj = nexus_json_object();
		
		if (dep->id)
			nexus_json_object_add(dep_obj, "id", nexus_json_string(dep->id));
		
		if (dep->version)
			nexus_json_object_add(dep_obj, "version", nexus_json_string(dep->version));
		
		nexus_json_object_add(dep_obj, "optional", nexus_json_bool(dep->optional));
		
		nexus_json_array_add(deps, dep_obj);
	}
	nexus_json_object_add(root, "dependencies", deps);
	
	// Exported symbols
	NexusJsonValue* exported = nexus_json_array();
	for (size_t i = 0; i < metadata->exported_count; i++) {
		if (metadata->exported_symbols[i]) {
			nexus_json_array_add(exported, nexus_json_string(metadata->exported_symbols[i]));
		}
	}
	nexus_json_object_add(root, "exported_symbols", exported);
	
	// Imported symbols
	NexusJsonValue* imported = nexus_json_array();
	for (size_t i = 0; i < metadata->imported_count; i++) {
		if (metadata->imported_symbols[i]) {
			nexus_json_array_add(imported, nexus_json_string(metadata->imported_symbols[i]));
		}
	}
	nexus_json_object_add(root, "imported_symbols", imported);
	
	// Resource usage metrics
	nexus_json_object_add(root, "memory_footprint", nexus_json_number((double)metadata->memory_footprint));
	nexus_json_object_add(root, "avg_load_time_ms", nexus_json_number(metadata->avg_load_time_ms));
	
	// Usage statistics (optional, for debugging)
	nexus_json_object_add(root, "usage_count", nexus_json_number(metadata->usage_count));
	nexus_json_object_add(root, "last_used", nexus_json_number((double)metadata->last_used));
	
	// Write to file
	bool success = nexus_json_write_file(root, output_path, true);
	
	// Clean up
	nexus_json_free(root);
	
	return success;
}

// Check if component has all required dependencies
bool nexus_metadata_check_dependencies(
	const ComponentMetadata* metadata, 
	const ComponentMetadata** available_components,
	size_t num_available_components,
	char** missing_dependency
) {
	if (!metadata) return true;  // No metadata means no dependencies to check
	
	for (size_t i = 0; i < metadata->dependencies_count; i++) {
		Dependency* dep = &metadata->dependencies[i];
		
		// Skip optional dependencies
		if (dep->optional) continue;
		
		bool found = false;
		
		// Check if dependency is in available components
		for (size_t j = 0; j < num_available_components; j++) {
			const ComponentMetadata* comp = available_components[j];
			
			if (comp && comp->id && strcmp(comp->id, dep->id) == 0) {
				// Found the dependency
				// Check version compatibility if specified
				if (dep->version && comp->version) {
					// For simplicity, we just check for exact match
					// In a real system, you'd implement semantic versioning comparison
					if (strcmp(dep->version, comp->version) == 0) {
						found = true;
						break;
					}
				} else {
					// No version requirement, or component has no version info
					found = true;
					break;
				}
			}
		}
		
		if (!found) {
			// Dependency not found
			if (missing_dependency) {
				*missing_dependency = strdup(dep->id);
			}
			return false;
		}
	}
	
	return true;
}

// Create a sample metadata.json file for a component
void nexus_generate_metadata_template(const char* component_id, const char* output_path) {
	NexusJsonValue* root = nexus_json_object();
	
	// Basic info
	nexus_json_object_add(root, "id", nexus_json_string(component_id));
	nexus_json_object_add(root, "version", nexus_json_string("0.1.0"));
	nexus_json_object_add(root, "description", nexus_json_string("Auto-generated metadata template"));
	
	// Empty dependencies array
	NexusJsonValue* deps = nexus_json_array();
	nexus_json_object_add(root, "dependencies", deps);
	
	// Empty exported symbols array
	NexusJsonValue* exported = nexus_json_array();
	nexus_json_object_add(root, "exported_symbols", exported);
	
	// Empty imported symbols array
	NexusJsonValue* imported = nexus_json_array();
	nexus_json_object_add(root, "imported_symbols", imported);
	
	// Resource estimates
	nexus_json_object_add(root, "memory_footprint", nexus_json_number(0));
	nexus_json_object_add(root, "avg_load_time_ms", nexus_json_number(0.0));
	
	// Write to file
	if (!nexus_json_write_file(root, output_path, true)) {
		fprintf(stderr, "Error: Could not write metadata to: %s\n", output_path);
	} else {
		printf("Generated metadata template at: %s\n", output_path);
	}
	
	// Clean up
	nexus_json_free(root);
}
