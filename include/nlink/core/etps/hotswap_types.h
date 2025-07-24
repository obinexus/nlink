// include/nlink/core/etps/hotswap_types.h
// OBINexus ETPS: Hotswap Constitutional Types Framework
#ifndef OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H
#define OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Hotswap State Machine: Constitutional Coordination Protocol
typedef enum {
    HOTSWAP_STATE_IDLE = 0,
    HOTSWAP_STATE_PREPARING = 1,
    HOTSWAP_STATE_EXECUTING = 2,
    HOTSWAP_STATE_VALIDATING = 3,
    HOTSWAP_STATE_COMPLETED = 4,
    HOTSWAP_STATE_FAILED = -1
} hotswap_state_t;

// Hotswap Operation Types: Polymorphic Coordination
typedef enum {
    HOTSWAP_OP_NONE = 0,
    HOTSWAP_OP_MODULE_RELOAD = 1,
    HOTSWAP_OP_CONFIG_UPDATE = 2,
    HOTSWAP_OP_TELEMETRY_RECONFIGURE = 3,
    HOTSWAP_OP_BOUNDARY_ADJUSTMENT = 4
} hotswap_operation_t;

// Hotswap Result Type: Comprehensive Error Coordination
typedef enum {
    HOTSWAP_SUCCESS = 0,
    HOTSWAP_ERROR_INVALID_STATE = -1,
    HOTSWAP_ERROR_COORDINATION_FAILED = -2,
    HOTSWAP_ERROR_BOUNDARY_VIOLATION = -3,
    HOTSWAP_ERROR_ENTROPY_EXHAUSTED = -4,
    HOTSWAP_ERROR_VALIDATION_FAILED = -5
} hotswap_result_t;

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H
