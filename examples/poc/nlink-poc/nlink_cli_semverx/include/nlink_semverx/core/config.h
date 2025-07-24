/**
 * @file config.h
 * @brief Configuration system for NexusLink SemVerX
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0
 */

#ifndef NLINK_SEMVERX_CORE_CONFIG_H
#define NLINK_SEMVERX_CORE_CONFIG_H

#include "nlink_semverx/core/types.h"

// Configuration result codes (local definition to avoid circular dependencies)
typedef enum {
    NLINK_CONFIG_SUCCESS = 0,
    NLINK_CONFIG_ERROR_FILE_NOT_FOUND = -1,
    NLINK_CONFIG_ERROR_PARSE_FAILED = -2,
    NLINK_CONFIG_ERROR_INVALID_FORMAT = -3,
    NLINK_CONFIG_ERROR_MISSING_REQUIRED_FIELD = -4,
    NLINK_CONFIG_ERROR_THREAD_POOL_INVALID = -5,
    NLINK_CONFIG_ERROR_MEMORY_ALLOCATION = -6,
    NLINK_CONFIG_ERROR_RANGE_STATE_INVALID = -7
} nlink_config_result_t;

// Forward declarations
typedef struct nlink_pkg_config nlink_pkg_config_t;

// Core configuration functions
nlink_config_result_t nlink_config_init(void);
nlink_config_result_t nlink_parse_pkg_config(const char *path, nlink_pkg_config_t *config);
void nlink_config_destroy(void);

#endif /* NLINK_SEMVERX_CORE_CONFIG_H */
