/**
 * @file range_state.h
 * @brief SemVerX Range State Definitions (Stub Implementation)
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0
 * 
 * NOTE: This is a stub implementation for compilation compatibility.
 * Full SemVerX implementation will be added in Phase 2.
 */

#ifndef NLINK_SEMVERX_SEMVERX_RANGE_STATE_H
#define NLINK_SEMVERX_SEMVERX_RANGE_STATE_H

// SemVerX range state enumeration (placeholder)
typedef enum {
    SEMVERX_RANGE_STATE_UNKNOWN = 0,
    SEMVERX_RANGE_STATE_LEGACY,
    SEMVERX_RANGE_STATE_STABLE,
    SEMVERX_RANGE_STATE_EXPERIMENTAL
} semverx_range_state_t;

// Forward declarations for future implementation
typedef struct semverx_component_metadata semverx_component_metadata_t;

// Stub functions (to be implemented in Phase 2)
semverx_range_state_t semverx_parse_range_state(const char *state_str);
const char* semverx_range_state_to_string(semverx_range_state_t state);

#endif /* NLINK_SEMVERX_SEMVERX_RANGE_STATE_H */
