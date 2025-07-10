/**
 * @file nexus_exception.h
 * @brief Exception handling for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NEXUS_EXCEPTION_H
#define NEXUS_EXCEPTION_H

#include "nlink/core/common/nexus_error.h"

/**
 * Exception structure for recoverable errors
 */
typedef struct nexus_exception {
    NexusErrorCode code;        // Exception code
    char* message;              // Exception message
    char* file;                 // Source file where exception occurred
    int line;                   // Line number where exception occurred
    char* stacktrace;           // Optional stack trace (if available)
    bool recoverable;           // Whether this exception is recoverable
    struct nexus_exception* cause; // Chain of causality (optional)
} nexus_exception;

/**
 * Create a new exception
 * @param code Exception code
 * @param message Exception message (will be copied)
 * @param file Source file
 * @param line Line number
 * @param recoverable Whether this exception is recoverable
 * @return New exception object
 */
nexus_exception* nexus_exception_create(NexusErrorCode code, const char* message, 
                                      const char* file, int line, 
                                      bool recoverable);

/**
 * Create a new exception with cause
 * @param code Exception code
 * @param message Exception message (will be copied)
 * @param file Source file
 * @param line Line number
 * @param recoverable Whether this exception is recoverable
 * @param cause Causal exception (ownership transferred)
 * @return New exception object
 */
nexus_exception* nexus_exception_create_with_cause(NexusErrorCode code, 
                                                 const char* message, 
                                                 const char* file, 
                                                 int line, 
                                                 bool recoverable,
                                                 nexus_exception* cause);

/**
 * Free an exception and its resources
 * @param exception Exception to free
 */
void nexus_exception_free(nexus_exception* exception);

/**
 * Convert an exception to an error (for unhandled exceptions)
 * @param exception Exception to convert (will be freed)
 * @return New error object
 */
nexus_error* nexus_exception_to_error(nexus_exception* exception);

/**
 * Convert an exception to a string representation
 * @param exception Exception to convert
 * @param buffer Buffer to write to
 * @param size Buffer size
 * @return Number of bytes written (excluding null terminator)
 */
size_t nexus_exception_to_string(const nexus_exception* exception, 
                                char* buffer, size_t size);

/**
 * Log an exception using the current context's logging system
 * @param ctx Context for logging
 * @param exception Exception to log
 */
void nexus_exception_log(NexusContext* ctx, const nexus_exception* exception);

/**
 * Convenience macro for creating exceptions with location info
 */
#define NEXUS_EXCEPTION_CREATE(code, message, recoverable) \
    nexus_exception_create((code), (message), __FILE__, __LINE__, (recoverable))

/**
 * Convenience macro for creating exceptions with cause and location info
 */
#define NEXUS_EXCEPTION_CREATE_WITH_CAUSE(code, message, recoverable, cause) \
    nexus_exception_create_with_cause((code), (message), __FILE__, __LINE__, \
                                     (recoverable), (cause))

#endif /* NEXUS_EXCEPTION_H */