/**
 * @file mock_component.c
 * @brief Mock component for NexusLink testing
 * 
 * This file provides a mock component that can be loaded during testing
 * to verify the component loading and symbol resolution functionality.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include <stdio.h>
 
 // Component initialization function
 int nexus_component_init(NexusContext* ctx) {
     if (ctx) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Mock component initialized");
     } else {
         printf("Mock component initialized (no context)\n");
     }
     return 1;
 }
 
 // Component cleanup function
 void nexus_component_cleanup(NexusContext* ctx) {
     if (ctx) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Mock component cleaned up");
     } else {
         printf("Mock component cleaned up (no context)\n");
     }
 }
 
 // Mock function for testing
 void test_function(int x) {
     printf("Mock test_function called with argument %d\n", x);
 }
 
 // Mock function with return value
 int test_function_with_return(int x) {
     printf("Mock test_function_with_return called with argument %d\n", x);
     return x * 2;
 }
 
 // Mock variable
 int test_variable = 42;
 
 // Mock type definition
 typedef struct {
     int x;
     int y;
 } test_type;