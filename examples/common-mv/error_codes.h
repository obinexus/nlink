/**
 * @file error_codes.h
 * @brief Core Error Code Definitions for NexusLink SemVerX
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0
 * 
 * NOTE: CLI result codes moved to cli/parser_interface.h
 * to prevent duplicate declarations and maintain separation of concerns.
 */

#ifndef NLINK_SEMVERX_CORE_ERROR_CODES_H
#define NLINK_SEMVERX_CORE_ERROR_CODES_H

// Configuration result codes (Core system only)
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

// Note: CLI result codes (nlink_cli_result_t) are defined in 
// include/nlink_semverx/cli/parser_interface.h to maintain
// systematic separation of concerns and prevent redeclaration conflicts.

#endif /* NLINK_SEMVERX_CORE_ERROR_CODES_H */
