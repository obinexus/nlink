/**
 * @file tactic.c
 * @brief Implementation of tactical programming paradigms
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "tactic.h"
#include <stdlib.h>
#include <string.h>

void* nlink_compose(void* data, nlink_transform_fn* transforms, int count, void* context) {
    void* result = data;
    
    for (int i = 0; i < count; i++) {
        if (transforms[i] != NULL) {
            result = transforms[i](result, context);
            if (result == NULL) {
                return NULL;
            }
        }
    }
    
    return result;
}

void* nlink_identity(void* data, void* context) {
    (void)context; // Unused parameter
    return data;
}

void* nlink_aggregate(void** items, size_t count, nlink_transform_fn operation, void* context) {
    if (items == NULL || count == 0 || operation == NULL) {
        return NULL;
    }
    
    void* result = NULL;
    
    for (size_t i = 0; i < count; i++) {
        void* transformed = operation(items[i], context);
        
        // First item becomes the initial result
        if (i == 0) {
            result = transformed;
        } else {
            // Implementation-specific aggregation logic would go here
            // For now, just return the last transformed item
            result = transformed;
        }
    }
    
    return result;
}

void* nlink_traverse(void* structure, nlink_transform_fn visit, void* context) {
    // Basic implementation - for real use, would need structure-specific logic
    if (structure == NULL || visit == NULL) {
        return NULL;
    }
    
    return visit(structure, context);
}

void* nlink_when(void* data, nlink_predicate_fn condition, nlink_transform_fn transform, void* context) {
    if (data == NULL || condition == NULL || transform == NULL) {
        return data;
    }
    
    if (condition(data, context)) {
        return transform(data, context);
    }
    
    return data;
}

void* nlink_iterate(void* data, nlink_predicate_fn condition, nlink_transform_fn transform, void* context) {
    if (data == NULL || condition == NULL || transform == NULL) {
        return data;
    }
    
    void* result = data;
    
    while (condition(result, context)) {
        result = transform(result, context);
        if (result == NULL) {
            break;
        }
    }
    
    return result;
}
