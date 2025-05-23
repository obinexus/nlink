/**
 * @file version.c
 * @brief Implementation of version command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/version.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /* Version information */
 #define NEXUSLINK_VERSION "1.0.0"
 #define NEXUSLINK_BUILD_DATE "April 2025"
 #define NEXUSLINK_COPYRIGHT "Copyright © 2025 OBINexus Computing"
 
 /**
  * @brief Structure for version command data
  */
 typedef struct {
     bool show_detailed; /* Whether to show detailed version information */
     bool show_json;     /* Whether to output in JSON format */
 } VersionCommandData;
 
 /* Forward declarations for function prototypes */
 static NexusResult version_handler(NexusContext* ctx);
 static NexusResult version_handler_with_params(NexusContext* ctx, NlinkCommandParams* params);
 static void version_free_data(void* command_data);
 static void version_print_help(void);
 
 /**
  * @brief Handler for version command
  */
 static NexusResult version_handler(NexusContext* ctx) {
     /* In a real implementation, this would print version information */
     /* For simplicity, we'll just print basic version info */
     
     printf("NexusLink version %s (%s)\n", NEXUSLINK_VERSION, NEXUSLINK_BUILD_DATE);
     printf("%s\n", NEXUSLINK_COPYRIGHT);
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Handler for version command with parameters
  */
 static NexusResult version_handler_with_params(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Get command data */
     VersionCommandData* data = (VersionCommandData*)version_command.data;
     
     /* Get parameters */
     bool show_detailed = false;
     bool show_json = false;
     
     if (data) {
         show_detailed = data->show_detailed;
         show_json = data->show_json;
     }
     
     /* Check for parameters */
     if (params) {
         if (nlink_command_params_has(params, "detailed")) {
             show_detailed = true;
         }
         
         if (nlink_command_params_has(params, "json")) {
             show_json = true;
         }
     }
     
     if (show_json) {
         /* Output in JSON format */
         printf("{\n");
         printf("  \"name\": \"NexusLink\",\n");
         printf("  \"version\": \"%s\",\n", NEXUSLINK_VERSION);
         printf("  \"buildDate\": \"%s\",\n", NEXUSLINK_BUILD_DATE);
         
         if (show_detailed) {
             printf("  \"components\": {\n");
             printf("    \"core\": \"%s\",\n", "1.0.0");
             printf("    \"symbols\": \"%s\",\n", "1.0.0");
             printf("    \"versioning\": \"%s\",\n", "1.0.0");
             printf("    \"minimizer\": \"%s\"\n", "1.0.0");
             printf("  },\n");
         }
         
         printf("  \"copyright\": \"%s\"\n", NEXUSLINK_COPYRIGHT);
         printf("}\n");
     } else {
         /* Output in human-readable format */
         printf("NexusLink version %s (%s)\n", NEXUSLINK_VERSION, NEXUSLINK_BUILD_DATE);
         printf("%s\n", NEXUSLINK_COPYRIGHT);
         
         if (show_detailed) {
             printf("\nComponents:\n");
             printf("  Core:       v%s\n", "1.0.0");
             printf("  Symbols:    v%s\n", "1.0.0");
             printf("  Versioning: v%s\n", "1.0.0");
             printf("  Minimizer:  v%s\n", "1.0.0");
             
             /* In a real implementation, we would get the context and look up
                loaded components to show their versions. */
             if (ctx->symbols) {
                 printf("\nLoaded Components:\n");
                 /* Example of how we might list loaded components */
                 printf("  (No components currently loaded)\n");
             }
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Print help for the version command
  */
 static void version_print_help(void) {
     printf("Usage: version [OPTIONS]\n\n");
     printf("Options:\n");
     printf("  --detailed      Show detailed version information\n");
     printf("  --json          Output in JSON format\n");
 }
 
 /**
  * @brief Free version command data
  */
 static void version_free_data(void* command_data) {
     if (!command_data) {
         return;
     }
     
     VersionCommandData* data = (VersionCommandData*)command_data;
     free(data);
 }
 
 /* Version command definition */
 NexusCommand version_command = {
     .name = "version",
     .description = "Display version information",
     .handler = version_handler,
     .handler_with_params = version_handler_with_params,
     .execute = NULL,
     .data = NULL
 };
 
 /**
  * @brief Get the version command structure
  */
 NexusCommand* nlink_get_version_command(void) {
     return &version_command;
 }
