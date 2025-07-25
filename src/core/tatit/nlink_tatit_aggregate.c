/**
 * @file aggregation.c
 * @brief Implementation of aggregation tactics
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <nlink/core/tactic/aggregation.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
// Default comparison function for generic pointers
static int default_compare(const void* a, const void* b) {
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

// Numeric casting helper
static double to_double(void* value) {
    // This is a simplified implementation, in real code we'd need
    // proper type information and conversion
    return *((double*)value);
}

nlink_aggregation_config* nlink_aggregation_config_create(nlink_aggregation_op operation) {
    nlink_aggregation_config* config = malloc(sizeof(nlink_aggregation_config));
    if (config == NULL) {
        return NULL;
    }
    
    // Initialize with defaults
    config->operation = operation;
    config->custom_fn = NULL;
    config->combiner_fn = NULL;
    config->initializer_fn = NULL;
    config->compare_fn = default_compare;
    config->context = NULL;
    
    return config;
}

void nlink_aggregation_config_set_custom(nlink_aggregation_config* config, 
                                        nlink_aggregator_fn custom_fn, 
                                        void* context) {
    if (config == NULL) {
        return;
    }
    
    config->operation = NLINK_AGGREGATION_CUSTOM;
    config->custom_fn = custom_fn;
    config->context = context;
}

void nlink_aggregation_config_set_combiner(nlink_aggregation_config* config, 
                                          nlink_combiner_fn combiner_fn) {
    if (config == NULL) {
        return;
    }
    
    config->combiner_fn = combiner_fn;
}

void nlink_aggregation_config_set_initializer(nlink_aggregation_config* config, 
                                            nlink_initializer_fn initializer_fn) {
    if (config == NULL) {
        return;
    }
    
    config->initializer_fn = initializer_fn;
}

void nlink_aggregation_config_set_compare(nlink_aggregation_config* config, 
                                        int (*compare_fn)(const void*, const void*)) {
    if (config == NULL || compare_fn == NULL) {
        return;
    }
    
    config->compare_fn = compare_fn;
}

void nlink_aggregation_config_free(nlink_aggregation_config* config) {
    free(config);
}

static void* find_min(void** items, size_t count, int (*compare)(const void*, const void*)) {
    if (count == 0 || items == NULL) {
        return NULL;
    }
    
    void* min = items[0];
    for (size_t i = 1; i < count; i++) {
        if (compare(items[i], min) < 0) {
            min = items[i];
        }
    }
    
    return min;
}

static void* find_max(void** items, size_t count, int (*compare)(const void*, const void*)) {
    if (count == 0 || items == NULL) {
        return NULL;
    }
    
    void* max = items[0];
    for (size_t i = 1; i < count; i++) {
        if (compare(items[i], max) > 0) {
            max = items[i];
        }
    }
    
    return max;
}

static double compute_numeric_sum(void** items, size_t count) {
    if (count == 0 || items == NULL) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += to_double(items[i]);
    }
    
    return sum;
}

nlink_aggregation_result* nlink_aggregate(void** items, size_t count, 
                                         nlink_aggregation_config* config) {
    if (items == NULL || count == 0 || config == NULL) {
        return NULL;
    }
    
    nlink_aggregation_result* result = malloc(sizeof(nlink_aggregation_result));
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize result
    memset(result, 0, sizeof(nlink_aggregation_result));
    result->count = count;
    
    // Perform aggregation based on operation
    switch (config->operation) {
        case NLINK_AGGREGATION_CUSTOM:
            if (config->custom_fn != NULL) {
                result->custom = config->custom_fn(items, count, config->context);
            }
            break;
            
        case NLINK_AGGREGATION_MIN:
            result->min = find_min(items, count, config->compare_fn);
            break;
            
        case NLINK_AGGREGATION_MAX:
            result->max = find_max(items, count, config->compare_fn);
            break;
            
        case NLINK_AGGREGATION_SUM:
            // Attempt numeric sum
            result->numeric_sum = compute_numeric_sum(items, count);
            
            // Attempt custom sum if combiner provided
            if (config->combiner_fn != NULL && config->initializer_fn != NULL) {
                void* sum = config->initializer_fn(config->context);
                for (size_t i = 0; i < count; i++) {
                    sum = config->combiner_fn(sum, items[i], config->context);
                }
                result->sum = sum;
            }
            break;
            
        case NLINK_AGGREGATION_AVERAGE:
            // Calculate numeric average
            result->numeric_sum = compute_numeric_sum(items, count);
            result->numeric_avg = result->numeric_sum / count;
            break;
            
        case NLINK_AGGREGATION_COUNT:
            // Count is already set
            break;
            
        case NLINK_AGGREGATION_GROUP:
            // Not implemented in this function
            break;
            
        case NLINK_AGGREGATION_CONCAT:
            // Not implemented in this function
            break;
    }
    
    return result;
}

void nlink_aggregation_result_free(nlink_aggregation_result* result) {
    if (result == NULL) {
        return;
    }
    
    // Note: We don't free the contained pointers because they
    // might be references to original data. In a real implementation,
    // we would need more information about ownership.
    
    free(result);
}

// Hash table entry for group_by
typedef struct {
    void* key;
    void** items;
    size_t count;
    size_t capacity;
} group_entry;

// Simple hash function for pointers
static size_t hash_pointer(void* ptr) {
    return (size_t)ptr % 1024;
}

void*** nlink_group_by(void** items, size_t count, 
                      void* (*key_fn)(void* item, void* context),
                      void* context, size_t* num_groups) {
    if (items == NULL || count == 0 || key_fn == NULL || num_groups == NULL) {
        *num_groups = 0;
        return NULL;
    }
    
    // Simplified implementation using a basic hash table
    // In a real implementation, we would use a proper hash table
    
    group_entry groups[1024] = {0};  // Fixed size for simplicity
    size_t group_count = 0;
    
    // Group items by key
    for (size_t i = 0; i < count; i++) {
        void* key = key_fn(items[i], context);
        size_t hash = hash_pointer(key);
        
        // Find or create group
        if (groups[hash].key == NULL) {
            // New group
            groups[hash].key = key;
            groups[hash].capacity = 16;  // Initial capacity
            groups[hash].items = malloc(groups[hash].capacity * sizeof(void*));
            if (groups[hash].items == NULL) {
                // Cleanup and fail
                for (size_t j = 0; j < 1024; j++) {
                    free(groups[j].items);
                }
                *num_groups = 0;
                return NULL;
            }
            group_count++;
        } else if (groups[hash].key != key) {
            // Hash collision - for simplicity we'll just fail
            // In a real implementation, we would handle this properly
            for (size_t j = 0; j < 1024; j++) {
                free(groups[j].items);
            }
            *num_groups = 0;
            return NULL;
        }
        
        // Add item to group
        if (groups[hash].count >= groups[hash].capacity) {
            // Resize group
            groups[hash].capacity *= 2;
            void** new_items = realloc(groups[hash].items, 
                                     groups[hash].capacity * sizeof(void*));
            if (new_items == NULL) {
                // Cleanup and fail
                for (size_t j = 0; j < 1024; j++) {
                    free(groups[j].items);
                }
                *num_groups = 0;
                return NULL;
            }
            groups[hash].items = new_items;
        }
        
        groups[hash].items[groups[hash].count++] = items[i];
    }
    
    // Create result array
    void*** result = malloc((group_count + 1) * sizeof(void**));
    if (result == NULL) {
        // Cleanup and fail
        for (size_t j = 0; j < 1024; j++) {
            free(groups[j].items);
        }
        *num_groups = 0;
        return NULL;
    }
    
    // Fill result array
    size_t result_index = 0;
    for (size_t i = 0; i < 1024; i++) {
        if (groups[i].key != NULL) {
            // Create NULL-terminated array for this group
            void** group = malloc((groups[i].count + 1) * sizeof(void*));
            if (group == NULL) {
                // Cleanup and fail
                for (size_t j = 0; j < result_index; j++) {
                    free(result[j]);
                }
                for (size_t j = 0; j < 1024; j++) {
                    free(groups[j].items);
                }
                free(result);
                *num_groups = 0;
                return NULL;
            }
            
            // Copy items
            memcpy(group, groups[i].items, groups[i].count * sizeof(void*));
            group[groups[i].count] = NULL;  // NULL-terminate
            
            result[result_index++] = group;
            
            // Free original items array
            free(groups[i].items);
        }
    }
    
    // NULL-terminate result array
    result[result_index] = NULL;
    
    *num_groups = group_count;
    return result;
}

void** nlink_combine_arrays(void*** arrays, size_t array_count, size_t* result_count) {
    if (arrays == NULL || array_count == 0 || result_count == NULL) {
        *result_count = 0;
        return NULL;
    }
    
    // Count total number of items
    size_t total_count = 0;
    for (size_t i = 0; i < array_count; i++) {
        if (arrays[i] == NULL) {
            continue;
        }
        
        for (size_t j = 0; arrays[i][j] != NULL; j++) {
            total_count++;
        }
    }
    
    if (total_count == 0) {
        *result_count = 0;
        return NULL;
    }
    
    // Allocate result array
    void** result = malloc((total_count + 1) * sizeof(void*));
    if (result == NULL) {
        *result_count = 0;
        return NULL;
    }
    
    // Copy items
    size_t result_index = 0;
    for (size_t i = 0; i < array_count; i++) {
        if (arrays[i] == NULL) {
            continue;
        }
        
        for (size_t j = 0; arrays[i][j] != NULL; j++) {
            result[result_index++] = arrays[i][j];
        }
    }
    
    // NULL-terminate
    result[result_index] = NULL;
    
    *result_count = total_count;
    return result;
}

nlink_aggregation_result* nlink_numerical_summary(void** values, size_t count) {
    if (values == NULL || count == 0) {
        return NULL;
    }
    
    nlink_aggregation_result* result = malloc(sizeof(nlink_aggregation_result));
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize result
    memset(result, 0, sizeof(nlink_aggregation_result));
    result->count = count;
    
    // Calculate numeric statistics
    double min_val = to_double(values[0]);
    double max_val = min_val;
    double sum = min_val;
    
    for (size_t i = 1; i < count; i++) {
        double val = to_double(values[i]);
        
        if (val < min_val) {
            min_val = val;
        }
        
        if (val > max_val) {
            max_val = val;
        }
        
        sum += val;
    }
    
    // Allocate and set results
    double* min = malloc(sizeof(double));
    double* max = malloc(sizeof(double));
    double* avg = malloc(sizeof(double));
    
    if (min == NULL || max == NULL || avg == NULL) {
        free(min);
        free(max);
        free(avg);
        free(result);
        return NULL;
    }
    
    *min = min_val;
    *max = max_val;
    *avg = sum / count;
    
    result->min = min;
    result->max = max;
    result->average = avg;
    result->numeric_sum = sum;
    result->numeric_avg = sum / count;
    
    return result;
}