/**
 * @file identity.c
 * @brief Implementation of identity tactics
 * @copyright © 2025 OBINexus Computing
 */

#include <nlink/core/tactic/identity.h"
#include <stdlib.h>

void* nlink_identity(void* data, void* context) {
    // Ignore context, return data unchanged
    (void)context;
    return data;
}

void* nlink_transform_identity(void* data, void* context) {
    // Alias for nlink_identity to maintain semantic clarity
    return nlink_identity(data, context);
}

bool nlink_predicate_true(void* data, void* context) {
    // Ignore parameters, always return true
    (void)data;
    (void)context;
    return true;
}

bool nlink_predicate_false(void* data, void* context) {
    // Ignore parameters, always return false
    (void)data;
    (void)context;
    return false;
}

void nlink_consumer_noop(void* data, void* context) {
    // Do nothing with the data
    (void)data;
    (void)context;
}

int nlink_compare_identity(void* a, void* b, void* context) {
    // Compare pointers directly
    (void)context;
    
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}

size_t nlink_hash_identity(void* data, void* context) {
    // Simple hash based on pointer value
    (void)context;
    
    // Cast to size_t directly - this is implementation-defined behavior,
    // but works well as a simple identity hash
    return (size_t)data;
}

/**
 * Internal structure for conditional identity transformer
 */
typedef struct {
    bool (*condition)(void*, void*);
    void* (*transform)(void*, void*);
} conditional_identity_data;

/**
 * Internal implementation of conditional identity transformer
 */
static void* conditional_identity_impl(void* data, void* context) {
    conditional_identity_data* cid = (conditional_identity_data*)context;
    
    if (cid == NULL) {
        return data;  // No context, return unchanged
    }
    
    if (cid->condition(data, NULL)) {
        return data;  // Condition true, return unchanged
    } else {
        return cid->transform(data, NULL);  // Condition false, transform
    }
}

void* (*nlink_create_conditional_identity(bool (*condition)(void*, void*), 
                                        void* (*transform)(void*, void*)))
                                        (void*, void*) {
    // Create internal data structure
    conditional_identity_data* cid = malloc(sizeof(conditional_identity_data));
    if (cid == NULL) {
        return NULL;
    }
    
    cid->condition = condition != NULL ? condition : nlink_predicate_false;
    cid->transform = transform != NULL ? transform : nlink_transform_identity;
    
    // Note: This is a simplified implementation that leaks memory
    // A real implementation would need a way to clean up the context
    
    // Return the implementation function
    return conditional_identity_impl;
}

void* nlink_initializer_null(void* context) {
    // Return NULL as initial value
    (void)context;
    return NULL;
}

void* nlink_combiner_last(void* a, void* b, void* context) {
    // Ignore first value and context, return second value
    (void)a;
    (void)context;
    return b;
}

void* nlink_factory_identity(void* context) {
    // Return context as created object
    return context;
}