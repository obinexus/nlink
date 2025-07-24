/**
 * =============================================================================
 * OBINexus NexusLink - SemVerX ETPS Integration
 * Advanced Component Compatibility Validation with Structured Telemetry
 * =============================================================================
 * 
 * Implements:
 * - SemVerX range_state validation (legacy, stable, experimental)
 * - Cross-component integration safety
 * - Hot-swap policy enforcement
 * - Structured ETPS event emission
 * - Migration path recommendations
 */

#ifndef NLINK_SEMVERX_ETPS_H
#define NLINK_SEMVERX_ETPS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct {
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
} etps_semverx_event_t;

// =============================================================================
// ETPS System Context (Enhanced for SemVerX)
// =============================================================================

typedef struct etps_context {
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
} etps_context_t;

// =============================================================================
// Core ETPS Functions (Missing from previous implementation)
// =============================================================================

/**
 * Initialize global ETPS system
 * @return 0 on success, -1 on failure
 */
int etps_init(void);

/**
 * Shutdown global ETPS system and cleanup resources
 */
void etps_shutdown(void);

/**
 * Get global ETPS initialization status
 * @return true if initialized, false otherwise
 */
bool etps_is_initialized(void);

// =============================================================================
// SemVerX Integration Functions
// =============================================================================

/**
 * Register a component with SemVerX metadata
 * @param ctx ETPS context
 * @param component Component metadata
 * @return 0 on success, -1 on failure
 */
int etps_register_component(etps_context_t* ctx, const semverx_component_t* component);

/**
 * Validate component compatibility
 * @param ctx ETPS context
 * @param source_component Source component
 * @param target_component Target component being integrated
 * @param event Output event structure (filled on validation)
 * @return compatibility_result_t
 */
compatibility_result_t etps_validate_component_compatibility(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component,
    etps_semverx_event_t* event
);

/**
 * Emit ETPS SemVerX event (structured telemetry)
 * @param ctx ETPS context
 * @param event Event to emit
 */
void etps_emit_semverx_event(etps_context_t* ctx, const etps_semverx_event_t* event);

/**
 * Attempt hot-swap between components
 * @param ctx ETPS context
 * @param source_component Current component
 * @param target_component Target component for swap
 * @return hotswap_result_t
 */
hotswap_result_t etps_attempt_hotswap(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component
);

/**
 * Get migration recommendation for incompatible components
 * @param source_component Source component
 * @param target_component Target component
 * @param recommendation Output buffer for recommendation
 * @param max_len Maximum length of recommendation buffer
 */
void etps_get_migration_recommendation(
    const semverx_component_t* source_component,
    const semverx_component_t* target_component,
    char* recommendation,
    size_t max_len
);

// =============================================================================
// CLI Integration Functions
// =============================================================================

/**
 * Validate compatibility for entire project
 * @param project_path Path to .nlink or pkg.nlink file
 * @return 0 if all compatible, >0 for number of violations
 */
int etps_validate_project_compatibility(const char* project_path);

/**
 * Load project components from .nlink or pkg.nlink
 * @param ctx ETPS context
 * @param project_path Path to project file
 * @return 0 on success, -1 on failure
 */
int etps_load_project_components(etps_context_t* ctx, const char* project_path);

/**
 * Export ETPS events to JSON for CI/CD integration
 * @param ctx ETPS context
 * @param output_path Output file path
 * @return 0 on success, -1 on failure
 */
int etps_export_events_json(etps_context_t* ctx, const char* output_path);

// =============================================================================
// Utility Functions
// =============================================================================

/**
 * Convert range_state to string
 */
const char* etps_range_state_to_string(semverx_range_state_t state);

/**
 * Convert compatibility_result to string
 */
const char* etps_compatibility_result_to_string(compatibility_result_t result);

/**
 * Convert hotswap_result to string
 */
const char* etps_hotswap_result_to_string(hotswap_result_t result);

/**
 * Generate ISO8601 timestamp
 * @param buffer Output buffer
 * @param max_len Maximum buffer length
 */
void etps_generate_iso8601_timestamp(char* buffer, size_t max_len);

/**
 * Generate GUID string
 * @param buffer Output buffer (must be at least 37 chars)
 */
void etps_generate_guid_string(char* buffer);

// =============================================================================
// Component Registration Macros (for calculator.c, scientific.c)
// =============================================================================

#define ETPS_REGISTER_COMPONENT_METADATA(ctx, name, version, range_state, compatible_range, hot_swap) \
    do { \
        semverx_component_t comp = {0}; \
        strncpy(comp.name, name, sizeof(comp.name) - 1); \
        strncpy(comp.version, version, sizeof(comp.version) - 1); \
        comp.range_state = range_state; \
        strncpy(comp.compatible_range, compatible_range, sizeof(comp.compatible_range) - 1); \
        comp.hot_swap_enabled = hot_swap; \
        comp.component_id = etps_get_current_timestamp(); \
        etps_register_component(ctx, &comp); \
    } while(0)

#define ETPS_REGISTER_EXPERIMENTAL_COMPONENT(ctx, name, version) \
    ETPS_REGISTER_COMPONENT_METADATA(ctx, name, version, SEMVERX_RANGE_EXPERIMENTAL, "*", false)

#define ETPS_REGISTER_STABLE_COMPONENT(ctx, name, version, compatible_range) \
    ETPS_REGISTER_COMPONENT_METADATA(ctx, name, version, SEMVERX_RANGE_STABLE, compatible_range, true)

// =============================================================================
// CLI Command Integration
// =============================================================================

/**
 * CLI command: nlink --validate-compatibility
 */
int nlink_cli_validate_compatibility(int argc, char* argv[]);

/**
 * CLI command: nlink --semverx-status
 */
int nlink_cli_semverx_status(int argc, char* argv[]);

/**
 * CLI command: nlink --migration-plan
 */
int nlink_cli_migration_plan(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif // NLINK_SEMVERX_ETPS_H