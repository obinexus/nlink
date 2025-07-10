/**
 * =============================================================================
 * NexusLink ETPS (Error Telemetry Point System) - Core Header
 * OBINexus Aegis Engineering - Telemetry Debugging via GUID and Timestamp
 * =============================================================================
 * 
 * Implements the OBINexus ETPS specification for structured error reporting,
 * GUID-based correlation, and temporal ordering of system events.
 * 
 * Author: Nnamdi Michael Okpala (OBINexus Computing)
 * Framework: Program-First Architecture with Zero-Trust Telemetry
 */

#ifndef NLINK_ETPS_TELEMETRY_H
#define NLINK_ETPS_TELEMETRY_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// ETPS Core Types and Constants
// =============================================================================

// GUID: Cryptonomic State Transition Hash
typedef uint64_t etps_guid_t;

// High-resolution timestamp (nanoseconds since epoch)
typedef uint64_t etps_timestamp_t;

// Error severity levels aligned with ETPS specification
typedef enum {
    ETPS_SEVERITY_DEBUG     = 0,    // Debug information
    ETPS_SEVERITY_INFO      = 1,    // Informational events
    ETPS_SEVERITY_WARNING   = 2,    // Warning conditions
    ETPS_SEVERITY_ERROR     = 3,    // Recoverable error conditions
    ETPS_SEVERITY_PANIC     = 4,    // Critical, unrecoverable faults
    ETPS_SEVERITY_FATAL     = 5     // System failure - immediate shutdown
} etps_severity_t;

// Component/Layer identification for error location
typedef enum {
    ETPS_COMPONENT_CLI          = 1,    // CLI command layer
    ETPS_COMPONENT_CORE         = 2,    // Core library functions  
    ETPS_COMPONENT_CONFIG       = 3,    // Configuration parsing
    ETPS_COMPONENT_MARSHAL      = 4,    // Data marshalling
    ETPS_COMPONENT_NETWORK      = 5,    // Network operations
    ETPS_COMPONENT_VALIDATION   = 6,    // Input/output validation
    ETPS_COMPONENT_SECURITY     = 7,    // Security and auth
    ETPS_COMPONENT_FFI          = 8,    // Foreign function interfaces
    ETPS_COMPONENT_BINDINGS     = 9     // Language bindings
} etps_component_t;

// Error classification codes
typedef enum {
    ETPS_ERROR_SUCCESS          = 0,    // Operation successful
    ETPS_ERROR_INVALID_INPUT    = 1001, // Invalid input parameters
    ETPS_ERROR_CONFIG_PARSE     = 1002, // Configuration parsing failed
    ETPS_ERROR_NETWORK_TIMEOUT  = 2001, // Network timeout
    ETPS_ERROR_AUTH_FAILED      = 2002, // Authentication failure
    ETPS_ERROR_PERMISSION       = 2003, // Permission denied
    ETPS_ERROR_MARSHAL_FAILED   = 3001, // Marshalling failure
    ETPS_ERROR_VALIDATION       = 3002, // Validation failure
    ETPS_ERROR_FFI_BRIDGE       = 4001, // FFI bridge error
    ETPS_ERROR_BINDING_FAILED   = 4002, // Language binding error
    ETPS_ERROR_SYSTEM_PANIC     = 9001, // System panic condition
    ETPS_ERROR_MEMORY_FAULT     = 9002, // Memory allocation failure
    ETPS_ERROR_UNRECOVERABLE    = 9999  // Unrecoverable system error
} etps_error_code_t;

// ETPS Event structure - core telemetry record
typedef struct {
    etps_guid_t         guid;           // Unique session/operation identifier
    etps_timestamp_t    timestamp;      // High-resolution timestamp
    etps_severity_t     severity;       // Error severity level
    etps_component_t    component;      // Component that generated the event
    etps_error_code_t   error_code;     // Specific error code
    char                operation[64];  // Operation/function name
    char                message[256];   // Human-readable error message
    char                context[128];   // Additional context information
    uint32_t            line_number;    // Source code line number
    char                file_name[64];  // Source file name
} etps_event_t;

