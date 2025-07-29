// nlink/core/parser/nlink_parser_interface.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_PARSER_NLINK_PARSER_INTERFACE_H
#ifndef OBINEXUS_NLINK_CORE_PARSER_NLINK_PARSER_INTERFACE_H
#define OBINEXUS_NLINK_CORE_PARSER_NLINK_PARSER_INTERFACE_H

/**
 * @file parser_interface.h
 * @brief NexusLink CLI Parser Interface (Complete Type Definitions)
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0
 */


#include <stdbool.h>

// CLI result codes
typedef enum {
    NLINK_CLI_SUCCESS = 0,
    NLINK_CLI_ERROR_INVALID_ARGUMENTS = -1,
    NLINK_CLI_ERROR_CONFIG_NOT_FOUND = -2,
    NLINK_CLI_ERROR_PARSE_FAILED = -3,
    NLINK_CLI_ERROR_VALIDATION_FAILED = -4,
    NLINK_CLI_ERROR_THREADING_INVALID = -5,
    NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED = -6,
    NLINK_CLI_ERROR_INTERNAL_ERROR = -7
} nlink_cli_result_t;

// Complete CLI context structure definition
typedef struct nlink_cli_context {
    bool initialized;
    char project_root[512];
    bool verbose_mode;
    bool debug_mode;
} nlink_cli_context_t;

// Complete CLI arguments structure definition
typedef struct nlink_cli_args {
    int argc;
    char **argv;
    bool help_requested;
    bool version_requested;
    bool config_check_requested;
    bool semverx_validate_requested;
    char *project_root_override;
} nlink_cli_args_t;

// CLI interface functions
nlink_cli_result_t nlink_cli_init(nlink_cli_context_t *context);
nlink_cli_result_t nlink_cli_parse_args(int argc, char *argv[], nlink_cli_args_t *args);
nlink_cli_result_t nlink_cli_execute(nlink_cli_context_t *context, nlink_cli_args_t *args);
void nlink_cli_cleanup(nlink_cli_context_t *context);
void nlink_cli_display_help(const char *program_name);


#endif // OBINEXUS_NLINK_CORE_PARSER_NLINK_PARSER_INTERFACE_H
