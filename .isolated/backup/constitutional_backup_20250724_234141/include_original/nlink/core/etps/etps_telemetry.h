#ifndef NLINK_CORE_ETPS_TELEMETRY_H
#define NLINK_CORE_ETPS_TELEMETRY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "semverx/core/types.h"
#include "semverx/compatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

// Error codes
typedef enum {
    ETPS_ERROR_SUCCESS = 0,
    ETPS_ERROR_INVALID_PARAMETER = 1,
    ETPS_ERROR_MEMORY_ALLOCATION = 2,
    ETPS_ERROR_INITIALIZATION_FAILURE = 3
} etps_error_code_t;

// Component classification
typedef enum {
    ETPS_COMPONENT_UNKNOWN = 0,
    ETPS_COMPONENT_CORE = 1,
    ETPS_COMPONENT_CLI = 2,
    ETPS_COMPONENT_PARSER = 3,
    ETPS_COMPONENT_MINIMIZER = 4,
    ETPS_COMPONENT_SEMVERX = 5
} etps_component_t;

// ETPS context structure
typedef struct etps_context {
    uint64_t binding_guid;
    uint64_t created_time;
    uint64_t last_activity;
    bool is_active;
    char context_name[128];
    char project_root[512];
    semverx_component_t* registered_components;
    size_t component_count;
    size_t component_capacity;
    bool strict_mode;
    bool allow_experimental_stable;
    bool auto_migration_enabled;
} etps_context_t;

// SemVerX event structure
typedef struct etps_semverx_event {
    char event_id[37];
    char timestamp[32];
    char layer[64];
    semverx_component_t source_component;
    semverx_component_t target_component;
    compatibility_result_t compatibility_result;
    uint32_t severity;
    char migration_recommendation[256];
    char project_path[512];
    char build_target[64];
} etps_semverx_event_t;

// Function declarations (NO IMPLEMENTATIONS)
int etps_init(void);
void etps_shutdown(void);
bool etps_is_initialized(void);
etps_context_t* etps_context_create(const char* context_name);
void etps_context_destroy(etps_context_t* ctx);
int etps_register_component(etps_context_t* ctx, const semverx_component_t* component);
compatibility_result_t etps_validate_component_compatibility(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component,
    etps_semverx_event_t* event);
void etps_emit_semverx_event(etps_context_t* ctx, const etps_semverx_event_t* event);
hotswap_result_t etps_attempt_hotswap(
    etps_context_t* ctx,
    const semverx_component_t* source_component,
    const semverx_component_t* target_component);

// Utility functions
uint64_t etps_get_current_timestamp(void);
void etps_generate_guid_string(char* buffer);
void etps_generate_iso8601_timestamp(char* buffer, size_t max_len);
const char* etps_range_state_to_string(semverx_range_state_t state);
const char* etps_compatibility_result_to_string(compatibility_result_t result);
const char* etps_hotswap_result_to_string(hotswap_result_t result);

// Validation functions
bool etps_validate_input(etps_context_t* ctx, const char* param_name, const void* value, const char* type);
bool etps_validate_config(etps_context_t* ctx, const char* buffer, size_t size);
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

#endif // NLINK_CORE_ETPS_TELEMETRY_H
