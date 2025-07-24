/**
 * @file config_validator.c
 * @brief Implementation of configuration validation
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/config/config_validator.h"
#include "nlink/core/common/nexus_error.h"
#include <string.h>
#include <ctype.h>

#define RETURN_IF_ERROR(res) if ((res).status != NEXUS_STATUS_SUCCESS) return (res)

// Helper function to check if a key name is valid
static bool is_valid_key_name(const char* key) {
    if (!key || !*key) return false;
    
    // First character must be a letter or underscore
    if (!isalpha(key[0]) && key[0] != '_') return false;
    
    // Remaining characters must be letters, digits, or underscores
    for (size_t i = 1; key[i]; i++) {
        if (!isalnum(key[i]) && key[i] != '_') return false;
    }
    
    return true;
}

// Helper function to check for duplicate property keys
static bool has_duplicate_properties(nlink_token_config_section* section) {
    if (!section || !section->properties || section->property_count <= 1) return false;
    
    for (size_t i = 0; i < section->property_count; i++) {
        nlink_token_config_property* prop_i = (nlink_token_config_property*)section->properties[i];
        
        for (size_t j = i + 1; j < section->property_count; j++) {
            nlink_token_config_property* prop_j = (nlink_token_config_property*)section->properties[j];
            
            if (strcmp(prop_i->key, prop_j->key) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

NexusResult nlink_validate_config_key(const char* key) {
    if (!is_valid_key_name(key)) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_VALIDATION_FAILED,
            "Invalid configuration key name, must match [a-zA-Z_][a-zA-Z0-9_]*",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    return nexus_success(NULL, NULL);
}

NexusResult nlink_validate_config_property(nlink_token_config_property* property,
                                          NexusContext* context) {
    if (!property) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Property cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Validate key name
    NexusResult result = nlink_validate_config_key(property->key);
    RETURN_IF_ERROR(result);
    
    // Check if the property is required but has no value
    if (property->is_required && property->value == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_VALIDATION_FAILED,
            "Required property has no value",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    return nexus_success(property, NULL);
}

NexusResult nlink_validate_config_section(nlink_token_config_section* section, 
                                         NexusContext* context) {
    if (!section) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Section cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Validate section name
    if (!is_valid_key_name(section->section_name)) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_VALIDATION_FAILED,
            "Invalid section name, must match [a-zA-Z_][a-zA-Z0-9_]*",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Check for duplicate properties
    if (has_duplicate_properties(section)) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_VALIDATION_FAILED,
            "Section contains duplicate property keys",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Validate all properties
    for (size_t i = 0; i < section->property_count; i++) {
        nlink_token_config_property* property = 
            (nlink_token_config_property*)section->properties[i];
            
        NexusResult result = nlink_validate_config_property(property, context);
        RETURN_IF_ERROR(result);
    }
    
    return nexus_success(section, NULL);
}

NexusResult nlink_validate_config_tree(nlink_token_config_section* root,
                                      NexusContext* context) {
    if (!root) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Root section cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Validate the root section itself
    NexusResult result = nlink_validate_config_section(root, context);
    RETURN_IF_ERROR(result);
    
    // Create a set of section names to track uniqueness
    // In a real implementation, this would be a hash set or similar
    // For simplicity, we'll assume there's a function to check uniqueness
    
    // Traverse the config tree and validate each section
    // This is a simplified implementation; a real one would traverse the tree
    
    return nexus_success(root, NULL);
}