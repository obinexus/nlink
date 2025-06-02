/**
 * @file abstraction.h
 * @brief Abstraction tactics for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module implements the abstraction tactical pattern,
 * allowing operations to be generalized across different types and contexts.
 */

#ifndef NLINK_TACTIC_ABSTRACTION_H
#define NLINK_TACTIC_ABSTRACTION_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Function pointer types for abstract operations
 */
typedef void* (*nlink_map_fn)(void* item, void* context);
typedef bool (*nlink_filter_fn)(void* item, void* context);
typedef void* (*nlink_fold_fn)(void* accumulator, void* item, void* context);
typedef int (*nlink_compare_fn)(void* a, void* b, void* context);

/**
 * @brief Map a function over an array of items
 *
 * Applies the mapping function to each item in the array and
 * returns a new array with the results.
 *
 * @param items Array of input items
 * @param count Number of items
 * @param map_func Function to apply to each item
 * @param context Optional context passed to the mapping function
 * @param result_size Pointer to receive the size of the result array
 * @return Newly allocated array with mapped results, or NULL on failure
 */
void** nlink_map(void** items, size_t count, nlink_map_fn map_func, 
                void* context, size_t* result_size);

/**
 * @brief Filter an array using a predicate function
 *
 * Creates a new array containing only items for which the predicate returns true.
 *
 * @param items Array of input items
 * @param count Number of items
 * @param filter_func Predicate function
 * @param context Optional context passed to the predicate
 * @param result_size Pointer to receive the size of the result array
 * @return Newly allocated array with filtered results, or NULL on failure
 */
void** nlink_filter(void** items, size_t count, nlink_filter_fn filter_func,
                   void* context, size_t* result_size);

/**
 * @brief Fold (reduce) an array to a single value
 *
 * Iteratively combines elements of the array using the fold function.
 *
 * @param items Array of input items
 * @param count Number of items
 * @param initial Initial accumulator value
 * @param fold_func Function to combine accumulator with each item
 * @param context Optional context passed to the fold function
 * @return Final accumulator value after processing all items
 */
void* nlink_fold(void** items, size_t count, void* initial, 
                nlink_fold_fn fold_func, void* context);

/**
 * @brief Apply a function to each item in an array (side effects)
 *
 * @param items Array of input items
 * @param count Number of items
 * @param func Function to apply to each item
 * @param context Optional context passed to the function
 */
void nlink_foreach(void** items, size_t count, nlink_map_fn func, void* context);

/**
 * @brief Find the first item matching a predicate
 *
 * @param items Array of input items
 * @param count Number of items
 * @param predicate Predicate function for matching
 * @param context Optional context passed to the predicate
 * @return First matching item, or NULL if none found
 */
void* nlink_find(void** items, size_t count, nlink_filter_fn predicate, void* context);

/**
 * @brief Find the index of the first item matching a predicate
 *
 * @param items Array of input items
 * @param count Number of items
 * @param predicate Predicate function for matching
 * @param context Optional context passed to the predicate
 * @return Index of first matching item, or -1 if none found
 */
int nlink_find_index(void** items, size_t count, nlink_filter_fn predicate, void* context);

/**
 * @brief Sort an array using a comparison function
 *
 * @param items Array of items to sort (modified in place)
 * @param count Number of items
 * @param compare Comparison function
 * @param context Optional context passed to the comparison function
 */
void nlink_sort(void** items, size_t count, nlink_compare_fn compare, void* context);

/**
 * @brief Check if any item in the array matches a predicate
 *
 * @param items Array of input items
 * @param count Number of items
 * @param predicate Predicate function
 * @param context Optional context passed to the predicate
 * @return true if any item matches the predicate, false otherwise
 */
bool nlink_any(void** items, size_t count, nlink_filter_fn predicate, void* context);

/**
 * @brief Check if all items in the array match a predicate
 *
 * @param items Array of input items
 * @param count Number of items
 * @param predicate Predicate function
 * @param context Optional context passed to the predicate
 * @return true if all items match the predicate, false otherwise
 */
bool nlink_all(void** items, size_t count, nlink_filter_fn predicate, void* context);

/**
 * @brief Create a new array with the same items
 * 
 * @param items Array to clone
 * @param count Number of items
 * @return New array with the same items
 */
void** nlink_clone_array(void** items, size_t count);

/**
 * Abstraction macros for common operations
 */
#define NLINK_MAP(items, count, func, ctx) \
    nlink_map((void**)(items), (count), (nlink_map_fn)(func), (ctx), NULL)

#define NLINK_FILTER(items, count, predicate, ctx) \
    nlink_filter((void**)(items), (count), (nlink_filter_fn)(predicate), (ctx), NULL)

#define NLINK_FOLD(items, count, initial, func, ctx) \
    nlink_fold((void**)(items), (count), (initial), (nlink_fold_fn)(func), (ctx))

#endif /* NLINK_TACTIC_ABSTRACTION_H */