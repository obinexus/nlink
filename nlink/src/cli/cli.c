/**
 * @file cli.c
 * @brief Unified CLI implementation for NexusLink
 * 
 * Provides a command-line interface for interacting with NexusLink
 * functionality using pattern-based command routing.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/cli.h"
 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command_registry.h"
 #include "nlink/cli/command_registration.h"
 #include "nlink/cli/commands/load.h"
 #include "nlink/cli/commands/minimal.h"
 #include "nlink/cli/commands/version.h"
 #include "nlink/cli/commands/minimize.h"
 #include "nlink/cli/commands/pipeline.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/nlink.h"
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 // Global command router for pattern-based routing
 static NlinkCommandRouter* g_command_router = NULL;
 
 // Forward declarations for standard commands
 extern NexusCommand load_command;
 extern NexusCommand stats_command;
 extern NexusCommand version_command;
 extern NexusCommand minimal_command;
 extern NexusCommand minimize_command;
 extern NexusCommand pipeline_command;
 
 // Forward declarations for command handlers
 static NexusResult help_command_handler(NexusContext* ctx);
 static NexusResult list_command_handler(NexusContext* ctx);
 static NexusResult stats_command_handler(NexusContext* ctx);
 
 // Basic command definitions
 static NexusCommand help_command = {
     .name = "help",
     .description = "Display help information",
     .handler = help_command_handler,
     .handler_with_params = NULL
 };
 
 static NexusCommand list_command = {
     .name = "list",
     .description = "List available components",
     .handler = list_command_handler,
     .handler_with_params = NULL
 };
 
 static NexusCommand* g_basic_commands[] = {
     &help_command,
     &list_command,
     &stats_command
 };
 
 // Number of basic commands
 static const size_t g_basic_command_count = sizeof(g_basic_commands) / sizeof(g_basic_commands[0]);
 
 /**
  * @brief Initialize the CLI subsystem
  */
 bool nexus_cli_init(NexusCLI* cli, const char* prog_name, NexusContext* context) {
     if (!cli || !prog_name) {
         return false;
     }
     
     // Initialize command registry
     if (!cli_command_registry_init(&cli->registry)) {
         return false;
     }
     
     // Store program name
     cli->prog_name = strdup(prog_name);
     if (!cli->prog_name) {
         cli_command_registry_cleanup(&cli->registry);
         return false;
     }
     
     // Set up context
     if (context) {
         cli->context = context;
     } else {
         // Create a new context with default configuration
         NexusConfig config = {0};
         config.log_level = NEXUS_LOG_INFO;
         config.flags = NEXUS_FLAG_NONE;
         
         cli->context = nexus_create_context(&config);
         if (!cli->context) {
             free((void*)cli->prog_name);
             cli_command_registry_cleanup(&cli->registry);
             return false;
         }
     }
     
     // Create command router for pattern matching
     g_command_router = nlink_command_router_create();
     if (!g_command_router) {
         free((void*)cli->prog_name);
         if (!context) nexus_destroy_context(cli->context);
         cli_command_registry_cleanup(&cli->registry);
         return false;
     }
     
     // Set version
     cli->version = "1.0.0";
     
     // Disable minimal mode by default
     cli->minimal_mode_enabled = false;
     
     // Register commands
     if (!nexus_cli_register_standard_commands(cli)) {
         nexus_cli_cleanup(cli);
         return false;
     }
     
     // Register patterns with the router
     if (register_all_commands(g_command_router) != NEXUS_SUCCESS) {
         nexus_cli_cleanup(cli);
         return false;
     }
     
     return true;
 }
 
 /**
  * @brief Register standard commands with the CLI
  */
 bool nexus_cli_register_standard_commands(NexusCLI* cli) {
     if (!cli) {
         return false;
     }
     
     // Register basic commands first
     for (size_t i = 0; i < g_basic_command_count; i++) {
         if (!cli_command_registry_register(&cli->registry, g_basic_commands[i])) {
             return false;
         }
     }
     
     // Register main commands
     if (!cli_command_registry_register(&cli->registry, &load_command) ||
         !cli_command_registry_register(&cli->registry, &stats_command) ||
         !cli_command_registry_register(&cli->registry, &version_command) ||
         !cli_command_registry_register(&cli->registry, &minimal_command) ||
         !cli_command_registry_register(&cli->registry, &minimize_command) ||
         !cli_command_registry_register(&cli->registry, &pipeline_command)) {
         return false;
     }
     
     return true;
 }
 
 /**
  * @brief Parse command-line arguments and execute the command
  */
 int nexus_cli_parse_and_execute(NexusCLI* cli, int argc, char** argv) {
     if (!cli || argc < 1 || !argv) {
         return 1;
     }
     
     // If no command specified, print help
     if (argc == 1) {
         nexus_cli_print_help(cli);
         return 0;
     }
     
     // Check for global options
     if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
         nexus_cli_print_help(cli);
         return 0;
     }
     
     if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
         printf("%s version %s\n", cli->prog_name, cli->version);
         return 0;
     }
     
     if (strcmp(argv[1], "--minimal") == 0 || strcmp(argv[1], "-m") == 0) {
         if (argc < 3) {
             fprintf(stderr, "Error: Minimal mode requires a command string\n");
             return 1;
         }
         return nexus_cli_parse_minimal(cli, argv[2]) ? 0 : 1;
     }
     
     // Check for interactive mode
     if (strcmp(argv[1], "--interactive") == 0 || strcmp(argv[1], "-i") == 0) {
         return (nexus_cli_run_interactive(cli) == NEXUS_SUCCESS) ? 0 : 1;
     }
     
     // Check for script execution
     if (strcmp(argv[1], "--execute") == 0 || strcmp(argv[1], "-e") == 0) {
         if (argc < 3) {
             fprintf(stderr, "Error: Script execution requires a script file\n");
             return 1;
         }
         return (nexus_cli_execute_script(cli, argv[2]) == NEXUS_SUCCESS) ? 0 : 1;
     }
     
     // Check for minimal mode pattern: name@version:function
     if (cli->minimal_mode_enabled) {
         const char* arg = argv[1];
         if (strchr(arg, '@') || strchr(arg, ':')) {
             return nexus_cli_parse_minimal(cli, arg) ? 0 : 1;
         }
     }
     
     // Build command string from arguments
     char command[1024] = {0};
     for (int i = 1; i < argc; i++) {
         if (i > 1) strcat(command, " ");
         strcat(command, argv[i]);
     }
     
     // Execute the command
     NexusResult result = nexus_cli_execute(cli, command);
     return (result == NEXUS_SUCCESS) ? 0 : 1;
 }
 
 /**
  * @brief Execute a CLI command
  */
 NexusResult nexus_cli_execute(NexusCLI* cli, const char* command_string) {
     if (!cli || !command_string) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     if (!g_command_router) {
         fprintf(stderr, "Error: CLI system not initialized\n");
         return NEXUS_NOT_INITIALIZED;
     }
     
     // Skip leading whitespace
     while (*command_string == ' ' || *command_string == '\t') {
         command_string++;
     }
     
     // Skip if empty or comment
     if (*command_string == '\0' || *command_string == '#') {
         return NEXUS_SUCCESS;
     }
     
     // Log the command
     nexus_log(cli->context, NEXUS_LOG_INFO, "Executing command: %s", command_string);
     
     // Execute the command with parameter extraction
     NlinkCommandParams* params = NULL;
     NexusResult result = nlink_command_router_execute_with_params(
         g_command_router, 
         command_string, 
         cli->context,
         &params
     );
     
     // Clean up parameters if they were extracted
     if (params) {
         nlink_command_params_destroy(params);
     }
     
     return result;
 }
 
 /**
  * @brief Run the CLI in interactive mode
  */
 NexusResult nexus_cli_run_interactive(NexusCLI* cli) {
     if (!cli) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     if (!g_command_router) {
         fprintf(stderr, "Error: CLI system not initialized\n");
         return NEXUS_NOT_INITIALIZED;
     }
     
     char input[256];
     
     // Print banner
     nexus_cli_print_banner();
     printf("Type 'help' for available commands, 'exit' to quit\n");
     
     while (1) {
         printf("\nnexus> ");
         fflush(stdout);
         
         if (!fgets(input, sizeof(input), stdin)) {
             break;
         }
         
         // Remove trailing newline
         size_t len = strlen(input);
         if (len > 0 && input[len - 1] == '\n') {
             input[len - 1] = '\0';
         }
         
         // Check for exit
         if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
             break;
         }
         
         // Execute the command
         NexusResult result = nexus_cli_execute(cli, input);
         if (result != NEXUS_SUCCESS) {
             printf("Error: %s\n", nexus_result_to_string(result));
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Parse and execute a command in minimal syntax
  */
 bool nexus_cli_parse_minimal(NexusCLI* cli, const char* input) {
     if (!cli || !input) {
         return false;
     }
     
     // Find the minimal command
     NexusCommand* minimal_cmd = cli_command_registry_find(&cli->registry, "minimal");
     if (!minimal_cmd) {
         fprintf(stderr, "Error: Minimal mode command not registered\n");
         return false;
     }
     
     // Prepare arguments for the minimal command
     char* args[] = {(char*)input};
     
     // Execute the minimal command
     return minimal_cmd->execute(cli->context, 1, args) == 0;
 }
 
 /**
  * @brief Execute a script file
  */
 NexusResult nexus_cli_execute_script(NexusCLI* cli, const char* filename) {
     if (!cli || !filename) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     FILE* script_file = fopen(filename, "r");
     if (!script_file) {
         fprintf(stderr, "Error: Failed to open script file: %s\n", filename);
         return NEXUS_NOT_FOUND;
     }
     
     char line[256];
     unsigned int line_number = 0;
     NexusResult result = NEXUS_SUCCESS;
     
     while (fgets(line, sizeof(line), script_file)) {
         line_number++;
         
         // Remove trailing newline
         size_t len = strlen(line);
         if (len > 0 && line[len - 1] == '\n') {
             line[len - 1] = '\0';
         }
         
         // Skip empty lines and comments
         if (line[0] == '\0' || line[0] == '#') {
             continue;
         }
         
         // Execute the command
         result = nexus_cli_execute(cli, line);
         if (result != NEXUS_SUCCESS) {
             fprintf(stderr, "Error on line %u: %s\n", 
                     line_number, nexus_result_to_string(result));
             break;
         }
     }
     
     fclose(script_file);
     return result;
 }
 
 /**
  * @brief Print the CLI banner
  */
 void nexus_cli_print_banner(void) {
     printf("*******************************************\n");
     printf("*         NexusLink CLI System           *\n");
     printf("*          © OBINexus Computing          *\n");
     printf("*******************************************\n");
 }
 
 /**
  * @brief Print the CLI help message
  */
 void nexus_cli_print_help(NexusCLI* cli) {
     if (!cli) {
         return;
     }
     
     printf("Usage: %s [OPTIONS] COMMAND [ARGS...]\n\n", cli->prog_name);
     printf("A dynamic component linkage system for efficient binary sizes and runtime loading.\n\n");
     
     printf("Options:\n");
     printf("  -h, --help           Show this help message\n");
     printf("  -v, --version        Show version information\n");
     printf("  -m, --minimal CMD    Use minimal syntax mode\n");
     printf("  -i, --interactive    Run in interactive mode\n");
     printf("  -e, --execute FILE   Execute commands from script file\n\n");
     
     printf("Commands:\n");
     for (size_t i = 0; i < cli->registry.count; i++) {
         NexusCommand* cmd = cli->registry.commands[i];
         printf("  %-15s %s\n", cmd->name, cmd->description);
     }
     
     printf("\nFor more information on a command, run '%s COMMAND --help'\n", cli->prog_name);
     
     if (cli->minimal_mode_enabled) {
         printf("\nMinimal Mode Syntax:\n");
         printf("  %s name@version:function [args...]\n", cli->prog_name);
         printf("  Example: %s logger@1.2.3:log_message \"Hello World\"\n", cli->prog_name);
     }
 }
 
 /**
  * @brief Enable or disable minimal mode
  */
 void nexus_cli_set_minimal_mode(NexusCLI* cli, bool enabled) {
     if (!cli) {
         return;
     }
     
     cli->minimal_mode_enabled = enabled;
 }
 
 /**
  * @brief Clean up CLI resources
  */
 void nexus_cli_cleanup(NexusCLI* cli) {
     if (!cli) {
         return;
     }
     
     // Clean up command router
     if (g_command_router) {
         nlink_command_router_destroy(g_command_router);
         g_command_router = NULL;
     }
     
     // Clean up registry
     cli_command_registry_cleanup(&cli->registry);
     
     // Clean up context if we created it
     if (cli->context) {
         nexus_destroy_context(cli->context);
         cli->context = NULL;
     }
     
     // Free program name
     if (cli->prog_name) {
         free((void*)cli->prog_name);
         cli->prog_name = NULL;
     }
 }
 
 /*
  * Command handler implementations
  */
 
 static NexusResult help_command_handler(NexusContext* ctx) {
     printf("Available Commands:\n");
     printf("------------------\n\n");
     
     // Get all commands
     NexusCommand* commands[20]; // Assuming max 20 commands
     size_t count = cli_command_registry_get_all_commands(commands, 20);
     
     for (size_t i = 0; i < count; i++) {
         printf("%-15s - %s\n", commands[i]->name, commands[i]->description);
     }
     
     printf("\nUsage Examples:\n");
     printf("  load core                     - Load the core component\n");
     printf("  load minimizer version 1.2.3  - Load minimizer version 1.2.3\n");
     printf("  minimize automaton           - Minimize automaton component\n");
     printf("  pipeline create              - Create a new pipeline\n");
     printf("  pipeline add-stage tokenizer - Add a stage to the pipeline\n");
     printf("  pipeline execute             - Execute the pipeline\n");
     printf("  stats memory                 - Show memory statistics\n");
     printf("  version                      - Show version information\n");
     printf("  help                         - Show this help\n");
     printf("  exit                         - Exit the CLI\n");
     
     printf("\nMinimal Syntax:\n");
     printf("  component[@version][:function] - Quick component loading and function calls\n");
     printf("    core                - Load core component\n");
     printf("    minimizer@1.2       - Load minimizer version 1.2\n");
     printf("    logger:log          - Load logger and call log function\n");
     
     return NEXUS_SUCCESS;
 }
 
 static NexusResult list_command_handler(NexusContext* ctx) {
     // List available components
     printf("Available Components:\n");
     printf("-------------------\n");
     printf("  core       - NexusLink Core Library (v1.0.0)\n");
     printf("  minimizer  - Binary Size Optimizer (v1.2.3)\n");
     printf("  logger     - Logging Subsystem (v0.9.1)\n");
     printf("  network    - Network Communication Module (v2.0.0)\n");
     printf("  cli        - Command Line Interface (v1.0.0)\n");
     printf("  pipeline   - Pipeline Processing System (v1.0.0)\n");
     
     return NEXUS_SUCCESS;
 }
 
 static NexusResult stats_command_handler(NexusContext* ctx) {
     // Show system statistics
     printf("System Statistics:\n");
     printf("-----------------\n");
     printf("  Components loaded: 4\n");
     printf("  Memory usage: 1.2 MB\n");
     printf("  Heap allocations: 128\n");
     printf("  Peak memory: 2.4 MB\n");
     printf("  Symbol table entries: 478\n");
     printf("  Commands registered: %zu\n", g_basic_command_count);
     printf("  Pipelines active: 1\n");
     
     return NEXUS_SUCCESS;
 }