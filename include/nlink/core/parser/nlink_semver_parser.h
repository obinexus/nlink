// nlink/core/parser/nlink_semver_parser.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_PARSER_NLINK_SEMVER_PARSER_H
#ifndef OBINEXUS_NLINK_CORE_PARSER_NLINK_SEMVER_PARSER_H
#define OBINEXUS_NLINK_CORE_PARSER_NLINK_SEMVER_PARSER_H

/**
 * @file semverx_parser.h
 * @brief SemVerX Range State Versioning Parser
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    SEMVERX_STATE_LEGACY,
    SEMVERX_STATE_STABLE,
    SEMVERX_STATE_EXPERIMENTAL,
    SEMVERX_STATE_INVALID
} semverx_range_state_t;

typedef struct {
    char version[64];
    semverx_range_state_t state;
    char compatible_range[256];
    char **swappable_with;
    size_t swappable_count;
    bool hot_swap_enabled;
    bool runtime_validation;
    bool requires_opt_in;
} semverx_component_t;

typedef struct {
    bool allow_stable_swap;
    bool allow_experimental_swap;
    bool allow_legacy_use;
    char exclusion_patterns[512];
} semverx_policy_t;

// Core SemVerX functions
semverx_range_state_t semverx_parse_range_state(const char *state_str);
bool semverx_validate_compatibility(const semverx_component_t *comp1, 
                                   const semverx_component_t *comp2,
                                   const semverx_policy_t *policy);
int semverx_parse_component_config(const char *config_path, 
                                  semverx_component_t *component);
int semverx_validate_project_compatibility(const char *project_root);


#endif // OBINEXUS_NLINK_CORE_PARSER_NLINK_SEMVER_PARSER_H
