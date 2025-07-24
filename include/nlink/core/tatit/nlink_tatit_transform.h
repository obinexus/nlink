// nlink/core/tatit/nlink_tatit_transform.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRANSFORM_H
#ifndef OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRANSFORM_H
#define OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRANSFORM_H

/**
 * @file transformation.h
 * @brief Transformation tactics for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module implements the transformation tactical pattern,
 * allowing data structures to be systematically modified.
 */

#ifndef NLINK_TACTIC_TRANSFORMATION_H
#define NLINK_TACTIC_TRANSFORMATION_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Function pointer types for transformations
 */
typedef void* (*nlink_transformer_fn)(void* data, void* context);
typedef bool (*nlink_condition_fn)(void* data, void* context);
typedef void* (*nlink_factory_fn)(void* context);

/**
 * Transformation configuration
 */
typedef struct nlink_transform_config {
    nlink_transformer_fn transform_fn;    // Transformation function
    nlink_condition_fn condition_fn;      // Condition for conditional transforms
    nlink_factory_fn factory_fn;          // Factory for creating new objects
    void* context;                        // Context for transformation
    bool preserve_original;               // Whether to preserve original data
    bool abort_on_error;                  // Whether to abort on transformation error
} nlink_transform_config;

/**
 * @brief Create a new transformation configuration
 *
 * @param transform_fn Transformation function
 * @param context Context for transformation
 * @return New transformation configuration
 */
nlink_transform_config* nlink_transform_config_create(nlink_transformer_fn transform_fn, 
                                                    void* context);

/**
 * @brief Set condition function for conditional transformation
 *
 * @param config Transformation configuration
 * @param condition_fn Condition function
 */
void nlink_transform_config_set_condition(nlink_transform_config* config, 
                                         nlink_condition_fn condition_fn);

/**
 * @brief Set factory function for object creation
 *
 * @param config Transformation configuration
 * @param factory_fn Factory function
 */
void nlink_transform_config_set_factory(nlink_transform_config* config, 
                                       nlink_factory_fn factory_fn);

/**
 * @brief Set whether to preserve original data
 *
 * @param config Transformation configuration
 * @param preserve Whether to preserve original data
 */
void nlink_transform_config_set_preserve(nlink_transform_config* config, 
                                        bool preserve);

/**
 * @brief Set whether to abort on transformation error
 *
 * @param config Transformation configuration
 * @param abort_on_error Whether to abort on error
 */
void nlink_transform_config_set_abort_on_error(nlink_transform_config* config, 
                                              bool abort_on_error);

/**
 * @brief Free transformation configuration
 *
 * @param config Configuration to free
 */
void nlink_transform_config_free(nlink_transform_config* config);

/**
 * @brief Transform a single value
 *
 * @param data Data to transform
 * @param transform_fn Transformation function
 * @param context Context for transformation
 * @return Transformed data
 */
void* nlink_transform(void* data, nlink_transformer_fn transform_fn, void* context);

/**
 * @brief Conditionally transform a value
 *
 * @param data Data to transform
 * @param condition_fn Condition function
 * @param transform_fn Transformation function
 * @param context Context for transformation
 * @return Transformed data if condition is met, original data otherwise
 */
void* nlink_transform_if(void* data, nlink_condition_fn condition_fn, 
                        nlink_transformer_fn transform_fn, void* context);

/**
 * @brief Transform an array of values
 *
 * @param items Array of items to transform
 * @param count Number of items
 * @param config Transformation configuration
 * @return Array of transformed items
 */
void** nlink_transform_array(void** items, size_t count, 
                            nlink_transform_config* config);

/**
 * @brief Transform multiple values with different transformations
 *
 * @param data Array of data to transform
 * @param transforms Array of transformation functions
 * @param count Number of items/transformations
 * @param context Context for transformations
 * @return Array of transformed data
 */
void** nlink_transform_multi(void** data, nlink_transformer_fn* transforms, 
                            size_t count, void* context);

/**
 * @brief Create a normalized value
 *
 * @param value Value to normalize
 * @param min Minimum value in range
 * @param max Maximum value in range
 * @param target_min Target minimum (default 0.0)
 * @param target_max Target maximum (default 1.0)
 * @return Normalized value in target range
 */
double nlink_normalize(double value, double min, double max, 
                      double target_min, double target_max);

/**
 * @brief Normalize an array of double values
 *
 * @param values Array of values to normalize
 * @param count Number of values
 * @param target_min Target minimum (default 0.0)
 * @param target_max Target maximum (default 1.0)
 * @return Array of normalized values
 */
double* nlink_normalize_array(double* values, size_t count, 
                             double target_min, double target_max);

/**
 * @brief Apply a sequence of transformations
 *
 * @param initial Initial value
 * @param transforms Array of transformation functions
 * @param count Number of transformations
 * @param context Context for transformations
 * @return Result of applying all transformations
 */
void* nlink_transform_sequence(void* initial, nlink_transformer_fn* transforms, 
                              size_t count, void* context);

/**
 * Pre-defined transformation functions
 */

/**
 * @brief Identity transformation (no change)
 *
 * @param data Input data
 * @param context Ignored
 * @return Input data unchanged
 */
void* nlink_transform_identity(void* data, void* context);

/**
 * @brief Cast data to a different type
 * 
 * This function expects context to be a function pointer that
 * performs the actual type casting.
 *
 * @param data Data to cast
 * @param context Cast function
 * @return Cast data
 */
void* nlink_transform_cast(void* data, void* context);

/**
 * @brief Stringify data
 *
 * @param data Data to stringify
 * @param context Optional format string
 * @return String representation
 */
void* nlink_transform_stringify(void* data, void* context);

/**
 * @brief Parse string into a value
 *
 * @param data String to parse
 * @param context Parse function
 * @return Parsed value
 */
void* nlink_transform_parse(void* data, void* context);

/**
 * Macros for common transformations
 */
#define NLINK_TRANSFORM(data, func, ctx) \
    nlink_transform((void*)(data), (nlink_transformer_fn)(func), (ctx))

#define NLINK_TRANSFORM_IF(data, cond, func, ctx) \
    nlink_transform_if((void*)(data), (nlink_condition_fn)(cond), \
                       (nlink_transformer_fn)(func), (ctx))


#endif // OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRANSFORM_H
