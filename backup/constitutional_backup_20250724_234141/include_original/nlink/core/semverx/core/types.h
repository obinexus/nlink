// include/nlink/core/semverx/core/types.h
// OBINexus SemVerX Core Types: Constitutional Interface Declarations Only
#ifndef OBINEXUS_NLINK_SEMVERX_CORE_TYPES_H
#define OBINEXUS_NLINK_SEMVERX_CORE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for DAG-protected boundary management
struct compatibility_context;
typedef struct compatibility_context compatibility_context_t;

// SemVerX Range State Enumeration: Constitutional Classification
typedef enum {
    SEMVERX_RANGE_LEGACY        = 0x01,
    SEMVERX_RANGE_STABLE        = 0x02,
    SEMVERX_RANGE_EXPERIMENTAL  = 0x03
} semverx_range_state_t;

// SemVerX Component Structure: Hierarchical Boundary Management
typedef struct semverx_component {
    uint32_t constitutional_marker;     // 0x434F4D50 ("COMP")
    char name[64];
    char version[32];
    char compatible_range[128];
    semverx_range_state_t range_state;
    bool hot_swap_enabled;
    uint64_t component_id;
    uint8_t reserved_space[32];
} semverx_component_t;

// Public Interface Function Declarations (Implementation in .c files)
bool semverx_component_init(semverx_component_t* component, const char* name, 
                           const char* version, semverx_range_state_t range_state);
bool semverx_component_validate(const semverx_component_t* component);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_SEMVERX_CORE_TYPES_H
