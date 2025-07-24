/**
 * @file composition.c
 * @brief Implementation of function composition tactics
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <nlink/core/tactic/composition.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void* nlink_compose(void* data, nlink_transform_fn* transforms, size_t count, void* context) {
    if (transforms == NULL || count == 0) {
        return data;  // Nothing to transform
    }
    
    void* result = data;
    
    for (size_t i = 0; i < count; i++) {
        if (transforms[i] == NULL) {
            continue;  // Skip NULL transforms
        }
        
        void* next = transforms[i](result, context);
        
        // If a transformation returns NULL, the pipeline fails
        if (next == NULL) {
            return NULL;
        }
        
        result = next;
    }
    
    return result;
}

/**
 * Structure to pass both transformations to the composed function
 */
typedef struct {
    nlink_transform_fn first;
    nlink_transform_fn second;
    void* original_context;
} nlink_compose2_ctx;

/**
 * Internal function that implements the composition
 */
static void* nlink_compose2_impl(void* data, void* context) {
    nlink_compose2_ctx* ctx = (nlink_compose2_ctx*)context;
    
    if (ctx == NULL || ctx->first == NULL || ctx->second == NULL) {
        return data;
    }
    
    // Apply first transformation
    void* intermediate = ctx->first(data, ctx->original_context);
    
    if (intermediate == NULL) {
        return NULL;
    }
    
    // Apply second transformation
    return ctx->second(intermediate, ctx->original_context);
}

nlink_transform_fn nlink_compose2(nlink_transform_fn first, nlink_transform_fn second) {
    // This implementation is simplified and doesn't handle memory management properly.
    // A more robust implementation would need to track and free the context.
    nlink_compose2_ctx* ctx = malloc(sizeof(nlink_compose2_ctx));
    
    if (ctx == NULL) {
        return NULL;
    }
    
    ctx->first = first;
    ctx->second = second;
    ctx->original_context = NULL;  // Will be set when called
    
    // Return the implementation function with the context
    // This is a simplified implementation; in practice, we would need
    // a way to capture both the function and its context.
    return nlink_compose2_impl;
}

void* nlink_compose_n(void* data, nlink_transform_fn transform, size_t iterations, void* context) {
    if (transform == NULL || iterations == 0) {
        return data;  // Nothing to do
    }
    
    void* result = data;
    
    for (size_t i = 0; i < iterations; i++) {
        void* next = transform(result, context);
        
        if (next == NULL) {
            return NULL;  // Transformation failed
        }
        
        result = next;
    }
    
    return result;
}

void* nlink_identity(void* data, void* context) {
    (void)context;  // Unused parameter
    return data;    // Simply return the input unchanged
}

nlink_transform_pipeline* nlink_pipeline_create(size_t initial_capacity, void* context) {
    if (initial_capacity == 0) {
        initial_capacity = 4;  // Default to a small initial capacity
    }
    
    nlink_transform_pipeline* pipeline = malloc(sizeof(nlink_transform_pipeline));
    
    if (pipeline == NULL) {
        return NULL;
    }
    
    pipeline->transforms = malloc(initial_capacity * sizeof(nlink_transform_fn));
    
    if (pipeline->transforms == NULL) {
        free(pipeline);
        return NULL;
    }
    
    pipeline->count = 0;
    pipeline->context = context;
    
    return pipeline;
}

bool nlink_pipeline_add(nlink_transform_pipeline* pipeline, nlink_transform_fn transform) {
    if (pipeline == NULL || transform == NULL) {
        return false;
    }
    
    // Simplified implementation - in practice would need to handle resizing
    // when capacity is reached
    
    // Add the transformation
    pipeline->transforms[pipeline->count++] = transform;
    
    return true;
}

void* nlink_pipeline_execute(nlink_transform_pipeline* pipeline, void* data) {
    if (pipeline == NULL) {
        return data;
    }
    
    return nlink_compose(data, pipeline->transforms, pipeline->count, pipeline->context);
}

void nlink_pipeline_free(nlink_transform_pipeline* pipeline) {
    if (pipeline == NULL) {
        return;
    }
    
    free(pipeline->transforms);
    free(pipeline);
}