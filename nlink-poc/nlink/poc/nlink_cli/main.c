/**
 * @file main.c
 * @brief NexusLink CLI Main Entry Point
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 */

#include "nlink/cli/parser_interface.h"
#include <stdio.h>
#include <stdlib.h>

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
  nlink_cli_context_t context;
  nlink_cli_args_t args;

  nlink_cli_result_t init_result = nlink_cli_init(&context);
  if (init_result != NLINK_CLI_SUCCESS) {
    fprintf(stderr, "[NLINK FATAL] Failed to initialize CLI context: %d\n", init_result);
    return cli_result_to_exit_code(init_result);
  }

  nlink_cli_result_t parse_result = nlink_cli_parse_args(argc, argv, &args);
  if (parse_result != NLINK_CLI_SUCCESS) {
    fprintf(stderr, "[NLINK ERROR] Invalid command line arguments\n");
    nlink_cli_display_help(argv[0]);
    return cli_result_to_exit_code(parse_result);
  }

  nlink_cli_result_t exec_result = nlink_cli_execute(&context, &args);
  
  nlink_cli_cleanup(&context);
  nlink_config_destroy();
  
  return cli_result_to_exit_code(exec_result);
}
