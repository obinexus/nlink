/**
 * @file nlink.c
 * @brief Implementation of the main NexusLink library functions
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 
 #include "nlink/core/nlink.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/types.h"
 #include "nlink/core/minimizer/nexus_minimizer.h"
 #include "nlink/core/symbols/nexus_symbols.h"
 #include "nlink/core/versioning/nexus_version.h"
 
 // Forward declarations for functions not fully defined in headers
 extern NexusResult nexus_symbol_registry_initialize(NexusContext* ctx);
 extern NexusComponent* nexus_load_component(NexusContext* ctx, const char* path, const char* id);
 
 // Global context
 static NexusContext* g_global_context = NULL;
 
 // Command objects
 static NexusCommand* load_command = NULL;
 static NexusCommand* version_command = NULL;
 static NexusCommand* minimal_command = NULL;
 static NexusCommand* minimize_command = NULL;
 
 // Version information
 static const char* NLINK_VERSION = "1.0.0";
 static const char* NLINK_BUILD_DATE = __DATE__ " " __TIME__;
 static const char* NLINK_COPYRIGHT = "Copyright © 2025 OBINexus Computing";
 
 // Convert NlinkConfig to NexusConfig
 static NexusConfig convert_config(const NlinkConfig* config) {
     NexusConfig nexus_config;
     memset(&nexus_config, 0, sizeof(NexusConfig));
     
     if (config) {
         // Convert log level
         switch (config->log_level) {
             case NLINK_LOG_DEBUG:
                 nexus_config.log_level = NEXUS_LOG_DEBUG;
                 break;
             case NLINK_LOG_INFO:
                 nexus_config.log_level = NEXUS_LOG_INFO;
                 break;
             case NLINK_LOG_WARNING:
                 nexus_config.log_level = NEXUS_LOG_WARNING;
                 break;
             case NLINK_LOG_ERROR:
                 nexus_config.log_level = NEXUS_LOG_ERROR;
                 break;
             default:
                 nexus_config.log_level = NEXUS_LOG_INFO;
                 break;
         }
         
         // Convert flags
         nexus_config.flags = 0;
         if (config->flags & NLINK_FLAG_AUTO_LOAD)
             nexus_config.flags |= NEXUS_FLAG_AUTO_LOAD;
         // Note: NEXUS_FLAG_AUTO_MINIMIZE might not exist, using appropriate flag if available
         // or simply commenting out this mapping if no equivalent exists
         
         // Copy other fields
         nexus_config.log_callback = (NexusLogCallback)config->log_callback;
         nexus_config.component_path = config->component_path;
     } else {
         // Default configuration
         nexus_config.log_level = NEXUS_LOG_INFO;
         nexus_config.flags = NEXUS_FLAG_AUTO_LOAD;
         nexus_config.log_callback = NULL;
         nexus_config.component_path = NULL;
     }
     
     return nexus_config;
 }
 
 NexusResult nlink_initialize(const NlinkConfig* config) {
     if (g_global_context) {
         // Already initialized
         return NEXUS_ALREADY_EXISTS;
     }
     
     // Convert configuration
     NexusConfig nexus_config = convert_config(config);
     
     // Create context
     g_global_context = nexus_create_context(&nexus_config);
     if (!g_global_context) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize subsystems
     NexusResult result;
     
     // Initialize symbol registry
     result = nexus_symbol_registry_initialize(g_global_context);
     if (result != NEXUS_SUCCESS) {
         nexus_destroy_context(g_global_context);
         g_global_context = NULL;
         return result;
     }
     
     // Initialize version system
     result = nexus_version_initialize(g_global_context);
     if (result != NEXUS_SUCCESS) {
         nexus_symbol_registry_cleanup(g_global_context);
         nexus_destroy_context(g_global_context);
         g_global_context = NULL;
         return result;
     }
     
     return NEXUS_SUCCESS;
 }
 
 NexusContext* nlink_get_context(void) {
     return g_global_context;
 }
 
 void nlink_cleanup(void) {
     if (!g_global_context) {
         return;
     }
     
     // Clean up subsystems
     nexus_minimizer_cleanup(g_global_context);
     nexus_version_cleanup(g_global_context);
     nexus_symbol_registry_cleanup(g_global_context);
     
     // Destroy context
     nexus_destroy_context(g_global_context);
     g_global_context = NULL;
 }
 
 const char* nlink_get_version(void) {
     return NLINK_VERSION;
 }
 
 void nlink_get_build_info(NlinkBuildInfo* info) {
     if (!info) {
         return;
     }
     
     info->version = NLINK_VERSION;
     info->build_date = NLINK_BUILD_DATE;
     info->copyright = NLINK_COPYRIGHT;
 }
 
 NexusComponent* nlink_load_component(const char* path, const char* id, const char* version) {
     if (!g_global_context) {
         return NULL;
     }
     
     // Call the actual implementation
     return nexus_load_component(g_global_context, path, id);
 }
 
 NexusResult nlink_minimize_component(const char* path, NlinkMinimizeLevel level) {
     if (!g_global_context) {
         return NEXUS_NOT_INITIALIZED;
     }
     
     // Convert NlinkMinimizeLevel to NexusMinimizerLevel
     NexusMinimizerLevel minimizer_level;
     switch(level) {
         case NLINK_MINIMIZE_BASIC:
             minimizer_level = NEXUS_MINIMIZE_BASIC;
             break;
         case NLINK_MINIMIZE_STANDARD:
             minimizer_level = NEXUS_MINIMIZE_STANDARD;
             break;
         case NLINK_MINIMIZE_AGGRESSIVE:
             minimizer_level = NEXUS_MINIMIZE_AGGRESSIVE;
             break;
         default:
             minimizer_level = NEXUS_MINIMIZE_BASIC;
             break;
     }
     
     // Create minimizer configuration
     NexusMinimizerConfig config;
     memset(&config, 0, sizeof(config));
     config.level = minimizer_level;
     config.enable_metrics = true;  // Enable metrics collection
     config.verbose = false;        // No verbose output
     
     // Call the actual implementation using fields that exist in the struct
     return nexus_minimize_component(g_global_context, path, config, NULL);
 }
 
 NexusCommand* nlink_get_load_command(void) {
     if (!load_command) {
         // TODO: Initialize load command
         load_command = nexus_command_create("load", "Load a component", NULL);
     }
     return load_command;
 }
 
 NexusCommand* nlink_get_version_command(void) {
     if (!version_command) {
         // TODO: Initialize version command
         version_command = nexus_command_create("version", "Display version information", NULL);
     }
     return version_command;
 }
 
 NexusCommand* nlink_get_minimal_command(void) {
     if (!minimal_command) {
         // TODO: Initialize minimal command
         minimal_command = nexus_command_create("minimal", "Run in minimal mode", NULL);
     }
     return minimal_command;
 }
 
 NexusCommand* nlink_get_minimize_command(void) {
     if (!minimize_command) {
         // TODO: Initialize minimize command
         minimize_command = nexus_command_create("minimize", "Minimize a component", NULL);
     }
     return minimize_command;
 }