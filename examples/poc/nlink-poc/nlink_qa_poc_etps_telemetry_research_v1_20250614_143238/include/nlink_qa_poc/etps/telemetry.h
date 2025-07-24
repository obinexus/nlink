/**
 * @file telemetry.h
 * @brief ETPS Telemetry System Header - Fixed Type Definitions
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.1
 */

#ifndef NLINK_QA_POC_ETPS_TELEMETRY_H
#define NLINK_QA_POC_ETPS_TELEMETRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Forward Declarations and Core Types
// =============================================================================

typedef struct etps_context etps_context_t;
typedef struct etps_semverx_event etps_semverx_event_t;

// =============================================================================
// SemVerX Range State Types
// =============================================================================

typedef enum {
    SEMVERX_RANGE_LEGACY = 1,           // Legacy components (deprecated)
    SEMVERX_RANGE_STABLE = 2,           // Stable production components
    SEMVERX_RANGE_EXPERIMENTAL = 3      // Experimental/beta components
} semverx_range_state_t;

typedef enum {
    COMPAT_ALLOWED = 1,                 // Integration allowed
    COMPAT_REQUIRES_VALIDATION = 2,     // Requires manual validation
    COMPAT_DENIED = 3                   // Integration denied
} compatibility_result_t;

typedef enum {
    HOTSWAP_SUCCESS = 1,                // Hot-swap successful
    HOTSWAP_FAILED = 2,                 // Hot-swap failed
    HOTSWAP_NOT_APPLICABLE = 3          // Hot-swap not applicable
} hotswap_result_t;

// =============================================================================
// Basic ETPS Types
// =============================================================================

typedef enum {
    ETPS_COMPONENT_CONFIG = 1,
    ETPS_COMPONENT_CLI = 2,
    ETPS_COMPONENT_CORE = 3,
    ETPS_COMPONENT_PARSER = 4
} etps_component_t;

typedef enum {
    ETPS_ERROR_NONE = 0,
    ETPS_ERROR_INVALID_INPUT = 1001,
    ETPS_ERROR_MEMORY_FAULT = 1002,
    ETPS_ERROR_CONFIG_PARSE = 1003,
    ETPS_ERROR_FILE_IO = 1004
} etps_error_code_t;

// =============================================================================
// SemVerX Component Metadata
// =============================================================================

typedef struct {
    char name[64];                      // Component name
    char version[32];                   // Component version (SemVer)
    semverx_range_state_t range_state;  // Range state classification
    char compatible_range[128];         // Compatible version range
    bool hot_swap_enabled;              // Hot-swap capability
    char migration_policy[64];          // Migration policy identifier
    uint64_t component_id;              // Unique component identifier
} semverx_component_t;

// =============================================================================
// ETPS SemVerX Event Structure
// =============================================================================

struct etps_semverx_event {
    char event_id[37];                  // GUID string (36 chars + null)
    char timestamp[32];                 // ISO8601 timestamp
    char layer[32];                     // Always "semverx_validation"
    
    // Component Information
    semverx_component_t source_component;
    semverx_component_t target_component;
    
    // Validation Results
    compatibility_result_t compatibility_result;
    bool hot_swap_attempted;
    hotswap_result_t hot_swap_result;
    char resolution_policy_triggered[64];
    
    // Telemetry Metadata
    int severity;                       // 1=info, 5=critical
    char migration_recommendation[256]; // Suggested migration path
    
    // Context
    char project_path[256];             // Path to .nlink or pkg.nlink
    char build_target[64];              // Build target (debug/release/etc)
};

// =============================================================================
// ETPS Context Structure
// =============================================================================

struct etps_context {
    uint64_t binding_guid;              // Unique binding identifier
    uint64_t created_time;              // Context creation timestamp
    uint64_t last_activity;             // Last activity timestamp
    char context_name[64];              // Human-readable context name
    bool is_active;                     // Context active status
    
    // SemVerX Extensions
    char project_root[256];             // Project root directory
    semverx_component_t* registered_components; // Array of registered components
    size_t component_count;             // Number of registered components
    size_t component_capacity;          // Capacity of components array
    
    // Policy Configuration
    bool strict_mode;                   // Strict compatibility checking
    bool allow_experimental_stable;    // Allow experimental in stable builds
    bool auto_migration_enabled;       // Enable automatic migrations
};

// =============================================================================
// Function Declarations (NO IMPLEMENTATIONS IN HEADER)
// =============================================================================

// Core ETPS functions
int etps_init(void);
void etps_shutdown(void);
bool etps_is_initialized(void);
etps_context_t* etps_context_create(const char* context_name);
void etps_context_destroy(etps_context_t* ctx);

// SemVerX functions
int etps_register_component(etps_context_t* ctx, const semverx_component_t* component);
compatibility_result_t etps_validate_component_compatibility(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component,
    etps_semverx_event_t* event
);
void etps_emit_semverx_event(etps_context_t* ctx, const etps_semverx_event_t* event);
hotswap_result_t etps_attempt_hotswap(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component
);

// Utility functions
uint64_t etps_get_current_timestamp(void);
const char* etps_range_state_to_string(semverx_range_state_t state);
const char* etps_compatibility_result_to_string(compatibility_result_t result);
const char* etps_hotswap_result_to_string(hotswap_result_t result);
void etps_generate_iso8601_timestamp(char* buffer, size_t max_len);
void etps_generate_guid_string(char* buffer);

// Basic validation functions
bool etps_validate_input(etps_context_t* ctx, const char* param_name, 
                        const void* value, const char* type);
bool etps_validate_config(etps_context_t* ctx, const char* buffer, size_t size);

// Logging functions
void etps_log_error(etps_context_t* ctx, etps_component_t component, 
                   etps_error_code_t error_code, const char* function, const char* message);
void etps_log_info(etps_context_t* ctx, etps_component_t component, 
                  const char* function, const char* message);

// CLI functions
int nlink_cli_validate_compatibility(int argc, char* argv[]);
int nlink_cli_semverx_status(int argc, char* argv[]);
int nlink_cli_migration_plan(int argc, char* argv[]);
int etps_validate_project_compatibility(const char* project_path);
int etps_export_events_json(etps_context_t* ctx, const char* output_path);

#ifdef __cplusplus
}
#endif

#endif // NLINK_QA_POC_ETPS_TELEMETRY_H
