/**
 * @file nexus_exception.c
 * @brief Implementation of exception handling for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/common/nexus_exception.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Utility function to capture stack trace (platform-specific)
static char* capture_stacktrace(void) {
    // Simplified implementation - in a real system, this would
    // use platform-specific methods to capture a stack trace
    return strdup("Stack trace not available");
}

nexus_exception* nexus_exception_create(NexusErrorCode code, const char* message,
                                      const char* file, int line,
                                      bool recoverable) {
    nexus_exception* exception = (nexus_exception*)malloc(sizeof(nexus_exception));
    if (exception == NULL) {
        return NULL;
    }
    
    exception->code = code;
    exception->message = message ? strdup(message) : NULL;
    exception->file = file ? strdup(file) : NULL;
    exception->line = line;
    exception->stacktrace = capture_stacktrace();
    exception->recoverable = recoverable;
    exception->cause = NULL;
    
    return exception;
}

nexus_exception* nexus_exception_create_with_cause(NexusErrorCode code,
                                                 const char* message,
                                                 const char* file,
                                                 int line,
                                                 bool recoverable,
                                                 nexus_exception* cause) {
    nexus_exception* exception = nexus_exception_create(code, message, file, line, recoverable);
    if (exception == NULL) {
        return NULL;
    }
    
    exception->cause = cause; // Transfer ownership of cause
    
    return exception;
}

void nexus_exception_free(nexus_exception* exception) {
    if (exception == NULL) {
        return;
    }
    
    free(exception->message);
    free(exception->file);
    free(exception->stacktrace);
    
    // Free cause chain
    if (exception->cause != NULL) {
        nexus_exception_free(exception->cause);
    }
    
    free(exception);
}

nexus_error* nexus_exception_to_error(nexus_exception* exception) {
    if (exception == NULL) {
        return NULL;
    }
    
    // Convert cause chain if it exists
    nexus_error* cause_error = NULL;
    if (exception->cause != NULL) {
        cause_error = nexus_exception_to_error(exception->cause);
    }
    
    // Create new error with converted message
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), 
            "Unhandled exception: %s (recoverable: %s)",
            exception->message ? exception->message : "No message",
            exception->recoverable ? "true" : "false");
    
    nexus_error* error = nexus_error_create_with_cause(
        exception->code,
        buffer,
        exception->file,
        exception->line,
        cause_error);
    
    // Free the exception since ownership is transferred
    free(exception->message);
    free(exception->file);
    free(exception->stacktrace);
    free(exception);
    
    return error;
}

size_t nexus_exception_to_string(const nexus_exception* exception,
                                char* buffer, size_t size) {
    if (exception == NULL || buffer == NULL || size == 0) {
        return 0;
    }
    
    const char* exception_str = "Unknown";
    
    // Convert exception code to string (reuse error codes)
    switch (exception->code) {
        case NEXUS_ERROR_INVALID_ARGUMENT:
            exception_str = "Invalid Argument";
            break;
        case NEXUS_ERROR_OUT_OF_MEMORY:
            exception_str = "Out of Memory";
            break;
        case NEXUS_ERROR_INVALID_STATE:
            exception_str = "Invalid State";
            break;
        case NEXUS_ERROR_NOT_IMPLEMENTED:
            exception_str = "Not Implemented";
            break;
        // Add more error codes as needed
    }
    
    // Format exception message
    int written = snprintf(buffer, size, 
                          "Exception [%s]: %s (%s:%d) [%s]",
                          exception_str,
                          exception->message ? exception->message : "No message",
                          exception->file ? exception->file : "unknown",
                          exception->line,
                          exception->recoverable ? "Recoverable" : "Non-recoverable");
    
    // Add stack trace if available and space permits
    if (exception->stacktrace != NULL && written < (int)size) {
        int trace_written = snprintf(buffer + written,
                                   size - written,
                                   "\nStack trace: %s",
                                   exception->stacktrace);
        
        if (trace_written > 0) {
            written += trace_written;
        }
    }
    
    // Add cause if present and space permits
    if (exception->cause != NULL && written < (int)size) {
        int cause_written = snprintf(buffer + written,
                                   size - written,
                                   "\nCaused by: ");
        
        if (cause_written > 0 && (written + cause_written) < (int)size) {
            written += cause_written;
            written += nexus_exception_to_string(exception->cause,
                                              buffer + written,
                                              size - written);
        }
    }
    
    return (written < 0) ? 0 : (size_t)written;
}

void nexus_exception_log(NexusContext* ctx, const nexus_exception* exception) {
    // Forward declaration to avoid circular dependency with core module
    extern void nexus_log(NexusContext* ctx, NexusLogLevel level, const char* format, ...);
    
    if (exception == NULL) {
        return;
    }
    
    // Buffer for exception message
    char buffer[1024];
    nexus_exception_to_string(exception, buffer, sizeof(buffer));
    
    // Log as warning if recoverable, error if not
    NexusLogLevel level = exception->recoverable ? NEXUS_LOG_WARNING : NEXUS_LOG_ERROR;
    
    if (ctx != NULL) {
        nexus_log(ctx, level, "%s", buffer);
    } else {
        // Fallback if no context is available
        fprintf(stderr, "[%s] %s\n", 
               level == NEXUS_LOG_WARNING ? "WARNING" : "ERROR", 
               buffer);
    }
}