// ETPS Context for session tracking
typedef struct {
    etps_guid_t         session_guid;   // Session-level GUID
    etps_timestamp_t    session_start;  // Session start timestamp
    uint32_t            event_count;    // Number of events in session
    char                command_path[128]; // CLI command path (DFA route)
    bool                panic_mode;     // Panic mode flag
    etps_severity_t     max_severity;   // Highest severity seen
} etps_context_t;

// =============================================================================
// ETPS Core Functions
// =============================================================================

/**
 * Initialize ETPS telemetry system
 * Must be called before any other ETPS functions
 */
int etps_init(void);

/**
 * Shutdown ETPS telemetry system
 * Flushes all pending telemetry data
 */
void etps_shutdown(void);

/**
 * Generate a new GUID for session/operation tracking
 * Uses cryptographic state transition hash algorithm
 */
etps_guid_t etps_generate_guid(void);

/**
 * Get current high-resolution timestamp
 * Returns nanoseconds since Unix epoch
 */
etps_timestamp_t etps_get_timestamp(void);

/**
 * Create new ETPS context for session tracking
 * @param command_path CLI command path (for DFA route tracking)
 * @return New ETPS context or NULL on failure
 */
etps_context_t* etps_context_create(const char* command_path);

/**
 * Destroy ETPS context and flush telemetry
 * @param ctx ETPS context to destroy
 */
void etps_context_destroy(etps_context_t* ctx);

/**
 * Update GUID with state transition information
 * Implements cryptonomic state transition hash
 * @param current_guid Current GUID value
 * @param transition_data State transition information
 * @param data_size Size of transition data
 * @return Updated GUID incorporating transition
 */
etps_guid_t etps_guid_update(etps_guid_t current_guid, 
                            const void* transition_data, 
                            size_t data_size);

// =============================================================================
// ETPS Event Logging Functions
// =============================================================================

/**
 * Log ETPS event with full context
 * Primary function for structured error reporting
 */
void etps_log_event(etps_context_t* ctx,
                   etps_severity_t severity,
                   etps_component_t component,
                   etps_error_code_t error_code,
                   const char* operation,
                   const char* message,
                   const char* file,
                   uint32_t line);

/**
 * Log error event - convenience function
 */
void etps_log_error(etps_context_t* ctx,
                   etps_component_t component,
                   etps_error_code_t error_code,
                   const char* operation,
                   const char* message,
                   const char* file,
                   uint32_t line);

/**
 * Log panic event - critical system failure
 * Triggers immediate telemetry flush and system notification
 */
void etps_log_panic(etps_context_t* ctx,
                   etps_component_t component,
                   etps_error_code_t error_code,
                   const char* operation,
                   const char* message,
                   const char* file,
                   uint32_t line);

/**
 * Log validation failure with detailed context
 */
void etps_log_validation_failure(etps_context_t* ctx,
                                const char* validation_type,
                                const char* expected_value,
                                const char* actual_value,
                                const char* file,
                                uint32_t line);

// =============================================================================
// ETPS Convenience Macros
// =============================================================================

// Automatic file/line capture for ETPS logging
#define ETPS_LOG_ERROR(ctx, component, code, op, msg) \
    etps_log_error(ctx, component, code, op, msg, __FILE__, __LINE__)

#define ETPS_LOG_PANIC(ctx, component, code, op, msg) \
    etps_log_panic(ctx, component, code, op, msg, __FILE__, __LINE__)

#define ETPS_LOG_VALIDATION_FAILURE(ctx, type, expected, actual) \
    etps_log_validation_failure(ctx, type, expected, actual, __FILE__, __LINE__)

#define ETPS_LOG_INFO(ctx, component, op, msg) \
    etps_log_event(ctx, ETPS_SEVERITY_INFO, component, ETPS_ERROR_SUCCESS, \
                   op, msg, __FILE__, __LINE__)

#define ETPS_LOG_WARNING(ctx, component, code, op, msg) \
    etps_log_event(ctx, ETPS_SEVERITY_WARNING, component, code, \
                   op, msg, __FILE__, __LINE__)

