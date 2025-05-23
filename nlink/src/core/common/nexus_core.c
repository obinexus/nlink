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
#include "nlink/core/symbols/symbols.h"

 
 // Forward declaration for symbol registry initialization
 // This avoids including the full symbols header here
 extern NexusSymbolRegistry* nexus_init_symbol_registry(void);
 extern void nexus_cleanup_symbol_registry(NexusSymbolRegistry* registry);
 
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
     if (!ctx->symbols) {
         free(ctx->component_path);
         free(ctx);
         return NULL;
     }
     
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
 
const char* nexus_log_level_to_string(NexusLogLevel level) {
    switch (level) {
        case NEXUS_LOG_ERROR: return "ERROR";
        case NEXUS_LOG_INFO: return "INFO";
        default: return "UNKNOWN";
    }
}

void nexus_default_log_callback(NexusLogLevel level, const char* format, va_list args) {
    // Select output stream based on level
    FILE* stream = (level == NEXUS_LOG_ERROR) ? stderr : stdout;
    
    // Add level prefix
    const char* level_str = nexus_log_level_to_string(level);
     
     fprintf(stream, "[NEXUS %s] ", level_str);
     vfprintf(stream, format, args);
     fprintf(stream, "\n");
 }