/**
 * @file main.c
 * @brief NexusLink CLI Main Entry Point (Fixed Type Definitions)
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0
 */

#include "nlink_semverx/nlink_semverx.h"
#include <string.h>

static int cli_result_to_exit_code(nlink_cli_result_t result) {
    switch (result) {
    case NLINK_CLI_SUCCESS: return 0;
    case NLINK_CLI_ERROR_INVALID_ARGUMENTS: return 1;
    case NLINK_CLI_ERROR_CONFIG_NOT_FOUND: return 2;
    case NLINK_CLI_ERROR_PARSE_FAILED: return 3;
    case NLINK_CLI_ERROR_VALIDATION_FAILED: return 4;
    case NLINK_CLI_ERROR_THREADING_INVALID: return 5;
    case NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED: return 6;
    case NLINK_CLI_ERROR_INTERNAL_ERROR: return 7;
    default: return 99;
    }
}

int main(int argc, char *argv[]) {
    // Initialize structures with zero values
    nlink_cli_context_t context = {0};
    nlink_cli_args_t args = {0};

    printf("=== NexusLink CLI v%s - Systematic Architecture ===\n", NLINK_SEMVERX_VERSION_STRING);
    printf("Aegis Project Phase 1.5 - SemVerX Range State Versioning\n");
    printf("Author: Nnamdi Michael Okpala & Development Team\n");
    printf("Architecture: Waterfall Methodology with Systematic Validation\n");
    printf("\n");

    // Initialize configuration system
    nlink_config_result_t config_init = nlink_config_init();
    if (config_init != NLINK_CONFIG_SUCCESS) {
        fprintf(stderr, "[FATAL] Configuration initialization failed: %d\n", config_init);
        return 1;
    }

    // Initialize CLI context
    nlink_cli_result_t init_result = nlink_cli_init(&context);
    if (init_result != NLINK_CLI_SUCCESS) {
        fprintf(stderr, "[FATAL] CLI initialization failed: %d\n", init_result);
        return cli_result_to_exit_code(init_result);
    }

    // Parse command line arguments
    nlink_cli_result_t parse_result = nlink_cli_parse_args(argc, argv, &args);
    if (parse_result != NLINK_CLI_SUCCESS) {
        fprintf(stderr, "[ERROR] Invalid command line arguments\n");
        nlink_cli_display_help(argv[0]);
        return cli_result_to_exit_code(parse_result);
    }

    // Execute CLI command with systematic processing
    nlink_cli_result_t exec_result = nlink_cli_execute(&context, &args);
    
    // Cleanup resources with systematic resource management
    nlink_cli_cleanup(&context);
    nlink_config_destroy();
    
    printf("\n[SYSTEM] NexusLink execution completed with systematic validation\n");
    return cli_result_to_exit_code(exec_result);
}