// =============================================================================
// ETPS JSON Output Functions
// =============================================================================

/**
 * Export ETPS event as JSON string
 * For structured telemetry output and external log aggregation
 * @param event ETPS event to serialize  
 * @param json_buffer Output buffer for JSON string
 * @param buffer_size Size of output buffer
 * @return Number of bytes written or -1 on error
 */
int etps_event_to_json(const etps_event_t* event, 
                      char* json_buffer, 
                      size_t buffer_size);

/**
 * Export ETPS context statistics as JSON
 * @param ctx ETPS context
 * @param json_buffer Output buffer for JSON string  
 * @param buffer_size Size of output buffer
 * @return Number of bytes written or -1 on error
 */
int etps_context_to_json(const etps_context_t* ctx,
                        char* json_buffer,
                        size_t buffer_size);

// =============================================================================
// ETPS Validation Functions
// =============================================================================

/**
 * Validate configuration parameters with ETPS logging
 * @param ctx ETPS context
 * @param config_data Configuration data to validate
 * @param config_size Size of configuration data
 * @return true if valid, false if validation failed (with ETPS event logged)
 */
bool etps_validate_config(etps_context_t* ctx,
                         const void* config_data,
                         size_t config_size);

/**
 * Validate input parameters with type checking
 * @param ctx ETPS context
 * @param param_name Parameter name for error reporting
 * @param value Parameter value  
 * @param expected_type Expected type identifier
 * @return true if valid, false if validation failed (with ETPS event logged)
 */
bool etps_validate_input(etps_context_t* ctx,
                        const char* param_name,
                        const void* value,
                        const char* expected_type);

/**
 * Validate marshalling integrity
 * @param ctx ETPS context
 * @param marshalled_data Marshalled data buffer
 * @param expected_checksum Expected checksum value
 * @param data_size Size of marshalled data
 * @return true if integrity check passed, false otherwise
 */
bool etps_validate_marshal_integrity(etps_context_t* ctx,
                                    const void* marshalled_data,
                                    uint32_t expected_checksum,
                                    size_t data_size);

// =============================================================================
// ETPS Cross-Language Binding Support
// =============================================================================

/**
 * Register language binding with ETPS telemetry
 * For tracking errors across FFI boundaries
 * @param binding_name Name of the language binding (e.g., "python", "java")
 * @param binding_version Version of the binding
 * @return Binding registration ID for telemetry correlation
 */
uint32_t etps_register_binding(const char* binding_name, 
                              const char* binding_version);

/**
 * Log cross-language error event
 * @param ctx ETPS context
 * @param binding_id Binding registration ID
 * @param error_message Error message from binding
 * @param binding_context Additional binding-specific context
 */
void etps_log_binding_error(etps_context_t* ctx,
                           uint32_t binding_id,
                           const char* error_message,
                           const char* binding_context);

// =============================================================================
// ETPS Zero-Trust Security Integration
// =============================================================================

/**
 * Generate security validation GUID
 * Creates tamper-proof GUID for security-critical operations
 * @param operation_data Data to include in security hash
 * @param data_size Size of operation data
 * @param secret_key Secret key for HMAC generation
 * @param key_size Size of secret key
 * @return Security-validated GUID
 */
etps_guid_t etps_generate_security_guid(const void* operation_data,
                                       size_t data_size,
                                       const void* secret_key,
                                       size_t key_size);

/**
 * Verify security GUID integrity
 * @param security_guid GUID to verify
 * @param operation_data Original operation data
 * @param data_size Size of operation data  
 * @param secret_key Secret key for verification
 * @param key_size Size of secret key
 * @return true if GUID is valid, false if tampered
 */
bool etps_verify_security_guid(etps_guid_t security_guid,
                              const void* operation_data,
                              size_t data_size,
                              const void* secret_key,
                              size_t key_size);

#ifdef __cplusplus
}
#endif

#endif // NLINK_ETPS_TELEMETRY_H
