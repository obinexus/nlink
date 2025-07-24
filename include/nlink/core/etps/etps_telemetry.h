// include/nlink/core/etps/etps_telemetry.h
// OBINexus ETPS: Entropy-Adaptive Telemetry Polymorphic Coordination
#ifndef OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H
#define OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Result Type System
typedef enum {
    ETPS_ERROR_SUCCESS = 0,
    ETPS_ERROR_INVALID_PARAMETER = -1,
    ETPS_ERROR_INITIALIZATION_FAILED = -2,
    ETPS_ERROR_MEMORY_ALLOCATION = -3,
    ETPS_ERROR_CONFIGURATION_INVALID = -4,
    ETPS_ERROR_TELEMETRY_DISABLED = -5
} etps_result_t;

// Hotswap Coordination Result System
typedef enum {
    HOTSWAP_SUCCESS = 0,
    HOTSWAP_ERROR_INVALID_STATE = -1,
    HOTSWAP_ERROR_COORDINATION_FAILED = -2,
    HOTSWAP_ERROR_BOUNDARY_VIOLATION = -3
} hotswap_result_t;

// ETPS Context: Entropy-Adaptive State Management
typedef struct etps_context {
    uint32_t magic_marker;          // Constitutional validation marker
    bool initialized;               // Initialization state flag
    uint64_t telemetry_sessions;    // Active telemetry coordination sessions
    void *entropy_pool;             // Adaptive entropy management pool
} etps_context_t;

// Constitutional Interface Functions: Polymorphic Coordination Protocol
etps_result_t etps_initialize(void);
etps_result_t etps_shutdown(void);
etps_result_t etps_create_context(etps_context_t **ctx);
etps_result_t etps_destroy_context(etps_context_t *ctx);
etps_result_t etps_configure_telemetry(etps_context_t *ctx, const char *config);
etps_result_t etps_start_telemetry_session(etps_context_t *ctx);
etps_result_t etps_stop_telemetry_session(etps_context_t *ctx);

// Hotswap Coordination Protocol
hotswap_result_t etps_hotswap_prepare(etps_context_t *ctx);
hotswap_result_t etps_hotswap_execute(etps_context_t *ctx);
hotswap_result_t etps_hotswap_validate(etps_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H
