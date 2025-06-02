/**
 * @file nexus_result.c
 * @brief Implementation of result monad for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/common/nexus_result.h"
#include <stdlib.h>

nexus_result nexus_success(void* value, NexusContext* context) {
    nexus_result result;
    result.status = NEXUS_STATUS_SUCCESS;
    result.value = value;
    result.context = context;
    return result;
}

nexus_result nexus_exception_result(nexus_exception* exception, NexusContext* context) {
    nexus_result result;
    result.status = NEXUS_STATUS_EXCEPTION;
    result.exception = exception;
    result.context = context;
    
    // Log the exception if context is available
    if (context != NULL) {
        nexus_exception_log(context, exception);
    }
    
    return result;
}

nexus_result nexus_error_result(nexus_error* error, NexusContext* context) {
    nexus_result result;
    result.status = NEXUS_STATUS_ERROR;
    result.error = error;
    result.context = context;
    
    // Log the error if context is available
    if (context != NULL) {
        nexus_error_log(context, error);
    }
    
    return result;
}

void nexus_result_free(nexus_result* result) {
    if (result == NULL) {
        return;
    }
    
    switch (result->status) {
        case NEXUS_STATUS_SUCCESS:
            // The value is not owned by the result, so don't free it
            break;
            
        case NEXUS_STATUS_EXCEPTION:
            nexus_exception_free(result->exception);
            break;
            
        case NEXUS_STATUS_ERROR:
            nexus_error_free(result->error);
            break;
    }
    
    // Clear the result
    result->status = NEXUS_STATUS_SUCCESS;
    result->value = NULL;
}

nexus_result nexus_result_exception_to_error(nexus_result result) {
    if (result.status != NEXUS_STATUS_EXCEPTION) {
        return result;
    }
    
    // Convert exception to error and create new result
    nexus_error* error = nexus_exception_to_error(result.exception);
    return nexus_error_result(error, result.context);
}

void nexus_result_log(const nexus_result* result) {
    if (result == NULL) {
        return;
    }
    
    switch (result->status) {
        case NEXUS_STATUS_SUCCESS:
            // Nothing to log for success
            break;
            
        case NEXUS_STATUS_EXCEPTION:
            nexus_exception_log(result->context, result->exception);
            break;
            
        case NEXUS_STATUS_ERROR:
            nexus_error_log(result->context, result->error);
            break;
    }
}

