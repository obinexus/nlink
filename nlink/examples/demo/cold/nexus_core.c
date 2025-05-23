/**
 * @file nexus_core.c
 * @brief Core implementation for the NexusLink library
 * 
 * Provides the fundamental functionality for the NexusLink system,
 * including initialization, configuration, and core utilities.
 * 
 * Copyright © 2025 OBINexus Computing
 */

    #include "nlink/core/common/nexus_core.h"
 // Global context
 static NexusContext* g_context = NULL;
 
 NexusContext* nexus_create_context(const NexusConfig* config) {
     // Allocate the context
     NexusContext* ctx = (NexusContext*)malloc(sizeof(NexusContext));
     if (!ctx) {
         return NULL;
     }
     
     // Initialize the context with default values
     memset(ctx, 0, sizeof(NexusContext));
     
     // Apply configuration if provided
     if (config) {
         ctx->flags = config->flags;
         ctx->log_level = config->log_level;
         
         if (config->log_callback) {
             ctx->log_callback = config->log_callback;
         } else {
             ctx->log_callback = nexus_default_log_callback;
         }
         
         if (config->component_path) {
             ctx->component_path = strdup(config->component_path);
         }
     } else {
         // Default configuration
         ctx->flags = NEXUS_FLAG_NONE;
         ctx->log_level = NEXUS_LOG_INFO;
         ctx->log_callback = nexus_default_log_callback;
     }
     
     // Initialize the symbol registry
     ctx->symbols = nexus_init_symbol_registry();
     
     // Set as global context if not already set
     if (!g_context) {
         g_context = ctx;
     }
     
     return ctx;
 }
 
 void nexus_destroy_context(NexusContext* ctx) {
     if (!ctx) {
         return;
     }
     
     // Free allocated resources
     free(ctx->component_path);
     
     // Cleanup the symbol registry
     nexus_cleanup_symbol_registry(ctx->symbols);
     
     // Reset global context if this is it
     if (g_context == ctx) {
         g_context = NULL;
     }
     
     // Free the context itself
     free(ctx);
 }
 
 NexusContext* nexus_get_global_context(void) {
     return g_context;
 }
 
 void nexus_set_global_context(NexusContext* ctx) {
     g_context = ctx;
 }
 
 NexusResult nexus_set_log_level(NexusContext* ctx, NexusLogLevel level) {
     if (!ctx) {
         if (!g_context) {
             return NEXUS_INVALID_PARAMETER;
         }
         ctx = g_context;
     }
     
     ctx->log_level = level;
     return NEXUS_SUCCESS;
 }
 
 void nexus_log(NexusContext* ctx, NexusLogLevel level, const char* format, ...) {
     if (!ctx) {
         if (!g_context) {
             return;
         }
         ctx = g_context;
     }
     
     // Skip if log level is too low
     if (level < ctx->log_level) {
         return;
     }
     
     // Use the context's log callback
     if (ctx->log_callback) {
         va_list args;
         va_start(args, format);
         ctx->log_callback(level, format, args);
         va_end(args);
     }
 }
 
 void nexus_default_log_callback(NexusLogLevel level, const char* format, va_list args) {
     // Select output stream based on level
     FILE* stream = (level == NEXUS_LOG_ERROR) ? stderr : stdout;
     
     // Add level prefix
     const char* level_str = "UNKNOWN";
     switch (level) {
         case NEXUS_LOG_DEBUG:   level_str = "DEBUG"; break;
         case NEXUS_LOG_INFO:    level_str = "INFO"; break;
         case NEXUS_LOG_WARNING: level_str = "WARNING"; break;
         case NEXUS_LOG_ERROR:   level_str = "ERROR"; break;
     }
     
     fprintf(stream, "[NEXUS %s] ", level_str);
     vfprintf(stream, format, args);
     fprintf(stream, "\n");
 }
 
 const char* nexus_result_to_string(NexusResult result) {
     switch (result) {
         case NEXUS_SUCCESS:            return "Success";
         case NEXUS_INVALID_PARAMETER:  return "Invalid parameter";
         case NEXUS_NOT_INITIALIZED:    return "Not initialized";
         case NEXUS_OUT_OF_MEMORY:      return "Out of memory";
         case NEXUS_NOT_FOUND:          return "Not found";
         case NEXUS_ALREADY_EXISTS:     return "Already exists";
         case NEXUS_INVALID_OPERATION:  return "Invalid operation";
         case NEXUS_UNSUPPORTED:        return "Unsupported operation";
         case NEXUS_IO_ERROR:           return "I/O error";
         case NEXUS_DEPENDENCY_ERROR:   return "Dependency error";
         case NEXUS_VERSION_CONFLICT:   return "Version conflict";
         case NEXUS_SYMBOL_ERROR:       return "Symbol error";
         default:                       return "Unknown error";
     }
 }