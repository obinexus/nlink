/**
 * @file cli.h
 * @brief Unified CLI interface for NexusLink
 * 
 * Provides the main CLI functionality for NexusLink, including command
 * registration, execution, interactive mode, and minimal syntax support.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CLI_H
 #define NLINK_CLI_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command.h"
 #include "nlink/cli/command_registry.h"
 #include <stddef.h>
 #include <stdbool.h>
#include "nlink/core/symbols/nexus_symbols.h"
#include "nlink/core/symbols/registry.h"
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief CLI context structure
  */
 typedef struct {
     const char* prog_name;         /**< Program name */
     const char* version;           /**< CLI version string */
     NexusContext* context;         /**< NexusLink context */
     CliCommandRegistry registry;   /**< Command registry */
     bool minimal_mode_enabled;     /**< Whether minimal mode is enabled */
 } NexusCLI;
 
 /**
  * @brief Initialize the CLI subsystem
  * 
  * @param cli CLI context to initialize
  * @param prog_name Program name
  * @param context NexusLink context (if NULL, creates a new one)
  * @return bool True on success, false on failure
  */
 bool nexus_cli_init(NexusCLI* cli, const char* prog_name, NexusContext* context);
 
 /**
  * @brief Parse command-line arguments and execute the command
  * 
  * @param cli CLI context
  * @param argc Argument count
  * @param argv Argument values
  * @return int Exit code (0 for success)
  */
 int nexus_cli_parse_and_execute(NexusCLI* cli, int argc, char** argv);
 
 /**
  * @brief Execute a command string
  * 
  * @param cli CLI context
  * @param command_string Command string to execute
  * @return NexusResult Result code
  */
 NexusResult nexus_cli_execute(NexusCLI* cli, const char* command_string);
 
 /**
  * @brief Run the CLI in interactive mode
  * 
  * @param cli CLI context
  * @return NexusResult Result code
  */
 NexusResult nexus_cli_run_interactive(NexusCLI* cli);
 
 /**
  * @brief Print the CLI help message
  * 
  * @param cli CLI context
  */
 void nexus_cli_print_help(NexusCLI* cli);
 
 /**
  * @brief Print the CLI banner
  */
 void nexus_cli_print_banner(void);
 
 /**
  * @brief Enable or disable minimal mode
  * 
  * @param cli CLI context
  * @param enabled Whether minimal mode should be enabled
  */
 void nexus_cli_set_minimal_mode(NexusCLI* cli, bool enabled);
 
 /**
  * @brief Parse and execute a command in minimal syntax
  * 
  * @param cli CLI context
  * @param input Minimal command string
  * @return bool True on success, false on failure
  */
 bool nexus_cli_parse_minimal(NexusCLI* cli, const char* input);
 
 /**
  * @brief Register standard commands with the CLI
  * 
  * @param cli CLI context
  * @return bool True on success, false on failure
  */
 bool nexus_cli_register_standard_commands(NexusCLI* cli);
 
 /**
  * @brief Execute a script file
  * 
  * @param cli CLI context
  * @param filename Script file to execute
  * @return NexusResult Result code
  */
 NexusResult nexus_cli_execute_script(NexusCLI* cli, const char* filename);
 
 /**
  * @brief Clean up CLI resources
  * 
  * @param cli CLI context
  */
 void nexus_cli_cleanup(NexusCLI* cli);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CLI_H */