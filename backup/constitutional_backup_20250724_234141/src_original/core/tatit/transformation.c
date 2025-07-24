/**
 * @file transformation.c
 * @brief Implementation of transformation tactics
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/tactic/transformation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

nlink_transform_config* nlink_transform_config_create(nlink_transformer_fn transform_fn, 
                                                    void* context) {
    nlink_transform_config* config = malloc(sizeof(nlink_transform_config));
    if (config == NULL) {
        return NULL;
    }
    
    // Initialize with defaults
    config->transform_fn = transform_fn;
    config->condition_fn = NULL;
    config->factory_fn = NULL;
    config->context = context;
    config->preserve_original = false;
    config->abort_on_error = true;
    
    return config;
}

void nlink_transform_config_set_condition(nlink_transform_config* config, 
                                         nlink_condition_fn condition_fn) {
    if (config == NULL) {
        return;
    }
    
    config->condition_fn = condition_fn;
}

void nlink_transform_config_set_factory(nlink_transform_config* config, 
                                       nlink_factory_fn factory_fn) {
    if (config == NULL) {
        return;
    }
    
    config->factory_fn = factory_fn;
}

void nlink_transform_config_set_preserve(nlink_transform_config* config, 
                                        bool preserve) {
    if (config == NULL) {
        return;
    }
    
    config->preserve_original = preserve;
}

void nlink_transform_config_set_abort_on_error(nlink_transform_config* config, 
                                              bool abort_on_error) {
    if (config == NULL) {
        return;
    }
    
    config->abort_on_error = abort_on_error;
}

void nlink_transform_config_free(nlink_transform_config* config) {
    free(config);
}

void* nlink_transform(void* data, nlink_transformer_fn transform_fn, void* context) {
    if (data == NULL || transform_fn == NULL) {
        return data;
    }
    
    return transform_fn(data, context);
}

void* nlink_transform_if(void* data, nlink_condition_fn condition_fn, 
                        nlink_transformer_fn transform_fn, void* context) {
    if (data == NULL || condition_fn == NULL || transform_fn == NULL) {
        return data;
    }
    
    if (condition_fn(data, context)) {
        return transform_fn(data, context);
    }
    
    return data;
}

void** nlink_transform_array(void** items, size_t count, 
                            nlink_transform_config* config) {
    if (items == NULL || count == 0 || config == NULL || config->transform_fn == NULL) {
        return items;
    }
    
    // Allocate result array
    void** result = malloc(count * sizeof(void*));
    if (result == NULL) {
        return items;  // Return original on allocation failure
    }
    
    // Transform each item
    for (size_t i = 0; i < count; i++) {
        // Apply condition if provided
        bool should_transform = true;
        if (config->condition_fn != NULL) {
            should_transform = config->condition_fn(items[i], config->context);
        }
        
        if (should_transform) {
            result[i] = config->transform_fn(items[i], config->context);
            
            // Handle transformation error
            if (result[i] == NULL && items[i] != NULL) {
                if (config->abort_on_error) {
                    // Clean up and return original
                    for (size_t j = 0; j < i; j++) {
                        // Free only if we're not preserving originals
                        if (!config->preserve_original && result[j] != items[j]) {
                            free(result[j]);
                        }
                    }
                    free(result);
                    return items;
                } else {
                    // Use original on error
                    result[i] = items[i];
                }
            }
        } else {
            // Use original if condition not met
            result[i] = items[i];
        }
    }
    
    // Clean up originals if not preserving
    if (!config->preserve_original) {
        for (size_t i = 0; i < count; i++) {
            if (result[i] != items[i]) {
                free(items[i]);
            }
        }
    }
    
    return result;
}

void** nlink_transform_multi(void** data, nlink_transformer_fn* transforms, 
                            size_t count, void* context) {
    if (data == NULL || transforms == NULL || count == 0) {
        return data;
    }
    
    // Allocate result array
    void** result = malloc(count * sizeof(void*));
    if (result == NULL) {
        return data;  // Return original on allocation failure
    }
    
    // Transform each item with its corresponding transformation
    for (size_t i = 0; i < count; i++) {
        if (transforms[i] != NULL) {
            result[i] = transforms[i](data[i], context);
            if (result[i] == NULL && data[i] != NULL) {
                // Failed transformation, use original
                result[i] = data[i];
            }
        } else {
            // No transformation, use original
            result[i] = data[i];
        }
    }
    
    return result;
}

double nlink_normalize(double value, double min, double max, 
                      double target_min, double target_max) {
    // Handle invalid inputs
    if (min >= max) {
        return value;
    }
    
    // Default target range is [0, 1]
    if (target_min == target_max) {
        target_min = 0.0;
        target_max = 1.0;
    }
    
    // Calculate normalized value
    double normalized = (value - min) / (max - min);
    
    // Scale to target range
    return target_min + normalized * (target_max - target_min);
}

double* nlink_normalize_array(double* values, size_t count, 
                             double target_min, double target_max) {
    if (values == NULL || count == 0) {
        return NULL;
    }
    
    // Find min and max values
    double min = DBL_MAX;
    double max = -DBL_MAX;
    
    for (size_t i = 0; i < count; i++) {
        if (values[i] < min) {
            min = values[i];
        }
        if (values[i] > max) {
            max = values[i];
        }
    }
    
    // Check if range is valid
    if (min >= max) {
        // All values are the same, return array of mid-point in target range
        double* result = malloc(count * sizeof(double));
        if (result == NULL) {
            return NULL;
        }
        
        double mid = (target_min + target_max) / 2.0;
        for (size_t i = 0; i < count; i++) {
            result[i] = mid;
        }
        
        return result;
    }
    
    // Normal case: normalize each value
    double* result = malloc(count * sizeof(double));
    if (result == NULL) {
        return NULL;
    }
    
    // Default target range is [0, 1]
    if (target_min == target_max) {
        target_min = 0.0;
        target_max = 1.0;
    }
    
    for (size_t i = 0; i < count; i++) {
        result[i] = nlink_normalize(values[i], min, max, target_min, target_max);
    }
    
    return result;
}