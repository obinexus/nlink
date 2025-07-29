/**
 * @file nexus_error.h
 * @brief Error handling for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NEXUS_ERROR_H
#define NEXUS_ERROR_H

#include "nlink/core/common/types.h"

/**
 * Error structure for unrecoverable errors
 */
typedef struct nexus_error {
    NexusErrorCode code;       // Error code
    char* message;             // Error message
    char* file;                // Source file where error occurred
    int line;                  // Line number where error occurred
    struct nexus_error* cause; // Chain of causality (optional)
} nexus_error;

/**
 * Create a new error
 * @param code Error code
 * @param message Error message (will be copied)
 * @param file Source file
 * @param line Line number
 * @return New error object
 */
nexus_error* nexus_error_create(NexusErrorCode code, const char* message, 
                                const char* file, int line);

/**
 * Create a new error with cause
 * @param code Error code
 * @param message Error message (will be copied)
 * @param file Source file
 * @param line Line number
 * @param cause Causal error (ownership transferred)
 * @return New error object
 */
nexus_error* nexus_error_create_with_cause(NexusErrorCode code, const char* message, 
                                          const char* file, int line, 
                                          nexus_error* cause);

/**
 * Free an error and its resources
 * @param error Error to free
 */
void nexus_error_free(nexus_error* error);

/**
 * Convert an error to a string representation
 * @param error Error to convert
 * @param buffer Buffer to write to
 * @param size Buffer size
 * @return Number of bytes written (excluding null terminator)
 */
size_t nexus_error_to_string(const nexus_error* error, char* buffer, size_t size);

/**
 * Log an error using the current context's logging system
 * @param ctx Context for logging
 * @param error Error to log
 */
void nexus_error_log(NexusContext* ctx, const nexus_error* error);

/**
 * Convenience macro for creating errors with location info
 */
#define NEXUS_ERROR_CREATE(code, message) \
    nexus_error_create((code), (message), __FILE__, __LINE__)

/**
 * Convenience macro for creating errors with cause and location info
 */
#define NEXUS_ERROR_CREATE_WITH_CAUSE(code, message, cause) \
    nexus_error_create_with_cause((code), (message), __FILE__, __LINE__, (cause))

#endif /* NEXUS_ERROR_H */