/**
 * @file tactic.h
 * @brief Core tactical programming paradigm implementations
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides implementations of core tactical programming patterns:
 * - Composition: Combining smaller tactics into complex operations
 * - Abstraction: Defining general tactics applicable across contexts
 * - Aggregation: Grouping tactics/data for collective operations
 * - Transformation: Applying change operations to structures
 * - Traversal: Systematically navigating through data structures
 * - Identity: Providing baseline operations that maintain state
 */

#ifndef NLINK_TACTIC_H
#define NLINK_TACTIC_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Function pointer type for transformation tactics
 * @param data The data to transform
 * @param context Additional context for the transformation
 * @return Transformed data
 */
typedef void* (*nlink_transform_fn)(void* data, void* context);

/**
 * Function pointer type for predicate tactics
 * @param data The data to test
 * @param context Additional context for the test
 * @return Boolean result of the test
 */
typedef bool (*nlink_predicate_fn)(void* data, void* context);

/**
 * Function pointer type for consumer tactics
 * @param data The data to consume
 * @param context Additional context for the consumption
 */
typedef void (*nlink_consumer_fn)(void* data, void* context);

/* Core tactical functions */

/**
 * Composition tactic - combines multiple transforms into one
 */
void* nlink_compose(void* data, nlink_transform_fn* transforms, int count, void* context);

/**
 * Identity tactic - returns the input unchanged
 */
void* nlink_identity(void* data, void* context);

/**
 * Aggregation tactic - applies an operation to a collection
 */
void* nlink_aggregate(void** items, size_t count, nlink_transform_fn operation, void* context);

/**
 * Traversal tactic - systematically visits and transforms elements
 */
void* nlink_traverse(void* structure, nlink_transform_fn visit, void* context);

/**
 * Conditional tactic - applies a transform based on a predicate
 */
void* nlink_when(void* data, nlink_predicate_fn condition, nlink_transform_fn transform, void* context);

/**
 * Iteration tactic - applies a transform repeatedly until a condition is met
 */
void* nlink_iterate(void* data, nlink_predicate_fn condition, nlink_transform_fn transform, void* context);

#endif /* NLINK_TACTIC_H */
