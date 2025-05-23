/**
 * @file command_params.c
 * @brief Implementation of command parameter handling for NexusLink CLI
 * 
 * Provides implementation for extracting, storing, and accessing parameters
 * from command line inputs using pattern matching.
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nlink/cli/command_params.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create a new command parameter list
 * 
 * @return NlinkCommandParams* New parameter list or NULL on failure
 */
NlinkCommandParams* nlink_command_params_create(void) {
    NlinkCommandParams* params = (NlinkCommandParams*)malloc(sizeof(NlinkCommandParams));
    if (!params) {
        return NULL;
    }
    
    // Initialize params
    params->count = 0;
    params->first = NULL;
    
    return params;
}

/**
 * @brief Add a parameter to the list
 * 
 * @param params Parameter list
 * @param name Parameter name
 * @param value Parameter value (can be NULL)
 * @return NexusResult Result code
 */
NexusResult nlink_command_params_add(NlinkCommandParams* params, 
                                   const char* name, 
                                   const char* value) {
    if (!params || !name) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Create a new parameter
    NlinkCommandParam* param = (NlinkCommandParam*)malloc(sizeof(NlinkCommandParam));
    if (!param) {
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Initialize parameter
    param->name = strdup(name);
    if (!param->name) {
        free(param);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    if (value) {
        param->value = strdup(value);
        if (!param->value) {
            free(param->name);
            free(param);
            return NEXUS_OUT_OF_MEMORY;
        }
    } else {
        param->value = NULL;
    }
    
    // Add to the linked list (prepend for simplicity)
    param->next = params->first;
    params->first = param;
    params->count++;
    
    return NEXUS_SUCCESS;
}

/**
 * @brief Get a parameter value by name
 * 
 * @param params Parameter list
 * @param name Parameter name
 * @return const char* Parameter value or NULL if not found
 */
const char* nlink_command_params_get(const NlinkCommandParams* params, const char* name) {
    if (!params || !name) {
        return NULL;
    }
    
    // Search through parameters
    NlinkCommandParam* param = params->first;
    while (param) {
        if (strcmp(param->name, name) == 0) {
            return param->value;
        }
        
        param = param->next;
    }
    
    return NULL;
}

/**
 * @brief Check if a parameter exists
 * 
 * @param params Parameter list
 * @param name Parameter name
 * @return bool True if parameter exists, false otherwise
 */
bool nlink_command_params_has(const NlinkCommandParams* params, const char* name) {
    return nlink_command_params_get(params, name) != NULL;
}

/**
 * @brief Get the number of parameters
 * 
 * @param params Parameter list
 * @return size_t Number of parameters
 */
size_t nlink_command_params_count(const NlinkCommandParams* params) {
    if (!params) {
        return 0;
    }
    
    return params->count;
}

/**
 * @brief Get parameter at index
 * 
 * @param params Parameter list
 * @param index Parameter index
 * @param name Pointer to store parameter name
 * @param value Pointer to store parameter value (can be NULL)
 * @return bool True if parameter found, false otherwise
 */
bool nlink_command_params_get_at(const NlinkCommandParams* params, 
                               size_t index, 
                               const char** name, 
                               const char** value) {
    if (!params || index >= params->count || !name) {
        return false;
    }
    
    // Find the parameter at the given index
    NlinkCommandParam* param = params->first;
    for (size_t i = 0; i < index && param; i++) {
        param = param->next;
    }
    
    if (!param) {
        return false;
    }
    
    // Return name and value
    *name = param->name;
    if (value) {
        *value = param->value;
    }
    
    return true;
}

/**
 * @brief Free command parameter list resources
 * 
 * @param params Parameter list to free
 */
void nlink_command_params_destroy(NlinkCommandParams* params) {
    if (!params) {
        return;
    }
    
    // Free all parameters
    NlinkCommandParam* param = params->first;
    while (param) {
        NlinkCommandParam* next = param->next;
        
        free(param->name);
        free(param->value);
        free(param);
        
        param = next;
    }
    
    // Free the list itself
    free(params);
}