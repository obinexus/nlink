/**
 * @file type.c
 * @brief Implementation of the NexusLink type system
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "type.h"
#include <stdlib.h>
#include <string.h>

#define MAX_TYPES 256

// Registry of registered types
static struct {
    nlink_type types[MAX_TYPES];
    size_t count;
} type_registry = {0};

nlink_type_id nlink_register_type(const char* name, size_t size, 
                                 nlink_type_category category, nlink_type_ops* ops) {
    if (name == NULL || type_registry.count >= MAX_TYPES) {
        return 0; // Invalid type ID
    }
    
    // Check for existing type with the same name
    for (size_t i = 0; i < type_registry.count; i++) {
        if (strcmp(type_registry.types[i].name, name) == 0) {
            return type_registry.types[i].id; // Return existing type ID
        }
    }
    
    // Create new type
    nlink_type_id id = (nlink_type_id)(type_registry.count + 1);
    nlink_type* type = &type_registry.types[type_registry.count++];
    
    type->id = id;
    type->name = strdup(name);
    type->size = size;
    type->category = category;
    
    // Store ops in metadata (simplified implementation)
    type->metadata = ops ? malloc(sizeof(nlink_type_ops)) : NULL;
    if (type->metadata && ops) {
        memcpy(type->metadata, ops, sizeof(nlink_type_ops));
    }
    
    return id;
}

const nlink_type* nlink_get_type(nlink_type_id id) {
    if (id == 0 || id > type_registry.count) {
        return NULL;
    }
    
    return &type_registry.types[id - 1];
}

const nlink_type* nlink_get_type_by_name(const char* name) {
    if (name == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < type_registry.count; i++) {
        if (strcmp(type_registry.types[i].name, name) == 0) {
            return &type_registry.types[i];
        }
    }
    
    return NULL;
}

bool nlink_is_type(void* value, nlink_type_id type) {
    // Basic implementation - real implementation would depend on runtime type information
    (void)value; // Unused in this simplified implementation
    
    return nlink_get_type(type) != NULL;
}

void* nlink_transform_typed(void* value, nlink_type_id type_id, void* context) {
    if (value == NULL) {
        return NULL;
    }
    
    const nlink_type* type = nlink_get_type(type_id);
    if (type == NULL || type->metadata == NULL) {
        return value; // No type or no ops, return unchanged
    }
    
    nlink_type_ops* ops = (nlink_type_ops*)type->metadata;
    if (ops->transform == NULL) {
        return value; // No transform op, return unchanged
    }
    
    return ops->transform(value, context);
}
