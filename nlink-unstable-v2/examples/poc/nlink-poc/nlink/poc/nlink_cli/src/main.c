/**
 * @file main.c
 * @brief NexusLink CLI Main Entry Point
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Main entry point for NexusLink CLI executable.
 * Separated from library code for proper library architecture.
 * Enhanced with systematic error handling and defensive programming practices.
 */

#include "nlink/cli/parser_interface.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Convert CLI result to appropriate exit code
 * @param result CLI operation result code from nlink_cli_result_t enum
 * @return int Exit code where 0 indicates success, 1-7 for specific errors, 
 *             and 99 for unknown errors
 */
static int cli_result_to_exit_code(nlink_cli_result_t result) {
  switch (result) {
  case NLINK_CLI_SUCCESS:
    return 0;
  case NLINK_CLI_ERROR_INVALID_ARGUMENTS:
    return 1;
  case NLINK_CLI_ERROR_CONFIG_NOT_FOUND:
    return 2;
  case NLINK_CLI_ERROR_PARSE_FAILED:
    return 3;
  case NLINK_CLI_ERROR_VALIDATION_FAILED:
    return 4;
  case NLINK_CLI_ERROR_THREADING_INVALID:
    return 5;
  case NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED:
    return 6;
  case NLINK_CLI_ERROR_INTERNAL_ERROR:
    return 7;
  default:
    return 99;
  }
}

/**
 * @brief Enhanced error message generation with context
 * @param result CLI result code for context-aware messaging
 * @return Human-readable error description
 */
static const char* get_error_description(nlink_cli_result_t result) {
  switch (result) {
  case NLINK_CLI_ERROR_INVALID_ARGUMENTS:
    return "Invalid command line arguments or format";
  case NLINK_CLI_ERROR_CONFIG_NOT_FOUND:
    return "Configuration file not found in project directory";
  case NLINK_CLI_ERROR_PARSE_FAILED:
    return "Configuration parsing failed due to syntax errors";
  case NLINK_CLI_ERROR_VALIDATION_FAILED:
    return "Configuration validation failed - check constraints";
  case NLINK_CLI_ERROR_THREADING_INVALID:
    return "Threading configuration parameters are invalid";
  case NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED:
    return "Component discovery failed - check project structure";
  case NLINK_CLI_ERROR_INTERNAL_ERROR:
    return "Internal system error occurred";
  default:
    return "Unknown error condition";
  }
}

/**
 * @brief Main entry point for nlink CLI executable
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code following systematic error propagation
 */
int main(int argc, char *argv[]) {
  // Enhanced input validation with defensive programming
  if (argv == NULL) {
    fprintf(stderr, "[NLINK FATAL] Invalid argument vector - system error\n");
    return cli_result_to_exit_code(NLINK_CLI_ERROR_INVALID_ARGUMENTS);
  }

  if (argc < 1 || argv[0] == NULL) {
    fprintf(stderr, "[NLINK FATAL] Invalid program invocation\n");
    return cli_result_to_exit_code(NLINK_CLI_ERROR_INVALID_ARGUMENTS);
  }

  nlink_cli_context_t context;
  nlink_cli_args_t args;

  // Initialize core configuration system with systematic error handling
  nlink_config_result_t config_init_result = nlink_config_init();
  if (config_init_result != NLINK_CONFIG_SUCCESS) {
    fprintf(stderr, "[NLINK FATAL] Failed to initialize configuration system: %d\n",
            config_init_result);
    fprintf(stderr, "[NLINK FATAL] System may lack required resources or permissions\n");
    return cli_result_to_exit_code(NLINK_CLI_ERROR_INTERNAL_ERROR);
  }

  // Initialize CLI context with systematic error handling
  nlink_cli_result_t init_result = nlink_cli_init(&context);
  if (init_result != NLINK_CLI_SUCCESS) {
    fprintf(stderr, "[NLINK FATAL] Failed to initialize CLI context: %d\n",
            init_result);
    fprintf(stderr, "[NLINK FATAL] Description: %s\n", get_error_description(init_result));
    
    // Cleanup configuration system before exit
    nlink_config_destroy();
    return cli_result_to_exit_code(init_result);
  }

  // Parse command line arguments with comprehensive validation
  nlink_cli_result_t parse_result = nlink_cli_parse_args(argc, argv, &args);
  if (parse_result != NLINK_CLI_SUCCESS) {
    fprintf(stderr, "[NLINK ERROR] %s\n", get_error_description(parse_result));
    
    // Display help for argument-related errors
    if (parse_result == NLINK_CLI_ERROR_INVALID_ARGUMENTS) {
      fprintf(stderr, "[NLINK INFO] Use --help for usage information\n");
      nlink_cli_display_help(argv[0]);
    }
    
    // Systematic cleanup before exit
    nlink_cli_cleanup(&context);
    nlink_config_destroy();
    return cli_result_to_exit_code(parse_result);
  }

  // Execute CLI command with systematic error propagation
  nlink_cli_result_t exec_result = nlink_cli_execute(&context, &args);
  
  // Enhanced error reporting for execution failures
  if (exec_result != NLINK_CLI_SUCCESS) {
    fprintf(stderr, "[NLINK ERROR] Command execution failed: %s\n", 
            get_error_description(exec_result));
  }

  // Clean up resources with systematic resource management
  nlink_cli_cleanup(&context);
  nlink_config_destroy();

  return cli_result_to_exit_code(exec_result);
}