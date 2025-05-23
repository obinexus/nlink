/**
 * @file test_param_extraction.c
 * @brief Test for command parameter extraction functionality
 * 
 * Tests the parameter extraction capabilities of the enhanced command system.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 
 // Test command handler with parameters
 static NexusResult test_handler_with_params(NexusContext* ctx, NlinkCommandParams* params) {
     printf("Parameters extracted:\n");
     
     // Print all parameters
     size_t count = nlink_command_params_count(params);
     printf("Parameter count: %zu\n", count);
     
     for (size_t i = 0; i < count; i++) {
         const char* name;
         const char* value;
         
         if (nlink_command_params_get_at(params, i, &name, &value)) {
             printf("  %s = %s\n", name, value ? value : "(null)");
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 // Test command definition
 static NexusCommand test_command = {
     .name = "test",
     .description = "Test command for parameter extraction",
     .handler = NULL,
     .handler_with_params = test_handler_with_params
 };
 
 int main(int argc, char** argv) {
     // Create a context
     NexusConfig config = {0};
     config.log_level = NEXUS_LOG_INFO;
     config.flags = NEXUS_FLAG_NONE;
     
     NexusContext* ctx = nexus_create_context(&config);
     if (!ctx) {
         fprintf(stderr, "Failed to create context\n");
         return 1;
     }
     
     // Create command router
     NlinkCommandRouter* router = nlink_command_router_create();
     if (!router) {
         fprintf(stderr, "Failed to create command router\n");
         nexus_destroy_context(ctx);
         return 1;
     }
     
     // Test pattern 1: Basic command with one parameter
     const char* pattern1 = "^test ([a-zA-Z0-9_-]+)$";
     const char* params1[] = {"component"};
     
     NexusResult result = nlink_command_router_register_with_params(
         router,
         pattern1,
         &test_command,
         NLINK_PATTERN_FLAG_REGEX,
         params1,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to register pattern 1\n");
         goto cleanup;
     }
     
     // Test pattern 2: Command with multiple parameters
     const char* pattern2 = "^test ([a-zA-Z0-9_-]+) version ([0-9.]+) path ([/a-zA-Z0-9._-]+)$";
     const char* params2[] = {"component", "version", "path"};
     
     result = nlink_command_router_register_with_params(
         router,
         pattern2,
         &test_command,
         NLINK_PATTERN_FLAG_REGEX,
         params2,
         3
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to register pattern 2\n");
         goto cleanup;
     }
     
     // Test pattern 3: Optional parameter pattern
     const char* pattern3 = "^optional ([a-zA-Z0-9_-]+)( with ([a-zA-Z0-9_-]+))?$";
     const char* params3[] = {"required", NULL, "optional"};
     
     result = nlink_command_router_register_with_params(
         router,
         pattern3,
         &test_command,
         NLINK_PATTERN_FLAG_REGEX,
         params3,
         3
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to register pattern 3\n");
         goto cleanup;
     }
     
     // Test case 1: Basic parameter extraction
     printf("\n=== Test Case 1: Basic Parameter ===\n");
     NlinkCommandParams* extracted1 = NULL;
     result = nlink_command_router_execute_with_params(
         router,
         "test component1",
         ctx,
         &extracted1
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to execute test case 1\n");
         goto cleanup;
     }
     
     // Clean up
     if (extracted1) {
         nlink_command_params_destroy(extracted1);
     }
     
     // Test case 2: Multiple parameters
     printf("\n=== Test Case 2: Multiple Parameters ===\n");
     NlinkCommandParams* extracted2 = NULL;
     result = nlink_command_router_execute_with_params(
         router,
         "test component2 version 1.2.3 path /usr/local/lib",
         ctx,
         &extracted2
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to execute test case 2\n");
         goto cleanup;
     }
     
     // Clean up
     if (extracted2) {
         nlink_command_params_destroy(extracted2);
     }
     
     // Test case 3: Optional parameter (with)
     printf("\n=== Test Case 3: Optional Parameter (with) ===\n");
     NlinkCommandParams* extracted3 = NULL;
     result = nlink_command_router_execute_with_params(
         router,
         "optional required-value with optional-value",
         ctx,
         &extracted3
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to execute test case 3\n");
         goto cleanup;
     }
     
     // Clean up
     if (extracted3) {
         nlink_command_params_destroy(extracted3);
     }
     
     // Test case 4: Optional parameter (without)
     printf("\n=== Test Case 4: Optional Parameter (without) ===\n");
     NlinkCommandParams* extracted4 = NULL;
     result = nlink_command_router_execute_with_params(
         router,
         "optional required-value-only",
         ctx,
         &extracted4
     );
     
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Failed to execute test case 4\n");
         goto cleanup;
     }
     
     // Clean up
     if (extracted4) {
         nlink_command_params_destroy(extracted4);
     }
     
     printf("\nAll tests completed successfully!\n");
     
 cleanup:
     // Clean up
     nlink_command_router_destroy(router);
     nexus_destroy_context(ctx);
     
     return (result == NEXUS_SUCCESS) ? 0 : 1;
 }