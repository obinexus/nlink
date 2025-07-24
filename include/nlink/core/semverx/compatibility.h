// nlink/core/semverx/compatibility.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_SEMVERX_COMPATIBILITY_H
#ifndef OBINEXUS_NLINK_CORE_SEMVERX_COMPATIBILITY_H
#define OBINEXUS_NLINK_CORE_SEMVERX_COMPATIBILITY_H

// include/nlink/core/semverx/compatibility.h
// OBINexus SemVerX Compatibility: Constitutional Interface Declarations Only

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Compatibility Result Enumeration: Constitutional Classification
typedef enum {
    COMPAT_ALLOWED              = 0x01,
    COMPAT_REQUIRES_VALIDATION  = 0x02,
    COMPAT_DENIED               = 0x03
} compatibility_result_t;

// Version Constraint Type Classification
typedef enum {
    VERSION_CONSTRAINT_EXACT     = 0x10,
    VERSION_CONSTRAINT_MINIMUM   = 0x20,
    VERSION_CONSTRAINT_MAXIMUM   = 0x30,
    VERSION_CONSTRAINT_RANGE     = 0x40
} version_constraint_type_t;

// Compatibility Context Structure: Hierarchical Boundary Management
typedef struct compatibility_context {
    uint32_t constitutional_marker;     // 0x434F4D54 ("COMT")
    compatibility_result_t primary_result;
    float compatibility_confidence;
    bool constitutional_validation_enabled;
    uint8_t reserved_space[64];
} compatibility_context_t;

// Public Interface Function Declarations (Implementation in .c files)
compatibility_result_t compatibility_context_init(compatibility_context_t* ctx, uint32_t entropy_seed);
compatibility_result_t validate_version_compatibility(const compatibility_context_t* ctx, 
                                                     const void* component_a, const void* component_b);

#ifdef __cplusplus
}
#endif


#endif // OBINEXUS_NLINK_CORE_SEMVERX_COMPATIBILITY_H
