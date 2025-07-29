/**
 * @file abstraction.c
 * @brief Implementation of abstraction tactics
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <core/tactic/abstraction.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void** nlink_map(void** items, size_t count, nlink_map_fn map_func, 
                void* context, size_t* result_size) {
    if (items == NULL || map_func == NULL || count == 0) {
        if (result_size != NULL) {
            *result_size = 0;
        }
        return NULL;
    }
    
    // Allocate result array
    void** result = malloc(count * sizeof(void*));
    if (result == NULL) {
        if (result_size != NULL) {
            *result_size = 0;
        }
        return NULL;
    }
    
    // Apply mapping function to each item
    for (size_t i = 0; i < count; i++) {
        result[i] = map_func(items[i], context);
        
        // If mapping failed for any item, clean up and fail
        if (result[i] == NULL && items[i] != NULL) {
            // Free previously mapped items
            for (size_t j = 0; j < i; j++) {
                // In a real implementation, we would use a cleanup function here
                // However, we don't have that information, so we just leave it
            }
            free(result);
            
            if (result_size != NULL) {
                *result_size = 0;
            }
            return NULL;
        }
    }
    
    if (result_size != NULL) {
        *result_size = count;
    }
    return result;
}

void** nlink_filter(void** items, size_t count, nlink_filter_fn filter_func,
                   void* context, size_t* result_size) {
    if (items == NULL || filter_func == NULL || count == 0) {
        if (result_size != NULL) {
            *result_size = 0;
        }
        return NULL;
    }
    
    // First pass: count matching items
    size_t match_count = 0;
    for (size_t i = 0; i < count; i++) {
        if (filter_func(items[i], context)) {
            match_count++;
        }
    }
    
    if (match_count == 0) {
        if (result_size != NULL) {
            *result_size = 0;
        }
        return NULL;  // No matches
    }
    
    // Allocate result array for matching items
    void** result = malloc(match_count * sizeof(void*));
    if (result == NULL) {
        if (result_size != NULL) {
            *result_size = 0;
        }
        return NULL;
    }
    
    // Second pass: collect matching items
    size_t result_index = 0;
    for (size_t i = 0; i < count; i++) {
        if (filter_func(items[i], context)) {
            result[result_index++] = items[i];
        }
    }
    
    if (result_size != NULL) {
        *result_size = match_count;
    }
    return result;
}

void* nlink_fold(void** items, size_t count, void* initial, 
                nlink_fold_fn fold_func, void* context) {
    if (items == NULL || fold_func == NULL || count == 0) {
        return initial;  // Nothing to fold, return initial value
    }
    
    void* accumulator = initial;
    
    for (size_t i = 0; i < count; i++) {
        accumulator = fold_func(accumulator, items[i], context);
    }
    
    return accumulator;
}

void nlink_foreach(void** items, size_t count, nlink_map_fn func, void* context) {
    if (items == NULL || func == NULL || count == 0) {
        return;  // Nothing to do
    }
    
    for (size_t i = 0; i < count; i++) {
        func(items[i], context);
    }
}

void* nlink_find(void** items, size_t count, nlink_filter_fn predicate, void* context) {
    if (items == NULL || predicate == NULL || count == 0) {
        return NULL;  // Nothing to find
    }
    
    for (size_t i = 0; i < count; i++) {
        if (predicate(items[i], context)) {
            return items[i];  // Return first matching item
        }
    }
    
    return NULL;  // No match found
}

int nlink_find_index(void** items, size_t count, nlink_filter_fn predicate, void* context) {
    if (items == NULL || predicate == NULL || count == 0) {
        return -1;  // Nothing to find
    }
    
    for (size_t i = 0; i < count; i++) {
        if (predicate(items[i], context)) {
            return (int)i;  // Return index of first matching item
        }
    }
    
    return -1;  // No match found
}

// Helper function for qsort_r compatibility
struct sort_context {
    nlink_compare_fn compare;
    void* user_context;
};

static int sort_compare_wrapper(const void* a, const void* b, void* ctx) {
    struct sort_context* sort_ctx = (struct sort_context*)ctx;
    return sort_ctx->compare(*(void**)a, *(void**)b, sort_ctx->user_context);
}

void nlink_sort(void** items, size_t count, nlink_compare_fn compare, void* context) {
    if (items == NULL || compare == NULL || count <= 1) {
        return;  // Nothing to sort
    }
    
    struct sort_context sort_ctx = {
        .compare = compare,
        .user_context = context
    };
    
    qsort_r(items, count, sizeof(void*), sort_compare_wrapper, &sort_ctx);
}

bool nlink_any(void** items, size_t count, nlink_filter_fn predicate, void* context) {
    if (items == NULL || predicate == NULL || count == 0) {
        return false;  // Nothing to check
    }
    
    for (size_t i = 0; i < count; i++) {
        if (predicate(items[i], context)) {
            return true;  // At least one item matches
        }
    }
    
    return false;  // No match found
}

bool nlink_all(void** items, size_t count, nlink_filter_fn predicate, void* context) {
    if (items == NULL || predicate == NULL || count == 0) {
        return false;  // Nothing to check
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!predicate(items[i], context)) {
            return false;  // At least one item doesn't match
        }
    }
    
    return true;  // All items match
}

void** nlink_clone_array(void** items, size_t count) {
    if (items == NULL || count == 0) {
        return NULL;  // Nothing to clone
    }
    
    void** result = malloc(count * sizeof(void*));
    if (result == NULL) {
        return NULL;
    }
    
    // Copy item pointers
    memcpy(result, items, count * sizeof(void*));
    
    return result;
}