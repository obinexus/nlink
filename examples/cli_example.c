/**
 * @file cli_example.c
 * @brief Example of using the enhanced command system with parameter extraction
 * 
 * This example demonstrates how to implement CLI commands that utilize
 * the parameter extraction capabilities for more sophisticated command handling.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/command_router.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/nlink.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // Forward declarations for command handlers
 static NexusResult load_component_handler(NexusContext* ctx, NlinkCommandParams* params);
 static NexusResult stats_handler(NexusContext* ctx, NlinkCommandParams* params);
 static NexusResult config_handler(NexusContext* ctx, NlinkCommandParams* params);
 
 /**
  * @brief Initialize CLI system with parameter-aware commands
  * 
  * @param router Command router
  * @return NexusResult Result code
  */
 NexusResult initialize_cli_example(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     // Create commands with parameter-aware handlers
     static NexusCommandEx load_command = {
         .name = "load",
         .description = "Load a component with specific version",
         .handler = NULL,  // No basic handler, we'll use the parameter-aware one
         .handler_with_params = load_component_handler
     };
     
     static NexusCommandEx stats_command = {
         .name = "stats",
         .description = "Display statistics for system or components",
         .handler = NULL,  // No basic handler, we'll use the parameter-aware one
         .handler_with_params = stats_handler
     };
     
     static NexusCommandEx config_command = {
         .name = "config",
         .description = "Configure system parameters",
         .handler = NULL,  // No basic handler, we'll use the parameter-aware one
         .handler_with_params = config_handler
     };
 
     // Standard command struct for compatibility
     static NexusCommand load_compat = {
         .name = "load",
         .description = "Load a component with specific version",
         .handler = NULL
     };
     
     static NexusCommand stats_compat = {
         .name = "stats",
         .description = "Display statistics for system or components",
         .handler = NULL
     };
     
     static NexusCommand config_compat = {
         .name = "config",
         .description = "Configure system parameters",
         .handler = NULL
     };
     
     // Register load command with regex pattern and parameter names
     // Pattern: "load <component> [version <version>]"
     // This will match patterns like:
     //   "load core"
     //   "load core version 1.0"
     const char* load_pattern = "^load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?$";
     const char* load_params[] = {"component", NULL, "version"};
     
     result = nlink_command_router_register_with_params(
         router,
         load_pattern,
         (NexusCommand*)&load_command,
         NLINK_PATTERN_FLAG_REGEX,
         load_params,
         3
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     // Register stats command with regex pattern and parameter names
     // Pattern: "stats [component <component>] [type <type>]"
     // This will match patterns like:
     //   "stats"
     //   "stats component core"
     //   "stats type memory"
     //   "stats component core type memory"
     const char* stats_pattern = "^stats( component ([a-zA-Z0-9_-]+))?( type ([a-zA-Z0-9_-]+))?$";
     const char* stats_params[] = {NULL, "component", NULL, "type"};
     
     result = nlink_command_router_register_with_params(
         router,
         stats_pattern,
         (NexusCommand*)&stats_command,
         NLINK_PATTERN_FLAG_REGEX,
         stats_params,
         4
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     // Register config command with regex pattern and parameter names
     // Pattern: "config set <key> <value>" or "config get <key>"
     // This will match patterns like:
     //   "config set log-level debug"
     //   "config get log-level"
     const char* config_pattern = "^config (set|get) ([a-zA-Z0-9_-]+)( (.+))?$";
     const char* config_params[] = {"action", "key", NULL, "value"};
     
     result = nlink_command_router_register_with_params(
         router,
         config_pattern,
         (NexusCommand*)&config_command,
         NLINK_PATTERN_FLAG_REGEX,
         config_params,
         4
     );
     
     return result;
 }
 
 /**
  * @brief Example command handler for loading components
  * 
  * @param ctx Execution context
  * @param params Command parameters
  * @return NexusResult Result code
  */
 static NexusResult load_component_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx || !params) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Extract parameters
     const char* component = nlink_command_params_get(params, "component");
     const char* version = nlink_command_params_get(params, "version");
     
     if (!component) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Missing required parameter 'component'");
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Log the operation
     if (version) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Loading component '%s' version '%s'", component, version);
     } else {
         nexus_log(ctx, NEXUS_LOG_INFO, "Loading component '%s' (latest version)", component);
     }
     
     // In a real implementation, we would call the appropriate API
     // For this example, we'll just print some messages
     printf("Loading component: %s\n", component);
     
     if (version) {
         printf("Version: %s\n", version);
     } else {
         printf("Using latest version\n");
     }
     
     // Simulate successful loading
     printf("Component loaded successfully\n");
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Example command handler for statistics
  * 
  * @param ctx Execution context
  * @param params Command parameters
  * @return NexusResult Result code
  */
 static NexusResult stats_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx || !params) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Extract parameters
     const char* component = nlink_command_params_get(params, "component");
     const char* type = nlink_command_params_get(params, "type");
     
     // Log the operation
     if (component && type) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Displaying %s stats for component '%s'", type, component);
     } else if (component) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Displaying all stats for component '%s'", component);
     } else if (type) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Displaying %s stats for all components", type);
     } else {
         nexus_log(ctx, NEXUS_LOG_INFO, "Displaying general system stats");
     }
     
     // In a real implementation, we would collect and display actual statistics
     // For this example, we'll just print some example stats
     
     printf("Statistics:\n");
     printf("===========\n");
     
     if (component) {
         printf("Component: %s\n", component);
         
         if (type && strcmp(type, "memory") == 0) {
             printf("Memory usage: 1024 KB\n");
             printf("Peak memory: 2048 KB\n");
             printf("Allocations: 256\n");
         } else if (type && strcmp(type, "performance") == 0) {
             printf("Processing time: 15.2 ms\n");
             printf("Throughput: 1250 ops/sec\n");
             printf("Latency: 0.8 ms\n");
         } else {
             // Show all stats
             printf("Memory usage: 1024 KB\n");
             printf("Processing time: 15.2 ms\n");
             printf("State count: 64\n");
             printf("Version: 1.2.0\n");
         }
     } else {
         // System-wide stats
         printf("Total components: 8\n");
         printf("Active components: 3\n");
         printf("Total memory usage: 8192 KB\n");
         printf("Uptime: 2h 15m\n");
         
         if (type && strcmp(type, "memory") == 0) {
             printf("\nMemory breakdown:\n");
             printf("  Core: 2048 KB\n");
             printf("  Minimizer: 4096 KB\n");
             printf("  CLI: 1024 KB\n");
             printf("  Other: 1024 KB\n");
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Example command handler for configuration
  * 
  * @param ctx Execution context
  * @param params Command parameters
  * @return NexusResult Result code
  */
 static NexusResult config_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx || !params) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Extract parameters
     const char* action = nlink_command_params_get(params, "action");
     const char* key = nlink_command_params_get(params, "key");
     const char* value = nlink_command_params_get(params, "value");
     
     if (!action || !key) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Missing required parameters");
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Log the operation
     if (strcmp(action, "set") == 0) {
         if (!value) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Missing required parameter 'value' for 'set' action");
             return NEXUS_INVALID_PARAMETER;
         }
         
         nexus_log(ctx, NEXUS_LOG_INFO, "Setting configuration '%s' to '%s'", key, value);
     } else if (strcmp(action, "get") == 0) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Getting configuration '%s'", key);
     } else {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Unknown action '%s'", action);
         return NEXUS_INVALID_PARAMETER;
     }
     
     // In a real implementation, we would manage actual configuration
     // For this example, we'll just simulate a configuration system
     
     // Example configuration handling
     if (strcmp(action, "set") == 0) {
         printf("Setting %s = %s\n", key, value);
         
         // Validate and handle specific settings
         if (strcmp(key, "log-level") == 0) {
             if (strcmp(value, "debug") == 0 ||
                 strcmp(value, "info") == 0 ||
                 strcmp(value, "warning") == 0 ||
                 strcmp(value, "error") == 0) {
                 
                 printf("Log level updated successfully\n");
                 
                 // In a real implementation, we would call the appropriate API
                 // nexus_set_log_level(ctx, convert_log_level(value));
             } else {
                 printf("Invalid log level '%s'\n", value);
                 printf("Valid values: debug, info, warning, error\n");
                 return NEXUS_INVALID_PARAMETER;
             }
         } else if (strcmp(key, "auto-load") == 0) {
             if (strcmp(value, "true") == 0 ||
                 strcmp(value, "false") == 0) {
                 
                 printf("Auto-load setting updated successfully\n");
                 
                 // In a real implementation, we would call the appropriate API
                 // nexus_set_auto_load(ctx, strcmp(value, "true") == 0);
             } else {
                 printf("Invalid value '%s' for auto-load\n", value);
                 printf("Valid values: true, false\n");
                 return NEXUS_INVALID_PARAMETER;
             }
         } else {
             printf("Unknown configuration key '%s'\n", key);
             return NEXUS_NOT_FOUND;
         }
     } else {
         // Handle 'get' action
         if (strcmp(key, "log-level") == 0) {
             printf("%s = info\n", key);
         } else if (strcmp(key, "auto-load") == 0) {
             printf("%s = true\n", key);
         } else if (strcmp(key, "version") == 0) {
             printf("%s = %s\n", key, nlink_get_version());
         } else {
             printf("Unknown configuration key '%s'\n", key);
             return NEXUS_NOT_FOUND;
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Test function to demonstrate CLI command execution
  * 
  * This shows how to use the command router with parameter extraction.
  * 
  * @param ctx Execution context
  * @return NexusResult Result code
  */
 NexusResult test_cli_commands(NexusContext* ctx) {
     // Create command router
     NlinkCommandRouter* router = nlink_command_router_create();
     if (!router) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize with example commands
     NexusResult result = initialize_cli_example(router);
     if (result != NEXUS_SUCCESS) {
         nlink_command_router_destroy(router);
         return result;
     }
     
     // Test various command patterns
     const char* test_commands[] = {
         "load core",
         "load minimizer version 1.2.0",
         "stats",
         "stats component core",
         "stats type memory",
         "stats component minimizer type performance",
         "config get log-level",
         "config set log-level debug",
         "config set auto-load false"
     };
     
     const int command_count = sizeof(test_commands) / sizeof(test_commands[0]);
     
     // Execute each test command
     for (int i = 0; i < command_count; i++) {
         printf("\n=== Executing command: %s ===\n", test_commands[i]);
         
         // Execute with parameter extraction
         NlinkCommandParams* params = NULL;
         result = nlink_command_router_execute_with_params(router, test_commands[i], ctx, &params);
         
         if (result != NEXUS_SUCCESS) {
             printf("Command failed: %s\n", nexus_result_to_string(result));
         }
         
         // Display extracted parameters
         if (params) {
             size_t param_count = nlink_command_params_count(params);
             
             if (param_count > 0) {
                 printf("\nExtracted parameters:\n");
                 
                 for (size_t j = 0; j < param_count; j++) {
                     const char* name;
                     const char* value;
                     
                     if (nlink_command_params_get_at(params, j, &name, &value)) {
                         printf("  %s = %s\n", name, value ? value : "(null)");
                     }
                 }
             }
             
             // Clean up
             nlink_command_params_destroy(params);
         }
         
         printf("\n");
     }
     
     // Clean up
     nlink_command_router_destroy(router);
     
     return NEXUS_SUCCESS;
 }