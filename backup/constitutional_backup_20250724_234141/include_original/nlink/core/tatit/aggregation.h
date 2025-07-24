/**
 * @file aggregation.h
 * @brief Aggregation tactics for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module implements the aggregation tactical pattern,
 * allowing data or operations to be grouped together for holistic processing.
 */

#ifndef NLINK_TACTIC_AGGREGATION_H
#define NLINK_TACTIC_AGGREGATION_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Function pointer types for aggregation operations
 */
typedef void* (*nlink_aggregator_fn)(void** items, size_t count, void* context);
typedef void* (*nlink_combiner_fn)(void* a, void* b, void* context);
typedef void* (*nlink_initializer_fn)(void* context);

/**
 * Aggregation result structure
 */
typedef struct nlink_aggregation_result {
    void* min;             // Minimum value (if applicable)
    void* max;             // Maximum value (if applicable)
    void* sum;             // Sum of values (if applicable)
    void* average;         // Average value (if applicable)
    double numeric_sum;    // Numeric sum (if applicable)
    double numeric_avg;    // Numeric average (if applicable)
    size_t count;          // Number of items processed
    void* custom;          // Custom aggregation result
} nlink_aggregation_result;

/**
 * Aggregation operation type
 */
typedef enum nlink_aggregation_op {
    NLINK_AGGREGATION_CUSTOM,     // Custom aggregation
    NLINK_AGGREGATION_MIN,        // Minimum value
    NLINK_AGGREGATION_MAX,        // Maximum value
    NLINK_AGGREGATION_SUM,        // Sum of values
    NLINK_AGGREGATION_AVERAGE,    // Average value
    NLINK_AGGREGATION_COUNT,      // Count of values
    NLINK_AGGREGATION_GROUP,      // Group by key
    NLINK_AGGREGATION_CONCAT      // Concatenate values
} nlink_aggregation_op;

/**
 * Aggregation configuration
 */
typedef struct nlink_aggregation_config {
    nlink_aggregation_op operation;       // Operation to perform
    nlink_aggregator_fn custom_fn;        // Custom aggregation function
    nlink_combiner_fn combiner_fn;        // Function to combine two values
    nlink_initializer_fn initializer_fn;  // Function to create initial value
    int (*compare_fn)(const void*, const void*);  // Comparison function
    void* context;                        // Context for custom functions
} nlink_aggregation_config;

/**
 * @brief Create a new aggregation configuration
 *
 * @param operation Aggregation operation to perform
 * @return New aggregation configuration structure
 */
nlink_aggregation_config* nlink_aggregation_config_create(nlink_aggregation_op operation);

/**
 * @brief Set custom aggregation function
 *
 * @param config Aggregation configuration
 * @param custom_fn Custom aggregation function
 * @param context Context to pass to the function
 */
void nlink_aggregation_config_set_custom(nlink_aggregation_config* config, 
                                        nlink_aggregator_fn custom_fn, 
                                        void* context);

/**
 * @brief Set combiner function for aggregation
 *
 * @param config Aggregation configuration
 * @param combiner_fn Function to combine two values
 */
void nlink_aggregation_config_set_combiner(nlink_aggregation_config* config, 
                                          nlink_combiner_fn combiner_fn);

/**
 * @brief Set initializer function for aggregation
 *
 * @param config Aggregation configuration
 * @param initializer_fn Function to create initial value
 */
void nlink_aggregation_config_set_initializer(nlink_aggregation_config* config, 
                                            nlink_initializer_fn initializer_fn);

/**
 * @brief Set comparison function for min/max operations
 *
 * @param config Aggregation configuration
 * @param compare_fn Comparison function
 */
void nlink_aggregation_config_set_compare(nlink_aggregation_config* config, 
                                        int (*compare_fn)(const void*, const void*));

/**
 * @brief Free aggregation configuration
 *
 * @param config Configuration to free
 */
void nlink_aggregation_config_free(nlink_aggregation_config* config);

/**
 * @brief Perform aggregation on an array of items
 *
 * @param items Array of items to aggregate
 * @param count Number of items
 * @param config Aggregation configuration
 * @return Aggregation result structure
 */
nlink_aggregation_result* nlink_aggregate(void** items, size_t count, 
                                         nlink_aggregation_config* config);

/**
 * @brief Free aggregation result
 *
 * @param result Result to free
 */
void nlink_aggregation_result_free(nlink_aggregation_result* result);

/**
 * @brief Group items by a key function
 *
 * @param items Array of items to group
 * @param count Number of items
 * @param key_fn Function to extract key from item
 * @param context Context for key function
 * @param num_groups Pointer to receive number of groups
 * @return Array of groups, each group is NULL-terminated
 */
void*** nlink_group_by(void** items, size_t count, 
                      void* (*key_fn)(void* item, void* context),
                      void* context, size_t* num_groups);

/**
 * @brief Combine multiple arrays into one
 *
 * @param arrays Array of arrays to combine
 * @param array_count Number of arrays
 * @param result_count Pointer to receive total item count
 * @return Combined array
 */
void** nlink_combine_arrays(void*** arrays, size_t array_count, size_t* result_count);

/**
 * @brief Create a numerical summary of values
 *
 * @param values Array of numeric values (must be castable to double)
 * @param count Number of values
 * @return Aggregation result with numeric statistics
 */
nlink_aggregation_result* nlink_numerical_summary(void** values, size_t count);

/**
 * Macro for simple aggregation operations
 */
#define NLINK_SUMMARIZE(items, count, op) \
    nlink_aggregate((void**)(items), (count), \
                   nlink_aggregation_config_create(op))

#endif /* NLINK_TACTIC_AGGREGATION_H */