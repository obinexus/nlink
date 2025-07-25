/**
 * NexusLink ETPS (Error Telemetry Point System) - Fixed Core Header
 * OBINexus Aegis Engineering - GUID + Timestamp Integration for Structured Error Reporting
 */

#ifndef NLINK_ETPS_TELEMETRY_H
#define NLINK_ETPS_TELEMETRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


// ETPS Core Types
typedef uint64_t etps_guid_t;
typedef uint64_t etps_timestamp_t;
typedef struct etps_context_t {
    char project_root[256];                 // example size
    void* registered_components;           // actual type array/pointer
    int component_count;
    int component_capacity;
    bool strict_mode;
    bool allow_experimental_stable;
    bool auto_migration_enabled;
} etps_context_t;

// ETPS Component Types
typedef enum {
    ETPS_COMPONENT_CONFIG = 1,          // Configuration subsystem
    ETPS_COMPONENT_CLI = 2,             // Command line interface
    ETPS_COMPONENT_CORE = 3,            // Core library functions
    ETPS_COMPONENT_PARSER = 4           // Parser subsystem
} etps_component_t;

// ETPS Error Codes
typedef enum {
    ETPS_ERROR_NONE = 0,                // No error
    ETPS_ERROR_INVALID_INPUT = 1001,    // Invalid input parameters
    ETPS_ERROR_MEMORY_FAULT = 1002,     // Memory allocation failure
    ETPS_ERROR_CONFIG_PARSE = 1003,     // Configuration parsing error
    ETPS_ERROR_FILE_IO = 1004           // File I/O error
} etps_error_code_t;

// =============================================================================
// Core ETPS Functions
// =============================================================================

/**
 * Create a new ETPS context
 * @param context_name Human-readable name for the context
 * @return Pointer to new context, or NULL on failure
 */
etps_context_t* etps_context_create(const char* context_name);

/**
 * Destroy an ETPS context and free resources
 * @param ctx Context to destroy
 */
void etps_context_destroy(etps_context_t* ctx);

/**
 * Validate input parameters with type checking
 * @param ctx ETPS context
 * @param param_name Name of parameter being validated
 * @param value Pointer to value being validated
 * @param type Expected type as string
 * @return true if valid, false otherwise
 */
bool etps_validate_input(etps_context_t* ctx, const char* param_name,
                        const void* value, const char* type);

/**
 * Validate configuration buffer
 * @param ctx ETPS context
 * @param buffer Configuration buffer
 * @param size Buffer size
 * @return true if valid, false otherwise
 */
bool etps_validate_config(etps_context_t* ctx, const char* buffer, size_t size);

// =============================================================================
// Logging Functions
// =============================================================================

/**
 * Log an error with structured telemetry
 * @param ctx ETPS context
 * @param component Component where error occurred
 * @param error_code Specific error code
 * @param function Function name where error occurred
 * @param message Human-readable error message
 */
void etps_log_error(etps_context_t* ctx, etps_component_t component,
                   etps_error_code_t error_code, const char* function, const char* message);

/**
 * Log informational message with structured telemetry
 * @param ctx ETPS context
 * @param component Component logging the message
 * @param function Function name logging the message
 * @param message Human-readable info message
 */
void etps_log_info(etps_context_t* ctx, etps_component_t component,
                  const char* function, const char* message);

// =============================================================================
// Utility Functions
// =============================================================================

/**
 * Get current timestamp
 * @return Current timestamp as uint64_t
 */
uint64_t etps_get_current_timestamp(void);

/**
 * Get human-readable component name
 * @param component Component enum value
 * @return String representation of component name
 */
const char* etps_get_component_name(etps_component_t component);

/**
 * Get human-readable error name
 * @param error_code Error code enum value
 * @return String representation of error name
 */
const char* etps_get_error_name(etps_error_code_t error_code);

// =============================================================================
// Logging Macros for Convenience
// =============================================================================

/**
 * Convenience macro for logging errors
 */
#define ETPS_LOG_ERROR(ctx, component, error_code, function, message) \
    etps_log_error(ctx, component, error_code, function, message)

/**
 * Convenience macro for logging info messages
 */
#define ETPS_LOG_INFO(ctx, component, function, message) \
    etps_log_info(ctx, component, function, message)

/**
 * Convenience macro for parameter validation
 */
#define ETPS_VALIDATE_PARAM(ctx, name, value, type) \
    etps_validate_input(ctx, name, value, type)

#ifdef __cplusplus
}
#endif

#endif // NLINK_ETPS_TELEMETRY_H
