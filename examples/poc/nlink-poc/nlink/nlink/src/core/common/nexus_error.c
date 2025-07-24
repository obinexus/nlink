/**
 * @file nexus_error.c
 * @brief Implementation of error handling for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/common/nexus_error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

nexus_error* nexus_error_create(NexusErrorCode code, const char* message,
                               const char* file, int line) {
    nexus_error* error = (nexus_error*)malloc(sizeof(nexus_error));
    if (error == NULL) {
        return NULL;
    }
    
    error->code = code;
    error->message = message ? strdup(message) : NULL;
    error->file = file ? strdup(file) : NULL;
    error->line = line;
    error->cause = NULL;
    
    return error;
}

nexus_error* nexus_error_create_with_cause(NexusErrorCode code, const char* message,
                                         const char* file, int line,
                                         nexus_error* cause) {
    nexus_error* error = nexus_error_create(code, message, file, line);
    if (error == NULL) {
        return NULL;
    }
    
    error->cause = cause; // Transfer ownership of cause
    
    return error;
}

void nexus_error_free(nexus_error* error) {
    if (error == NULL) {
        return;
    }
    
    free(error->message);
    free(error->file);
    
    // Free cause chain
    if (error->cause != NULL) {
        nexus_error_free(error->cause);
    }
    
    free(error);
}

size_t nexus_error_to_string(const nexus_error* error, char* buffer, size_t size) {
    if (error == NULL || buffer == NULL || size == 0) {
        return 0;
    }
    
    const char* error_str = "Unknown";
    
    // Convert error code to string
    switch (error->code) {
        case NEXUS_ERROR_INVALID_ARGUMENT:
            error_str = "Invalid Argument";
            break;
        case NEXUS_ERROR_OUT_OF_MEMORY:
            error_str = "Out of Memory";
            break;
        case NEXUS_ERROR_INVALID_STATE:
            error_str = "Invalid State";
            break;
        case NEXUS_ERROR_NOT_IMPLEMENTED:
            error_str = "Not Implemented";
            break;
        // Add more error codes as needed
    }
    
    // Format error message
    int written = snprintf(buffer, size, "Error [%s]: %s (%s:%d)",
                          error_str,
                          error->message ? error->message : "No message",
                          error->file ? error->file : "unknown",
                          error->line);
    
    // Add cause if present and space permits
    if (error->cause != NULL && written < (int)size) {
        int cause_written = snprintf(buffer + written,
                                   size - written,
                                   "\nCaused by: ");
        
        if (cause_written > 0 && (written + cause_written) < (int)size) {
            written += cause_written;
            written += nexus_error_to_string(error->cause,
                                           buffer + written,
                                           size - written);
        }
    }
    
    return (written < 0) ? 0 : (size_t)written;
}

void nexus_error_log(NexusContext* ctx, const nexus_error* error) {
    // Forward declaration to avoid circular dependency with core module
    extern void nexus_log(NexusContext* ctx, NexusLogLevel level, const char* format, ...);
    
    if (error == NULL) {
        return;
    }
    
    // Buffer for error message
    char buffer[1024];
    nexus_error_to_string(error, buffer, sizeof(buffer));
    
    if (ctx != NULL) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "%s", buffer);
    } else {
        // Fallback if no context is available
        fprintf(stderr, "[ERROR] %s\n", buffer);
    }
}