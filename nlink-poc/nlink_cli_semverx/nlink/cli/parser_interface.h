/**
 * @file parser_interface.h
 * @brief CLI Parser Interface Header
 */

#ifndef NLINK_CLI_PARSER_INTERFACE_H
#define NLINK_CLI_PARSER_INTERFACE_H

typedef struct nlink_cli_context nlink_cli_context_t;
typedef struct nlink_cli_args nlink_cli_args_t;

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

// Function declarations
nlink_cli_result_t nlink_cli_init(nlink_cli_context_t *context);
nlink_cli_result_t nlink_cli_parse_args(int argc, char *argv[], nlink_cli_args_t *args);
nlink_cli_result_t nlink_cli_execute(nlink_cli_context_t *context, nlink_cli_args_t *args);
void nlink_cli_cleanup(nlink_cli_context_t *context);
void nlink_cli_display_help(const char *program_name);

// Configuration functions
int nlink_config_init(void);
void nlink_config_destroy(void);

#endif /* NLINK_CLI_PARSER_INTERFACE_H */
