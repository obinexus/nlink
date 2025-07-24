// nlink/core/tatit/nlink_tatit_compose.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_COMPOSE_H
#ifndef OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_COMPOSE_H
#define OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_COMPOSE_H

/**
 * @file composition.h
 * @brief Function composition tactics for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module implements the composition tactical pattern,
 * allowing multiple operations to be chained together.
 */

#ifndef NLINK_TACTIC_COMPOSITION_H
#define NLINK_TACTIC_COMPOSITION_H

#include <stddef.h>

/**
 * Function pointer type for transformation functions
 */
typedef void* (*nlink_transform_fn)(void* data, void* context);

/**
 * @brief Compose multiple transformation functions into a single operation
 *
 * This function applies a sequence of transformations to the input data,
 * where each transformation receives the output of the previous transformation.
 *
 * @param data Initial input data
 * @param transforms Array of transformation functions
 * @param count Number of transformations
 * @param context Optional context passed to each transformation
 * @return Result of applying all transformations in sequence
 */
void* nlink_compose(void* data, nlink_transform_fn* transforms, size_t count, void* context);

/**
 * @brief Compose two transformation functions
 *
 * Helper function to compose exactly two transformations.
 *
 * @param first First transformation to apply
 * @param second Second transformation to apply
 * @return A new transformation function that applies both in sequence
 */
nlink_transform_fn nlink_compose2(nlink_transform_fn first, nlink_transform_fn second);

/**
 * @brief Apply a transformation N times
 *
 * @param data Input data
 * @param transform Transformation to apply
 * @param iterations Number of times to apply the transformation
 * @param context Optional context passed to each transformation
 * @return Result after applying the transformation N times
 */
void* nlink_compose_n(void* data, nlink_transform_fn transform, size_t iterations, void* context);

/**
 * @brief Identity transformation (no-op)
 *
 * @param data Input data
 * @param context Ignored
 * @return Input data unchanged
 */
void* nlink_identity(void* data, void* context);

/**
 * @brief Macro for creating a composed transformation from two functions
 */
#define NLINK_COMPOSE(f, g) \
    (void*(*)(void*, void*))nlink_compose2((nlink_transform_fn)(f), (nlink_transform_fn)(g))

/**
 * Transformation application structure
 */
typedef struct nlink_transform_pipeline {
    nlink_transform_fn* transforms;  // Array of transformations
    size_t count;                    // Number of transformations
    void* context;                   // Shared context for all transformations
} nlink_transform_pipeline;

/**
 * @brief Create a new transformation pipeline
 *
 * @param initial_capacity Initial number of transformations to allocate space for
 * @param context Shared context for all transformations
 * @return Newly allocated pipeline structure
 */
nlink_transform_pipeline* nlink_pipeline_create(size_t initial_capacity, void* context);

/**
 * @brief Add a transformation to the pipeline
 *
 * @param pipeline Pipeline to add to
 * @param transform Transformation to add
 * @return true if successful, false otherwise
 */
bool nlink_pipeline_add(nlink_transform_pipeline* pipeline, nlink_transform_fn transform);

/**
 * @brief Execute a transformation pipeline on input data
 *
 * @param pipeline Pipeline to execute
 * @param data Input data
 * @return Result of applying all transformations
 */
void* nlink_pipeline_execute(nlink_transform_pipeline* pipeline, void* data);

/**
 * @brief Free resources associated with a pipeline
 *
 * @param pipeline Pipeline to free
 */
void nlink_pipeline_free(nlink_transform_pipeline* pipeline);


#endif // OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_COMPOSE_H
