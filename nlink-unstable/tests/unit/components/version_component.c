/**
 * @file version_component.c
 * @brief Version-aware mock component for NexusLink testing
 * 
 * This file provides a version-aware mock component that implements
 * multiple versions of the same symbols to test version resolution.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_symbols.h"
 #include "nexus_versioned_symbols.h"
 #include <stdio.h>
 
 // Component versioning information
 const char* component_id = "version_component";
 const char* component_version = "2.0.0";
 
 // Component initialization function
 int nexus_component_init(NexusContext* ctx) {
     if (ctx) {
         nexus_log(ctx, NEXUS_LOG_INFO, 
                  "Version component initialized (version %s)", component_version);
         
         // Register version information
         VersionedSymbolRegistry* registry = 
             (VersionedSymbolRegistry*)ctx->symbols;
         
         // Register v1.0.0 symbols
         versioned_symbol_table_add(&registry->exported, 
                                   "versioned_function", "1.0.0", 
                                   (void*)versioned_function_v1, 
                                   VSYMBOL_FUNCTION, component_id, 10);
         
         // Register v2.0.0 symbols
         versioned_symbol_table_add(&registry->exported, 
                                   "versioned_function", "2.0.0", 
                                   (void*)versioned_function_v2, 
                                   VSYMBOL_FUNCTION, component_id, 10);
     } else {
         printf("Version component initialized (no context)\n");
     }
     
     return 1;
 }
 
 // Component cleanup function
 void nexus_component_cleanup(NexusContext* ctx) {
     if (ctx) {
         nexus_log(ctx, NEXUS_LOG_INFO, 
                  "Version component cleaned up (version %s)", component_version);
     } else {
         printf("Version component cleaned up (no context)\n");
     }
 }
 
 // V1.0.0 implementation
 void versioned_function_v1(int x) {
     printf("Versioned function v1.0.0 called with argument %d\n", x);
 }
 
 // V2.0.0 implementation
 void versioned_function_v2(int x) {
     printf("Versioned function v2.0.0 called with argument %d\n", x);
     printf("This version has additional functionality.\n");
 }
 
 // Function that should be available in all versions
 void common_function(const char* str) {
     printf("Common function called with string: %s\n", str);
 }