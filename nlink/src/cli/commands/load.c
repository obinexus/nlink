/**
 * @file load.c
 * @brief Implementation of the load command
 * 
 * Provides functionality to load NexusLink components dynamically.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/load.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command_registration.h"  // Include the central registration header
 #include "nlink/core/common/nexus_loader.h"
 #include "nlink/core/common/result.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // Forward declarations
 static int load_command_handler(NexusContext* ctx, int argc, char** argv);
 static NexusResult load_with_params_handler(NexusContext* ctx, NlinkCommandParams* params);
 static void load_command_print_help(void);
 
 /**
  * @brief Load command definition
  */
 NexusCommand load_command = {
     .name = "load",
     .short_name = "l",
     .description = "Load a component dynamically",
     .help = "Usage: load <component_name> [version <version>] [path <path>]\n\n"
             "Options:\n"
             "  version <version>   Specific version to load\n"
             "  path <path>         Custom path to load from\n",
     .handler = NULL,
     .handler_with_params = load_with_params_handler,
     .parse_args = NULL,
     .print_help = load_command_print_help,
     .execute = load_command_handler,
     .data = NULL
 };
 
 /**
  * @brief Print help for the load command
  */
 static void load_command_print_help(void) {
     printf("%s\n", load_command.help);
     printf("Examples:\n");
     printf("  load core                     # Load the core component\n");
     printf("  load minimizer version 1.2.3  # Load minimizer version 1.2.3\n");
     printf("  load logger path /custom/path # Load from custom path\n");
 }
 
 /**
  * @brief Handler for load command with standard arguments
  * 
  * @param ctx NexusLink context
  * @param argc Argument count
  * @param argv Argument values
  * @return int Status code
  */
 static int load_command_handler(NexusContext* ctx, int argc, char** argv) {
     if (argc < 1) {
         fprintf(stderr, "Error: Component name required\n");
         load_command_print_help();
         return 1;
     }
     
     const char* component_name = argv[0];
     const char* version = NULL;
     const char* path = NULL;
     
     // Parse options
     for (int i = 1; i < argc - 1; i++) {
         if (strcmp(argv[i], "version") == 0 && i + 1 < argc) {
             version = argv[i + 1];
             i++; // Skip the version value
         } else if (strcmp(argv[i], "path") == 0 && i + 1 < argc) {
             path = argv[i + 1];
             i++; // Skip the path value
         } else {
             fprintf(stderr, "Warning: Unknown option '%s'\n", argv[i]);
         }
     }
     
     // Log the load attempt
     if (version) {
         printf("Loading component '%s' version '%s'...\n", component_name, version);
     } else {
         printf("Loading component '%s'...\n", component_name);
     }
     
     // In a real implementation, we would use the NexusLink API to load the component
     // For this implementation, just simulate loading
     
     // Simulate component loading
     NexusComponent* component = NULL;
     
     if (path) {
         printf("Loading from custom path: %s\n", path);
         // Simulate loading from custom path
         component = nexus_load_component(ctx, path, component_name);
     } else {
         // Simulate loading from default path with version handling
         char default_path[256];
         if (version) {
             snprintf(default_path, sizeof(default_path), "/usr/local/lib/nlink/%s-%s.so", 
                     component_name, version);
         } else {
             snprintf(default_path, sizeof(default_path), "/usr/local/lib/nlink/%s.so", 
                     component_name);
         }
         
         component = nexus_load_component(ctx, default_path, component_name);
     }
     
     if (component) {
         printf("Successfully loaded component '%s'\n", component_name);
         return 0;
     } else {
         fprintf(stderr, "Error: Failed to load component '%s'\n", component_name);
         return 1;
     }
 }
 
 /**
  * @brief Execute function for the load command (public API)
  * 
  * @param ctx NexusLink context
  * @param argc Argument count
  * @param argv Argument values
  * @return int Status code
  */
 int load_command_execute(NexusContext* ctx, int argc, char** argv) {
     return load_command_handler(ctx, argc, argv);
 }
 
 /**
  * @brief Handler for load command with parameter extraction
  * 
  * @param ctx NexusLink context
  * @param params Extracted parameters
  * @return NexusResult Result code
  */
 static NexusResult load_with_params_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx || !params) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Get required component parameter
     const char* component = nlink_command_params_get(params, "component");
     if (!component) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Component name required");
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Get optional parameters
     const char* version = nlink_command_params_get(params, "version");
     const char* path = nlink_command_params_get(params, "path");
     
     // Log the load attempt
     if (version) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Loading component '%s' version '%s'", component, version);
     } else {
         nexus_log(ctx, NEXUS_LOG_INFO, "Loading component '%s'", component);
     }
     
     // Simulate component loading
     NexusComponent* loaded_component = NULL;
     
     if (path) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Loading from custom path: %s", path);
         loaded_component = nexus_load_component(ctx, path, component);
     } else {
         // Simulate loading from default path with version handling
         char default_path[256];
         if (version) {
             snprintf(default_path, sizeof(default_path), "/usr/local/lib/nlink/%s-%s.so", 
                     component, version);
         } else {
             snprintf(default_path, sizeof(default_path), "/usr/local/lib/nlink/%s.so", 
                     component);
         }
         
         loaded_component = nexus_load_component(ctx, default_path, component);
     }
     if (loaded_component) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Successfully loaded component '%s'", component);
         return NEXUS_SUCCESS;
     } else {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load component '%s'", component);
         return NEXUS_IO_ERROR;
     }
 }
 
 /**
  * @brief Get the load command structure
  * 
  * @return NexusCommand* Pointer to the command structure
  */
 NexusCommand* nlink_get_load_command(void) {
     return &load_command;
 }