/**
 * @file nexus_result.h
 * @brief Result monad for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NEXUS_RESULT_H
#define NEXUS_RESULT_H

#include "nlink/core/common/nexus_error.h"
#include "nlink/core/common/nexus_exception.h"

/**
 * Result status enumeration
 */
typedef enum {
    NEXUS_STATUS_SUCCESS,    // Operation succeeded
    NEXUS_STATUS_EXCEPTION,  // Recoverable exception occurred
    NEXUS_STATUS_ERROR       // Unrecoverable error occurred
} nexus_result_status;

/**
 * Result monad for handling success/exception/error states
 */
typedef struct {
    nexus_result_status status;   // Status of the result
    union {
        void* value;              // Success value
        nexus_exception* exception; // Recoverable exception
        nexus_error* error;       // Unrecoverable error
    };
    NexusContext* context;        // Context for error handling
} nexus_result;

/**
 * Create a success result
 * @param value Success value
 * @param context Context for error handling
 * @return Success result
 */
nexus_result nexus_success(void* value, NexusContext* context);

/**
 * Create an exception result
 * @param exception Exception object (ownership transferred)
 * @param context Context for error handling
 * @return Exception result
 */
nexus_result nexus_exception_result(nexus_exception* exception, NexusContext* context);

/**
 * Create an error result
 * @param error Error object (ownership transferred)
 * @param context Context for error handling
 * @return Error result
 */
nexus_result nexus_error_result(nexus_error* error, NexusContext* context);

/**
 * Free resources associated with a result
 * @param result Result to free
 */
void nexus_result_free(nexus_result* result);

/**
 * Convert exception result to error result
 * @param result Result to convert
 * @return Converted result
 */
nexus_result nexus_result_exception_to_error(nexus_result result);

/**
 * Log the result of an operation
 * @param result Result to log
 */
void nexus_result_log(const nexus_result* result);

/**
 * Check if a result is successful
 * @param result Result to check
 * @return true if successful
 */
static inline bool nexus_result_is_success(const nexus_result* result) {
    return result != NULL && result->status == NEXUS_STATUS_SUCCESS;
}

/**
 * Check if a result is an exception
 * @param result Result to check
 * @return true if exception
 */
static inline bool nexus_result_is_exception(const nexus_result* result) {
    return result != NULL && result->status == NEXUS_STATUS_EXCEPTION;
}

/**
 * Check if a result is an error
 * @param result Result to check
 * @return true if error
 */
static inline bool nexus_result_is_error(const nexus_result* result) {
    return result != NULL && result->status == NEXUS_STATUS_ERROR;
}

/**
 * Convenience macro for trying operations that return nexus_result
 * Propagates both exceptions and errors but allows handling exceptions
 */
#define NEXUS_TRY(expr) \
    do { \
        nexus_result _result = (expr); \
        if (_result.status != NEXUS_STATUS_SUCCESS) { \
            return _result; \
        } \
    } while (0)

/**
 * Convenience macro for trying operations with exception handling
 * Exceptions can be handled, errors are propagated
 */
#define NEXUS_TRY_CATCH(expr, exception_handler) \
    do { \
        nexus_result _result = (expr); \
        if (_result.status == NEXUS_STATUS_EXCEPTION) { \
            exception_handler(_result.exception); \
        } else if (_result.status == NEXUS_STATUS_ERROR) { \
            return _result; \
        } \
    } while (0)

/**
 * Convenience macro for trying operations with conversion
 * Converts unhandled exceptions to errors and propagates
 */
#define NEXUS_TRY_OR_ERROR(expr) \
    do { \
        nexus_result _result = (expr); \
        if (_result.status == NEXUS_STATUS_EXCEPTION) { \
            return nexus_result_exception_to_error(_result); \
        } else if (_result.status == NEXUS_STATUS_ERROR) { \
            return _result; \
        } \
    } while (0)

#endif /* NEXUS_RESULT_H */