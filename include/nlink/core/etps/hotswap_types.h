// include/nlink/core/etps/hotswap_types.h
// OBINexus ETPS: Hotswap Constitutional Types Framework
#ifndef OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H
#define OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H

#include <nlink/core/etps/etps_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Re-export constitutional types from unified ETPS types
typedef hotswap_result_t hotswap_result_t;
typedef hotswap_state_t hotswap_state_t;
typedef hotswap_operation_t hotswap_operation_t;

// Compatibility aliases for legacy code
#define HOTSWAP_SUCCESS HOTSWAP_RESULT_SUCCESS
#define HOTSWAP_ERROR_INVALID_STATE HOTSWAP_RESULT_ERROR_INVALID_STATE
#define HOTSWAP_ERROR_COORDINATION_FAILED HOTSWAP_RESULT_ERROR_COORDINATION_FAILED
#define HOTSWAP_ERROR_BOUNDARY_VIOLATION HOTSWAP_RESULT_ERROR_BOUNDARY_VIOLATION

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_ETPS_HOTSWAP_TYPES_H
