/**
 * @file test_core_context.c
 * @brief Unit tests for NexusLink core context
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // Custom log callback for testing
 static NexusLogLevel last_log_level = NEXUS_LOG_INFO;
 static char last_log_message[256] = {0};
 
 static void test_log_callback(NexusLogLevel level, const char* format, va_list args) {
     last_log_level = level;
     vsnprintf(last_log_message, sizeof(last_log_message), format, args);
 }
 
 void test_context_creation() {
     // Create context with default config
     NexusContext* ctx = nexus_create_context(NULL);
     test_assert("Context creation with default config", ctx != NULL);
     
     // Verify default values
     test_assert("Default log level", ctx->log_level == NEXUS_LOG_INFO);
     test_assert("Default flags", ctx->flags == NEXUS_FLAG_NONE);
     test_assert("Symbol registry initialized", ctx->symbols != NULL);
     
     // Clean up
     nexus_destroy_context(ctx);
     
     // Create context with custom config
     NexusConfig config = {
         .flags = NEXUS_FLAG_STRICT_VERSIONING | NEXUS_FLAG_VERBOSE_LOGGING,
         .log_level = NEXUS_LOG_DEBUG,
         .log_callback = test_log_callback,
         .component_path = "/custom/path"
     };
     
     ctx = nexus_create_context(&config);
     test_assert("Context creation with custom config", ctx != NULL);
     
     // Verify custom values
     test_assert("Custom log level", ctx->log_level == NEXUS_LOG_DEBUG);
     test_assert("Custom flags", ctx->flags == (NEXUS_FLAG_STRICT_VERSIONING | NEXUS_FLAG_VERBOSE_LOGGING));
     test_assert("Custom log callback", ctx->log_callback == test_log_callback);
     test_assert("Custom component path", 
                 ctx->component_path != NULL && 
                 strcmp(ctx->component_path, "/custom/path") == 0);
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_global_context() {
     // Set and get global context
     NexusContext* ctx = nexus_create_context(NULL);
     nexus_set_global_context(ctx);
     
     NexusContext* global = nexus_get_global_context();
     test_assert("Get global context", global == ctx);
     
     // Clean up
     nexus_destroy_context(ctx);
     
     // Global context should now be NULL
     global = nexus_get_global_context();
     test_assert("Global context is NULL after destruction", global == NULL);
 }
 
 void test_log_level() {
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Set log level
     NexusResult result = nexus_set_log_level(ctx, NEXUS_LOG_DEBUG);
     test_assert("Set log level", 
                 result == NEXUS_SUCCESS && 
                 ctx->log_level == NEXUS_LOG_DEBUG);
     
     // Set invalid context (should use global)
     nexus_set_global_context(ctx);
     result = nexus_set_log_level(NULL, NEXUS_LOG_ERROR);
     test_assert("Set log level with NULL context", 
                 result == NEXUS_SUCCESS && 
                 ctx->log_level == NEXUS_LOG_ERROR);
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_logging() {
     // Create context with custom log callback
     NexusConfig config = {
         .log_level = NEXUS_LOG_DEBUG,
         .log_callback = test_log_callback
     };
     
     NexusContext* ctx = nexus_create_context(&config);
     
     // Clear last log
     last_log_level = NEXUS_LOG_INFO;
     memset(last_log_message, 0, sizeof(last_log_message));
     
     // Log a message
     nexus_log(ctx, NEXUS_LOG_WARNING, "Test warning: %d", 42);
     
     // Verify log
     test_assert("Log level passed to callback", last_log_level == NEXUS_LOG_WARNING);
     test_assert("Log message format", strcmp(last_log_message, "Test warning: 42") == 0);
     
     // Test log level filtering
     // Clear last log
     last_log_level = NEXUS_LOG_INFO;
     memset(last_log_message, 0, sizeof(last_log_message));
     
     // Set higher log level
     nexus_set_log_level(ctx, NEXUS_LOG_ERROR);
     
     // Log a message below the level
     nexus_log(ctx, NEXUS_LOG_WARNING, "This should be filtered");
     
     // Message should be filtered
     test_assert("Log level filtering", last_log_message[0] == '\0');
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_result_to_string() {
     // Test all result codes
     test_assert("NEXUS_SUCCESS string", 
                 strcmp(nexus_result_to_string(NEXUS_SUCCESS), "Success") == 0);
     
     test_assert("NEXUS_INVALID_PARAMETER string", 
                 strcmp(nexus_result_to_string(NEXUS_INVALID_PARAMETER), "Invalid parameter") == 0);
     
     test_assert("NEXUS_NOT_INITIALIZED string", 
                 strcmp(nexus_result_to_string(NEXUS_NOT_INITIALIZED), "Not initialized") == 0);
     
     test_assert("NEXUS_OUT_OF_MEMORY string", 
                 strcmp(nexus_result_to_string(NEXUS_OUT_OF_MEMORY), "Out of memory") == 0);
     
     test_assert("NEXUS_NOT_FOUND string", 
                 strcmp(nexus_result_to_string(NEXUS_NOT_FOUND), "Not found") == 0);
     
     test_assert("NEXUS_ALREADY_EXISTS string", 
                 strcmp(nexus_result_to_string(NEXUS_ALREADY_EXISTS), "Already exists") == 0);
     
     test_assert("NEXUS_INVALID_OPERATION string", 
                 strcmp(nexus_result_to_string(NEXUS_INVALID_OPERATION), "Invalid operation") == 0);
     
     test_assert("NEXUS_UNSUPPORTED string", 
                 strcmp(nexus_result_to_string(NEXUS_UNSUPPORTED), "Unsupported operation") == 0);
     
     test_assert("NEXUS_IO_ERROR string", 
                 strcmp(nexus_result_to_string(NEXUS_IO_ERROR), "I/O error") == 0);
     
     test_assert("NEXUS_DEPENDENCY_ERROR string", 
                 strcmp(nexus_result_to_string(NEXUS_DEPENDENCY_ERROR), "Dependency error") == 0);
     
     test_assert("NEXUS_VERSION_CONFLICT string", 
                 strcmp(nexus_result_to_string(NEXUS_VERSION_CONFLICT), "Version conflict") == 0);
     
     test_assert("NEXUS_SYMBOL_ERROR string", 
                 strcmp(nexus_result_to_string(NEXUS_SYMBOL_ERROR), "Symbol error") == 0);
     
     // Test unknown result code
     test_assert("Unknown result code", 
                 strcmp(nexus_result_to_string(999), "Unknown error") == 0);
 }
 
 int main() {
     test_init("Core Context");
     
     test_context_creation();
     test_global_context();
     test_log_level();
     test_logging();
     test_result_to_string();
     
     return test_finalize();
 }