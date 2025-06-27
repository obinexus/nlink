/**
 * @file load_command.c
 * @brief Implementation of the load command with parameter support
 * 
 * Demonstrates how to implement a command that uses parameter extraction
 * for the NexusLink CLI.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/load.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/nlink.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * @brief Static command instance for load command
  */
 static NexusCommandEx load_command = {
     .name = "load",
     .description = "Load a component by name and optionally specify a version",
     .handler = NULL,  // We'll use the parameter-aware handler
     .handler_with_params = NULL  // Set in initialization
 };
 
 /**
  * @brief Handler function for the load command with parameters
  * 
  * @param ctx Execution context
  * @param params Command parameters
  * @return NexusResult Result code
  */
 static NexusResult load_command_handler(NexusContext* ctx, NlinkCommandParams* params) {
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
     
     // Build the component path
     char component_path[256];
     
     // Use version if provided, otherwise use latest
     if (version) {
         snprintf(component_path, sizeof(component_path), "lib%s-%s.so", component, version);
     } else {
         snprintf(component_path, sizeof(component_path), "lib%s.so", component);
     }
     
     // Attempt to load the component
     NexusComponent* loaded_component = nlink_load_component(component_path, component, version);
     
     if (!loaded_component) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load component '%s'", component);
         return NEXUS_IO_ERROR;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Successfully loaded component '%s'", component);
     
     // Note: In a real implementation, we would store the loaded component
     // For this example, we'll just return success
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Initialize the load command
  * 
  * @return NexusCommand* Command instance
  */
 NexusCommand* load_command_init(void) {
     // Set the handler
     load_command.handler_with_params = load_command_handler;
     
     // Return as base command type
     return (NexusCommand*)&load_command;
 }
 
 /**
  * @brief Register the load command with the router
  * 
  * @param router Command router
  * @return NexusResult Result code
  */
 NexusResult load_command_register(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Define the pattern and parameter names
     const char* pattern = "^load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?$";
     const char* param_names[] = {"component", NULL, "version"};
     
     // Register the command with parameter names
     return nlink_command_router_register_with_params(
         router,
         pattern,
         (NexusCommand*)&load_command,
         NLINK_PATTERN_FLAG_REGEX,
         param_names,
         3
     );
 }