// include/nlink/core/etps/etps_types.h
// OBINexus ETPS Types: Constitutional Type Coordination Framework
#ifndef OBINEXUS_NLINK_CORE_ETPS_ETPS_TYPES_H
#define OBINEXUS_NLINK_CORE_ETPS_ETPS_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional ETPS Result Type System (Unified)
typedef enum {
    ETPS_SUCCESS = 0,
    ETPS_ERROR_INVALID_PARAMETER = -1,
    ETPS_ERROR_INITIALIZATION_FAILED = -2,
    ETPS_ERROR_MEMORY_ALLOCATION = -3,
    ETPS_ERROR_CONFIGURATION_INVALID = -4,
    ETPS_ERROR_TELEMETRY_DISABLED = -5,
    ETPS_ERROR_CONTEXT_NOT_FOUND = -6,
    ETPS_ERROR_SESSION_FAILED = -7
} etps_result_t;

// Constitutional Hotswap Result Type System (Non-Conflicting)
typedef enum {
    HOTSWAP_RESULT_SUCCESS = 0,
    HOTSWAP_RESULT_ERROR_INVALID_STATE = -1,
    HOTSWAP_RESULT_ERROR_COORDINATION_FAILED = -2,
    HOTSWAP_RESULT_ERROR_BOUNDARY_VIOLATION = -3,
    HOTSWAP_RESULT_ERROR_ENTROPY_EXHAUSTED = -4,
    HOTSWAP_RESULT_ERROR_VALIDATION_FAILED = -5
} hotswap_result_t;

// Constitutional Hotswap State Machine
typedef enum {
    HOTSWAP_STATE_IDLE = 0,
    HOTSWAP_STATE_PREPARING = 1,
    HOTSWAP_STATE_EXECUTING = 2,
    HOTSWAP_STATE_VALIDATING = 3,
    HOTSWAP_STATE_COMPLETED = 4,
    HOTSWAP_STATE_FAILED = -1
} hotswap_state_t;

// Constitutional Hotswap Operation Types
typedef enum {
    HOTSWAP_OP_NONE = 0,
    HOTSWAP_OP_MODULE_RELOAD = 1,
    HOTSWAP_OP_CONFIG_UPDATE = 2,
    HOTSWAP_OP_TELEMETRY_RECONFIGURE = 3,
    HOTSWAP_OP_BOUNDARY_ADJUSTMENT = 4
} hotswap_operation_t;

// Constitutional ETPS Context Complete Structure
typedef struct etps_context {
    uint32_t magic_marker;          // Constitutional validation marker: 0x45545053 "ETPS"
    bool initialized;               // Initialization state flag
    uint64_t telemetry_sessions;    // Active telemetry coordination sessions
    void *entropy_pool;             // Adaptive entropy management pool
    size_t entropy_pool_size;       // Entropy pool allocation size
    uint32_t session_count;         // Total session coordination counter
    void *hotswap_registry;         // Hotswap coordination registry
} etps_context_t;

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_ETPS_ETPS_TYPES_H
