/**
 * @file test_component_lifecycle.c
 * @brief Integration test for component lifecycle
 * 
 * This test verifies the complete lifecycle of components in the NexusLink system:
 * loading, symbol resolution, reference counting, and unloading.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_loader.h"
 #include "nexus_symbols.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 
 // Mock functions for component lifecycle
 static int mock_component_init(NexusContext* ctx) {
     printf("Mock component initialization\n");
     return 1;
 }
 
 static void mock_component_cleanup(NexusContext* ctx) {
     printf("Mock component cleanup\n");
 }
 
 // Mock the dynamic loading functions
 static void* mock_dlopen(const char* path, int flags) {
     printf("MOCK: dlopen(%s, %d)\n", path, flags);
     return (void*)0x12345678; // Fake handle
 }
 
 static void* mock_dlsym(void* handle, const char* symbol) {
     printf("MOCK: dlsym(%p, %s)\n", handle, symbol);
     
     // Return appropriate function pointer based on symbol name
     if (strcmp(symbol, "nexus_component_init") == 0) {
         return (void*)mock_component_init;
     }
     else if (strcmp(symbol, "nexus_component_cleanup") == 0) {
         return (void*)mock_component_cleanup;
     }
     else if (strcmp(symbol, "test_function") == 0) {
         return (void*)mock_function;
     }
     
     return NULL;
 }
 
 static int mock_dlclose(void* handle) {
     printf("MOCK: dlclose(%p)\n", handle);
     return 0;
 }
 
 void test_component_loading() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest_component.so", "test_component");
     
     test_assert("Component loading", component != NULL);
     
     if (component) {
         test_assert("Component path", strcmp(component->path, "./libtest_component.so") == 0);
         test_assert("Component ID", strcmp(component->id, "test_component") == 0);
         test_assert("Initial reference count", component->ref_count == 1);
     }
     
     // Clean up
     if (component) {
         nexus_unload_component(ctx, component);
     }
     nexus_destroy_context(ctx);
 }
 
 void test_symbol_resolution_from_component() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest_component.so", "test_component");
     
     test_assert("Component loading for symbol resolution", component != NULL);
     
     // Resolve a symbol from the component
     void* symbol_addr = NULL;
     if (component) {
         symbol_addr = nexus_resolve_component_symbol(ctx, component, "test_function");
     }
     
     test_assert("Symbol resolution from component", symbol_addr != NULL);
     
     // Verify the symbol is added to the exported table
     if (symbol_addr) {
         NexusSymbol* symbol = nexus_symbol_table_find(&ctx->symbols->exported, "test_function");
         test_assert("Symbol added to exported table", 
                     symbol != NULL && 
                     symbol->address == symbol_addr && 
                     strcmp(symbol->component_id, "test_component") == 0);
     }
     
     // Clean up
     if (component) {
         nexus_unload_component(ctx, component);
     }
     nexus_destroy_context(ctx);
 }
 
 void test_component_reference_counting() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest_component.so", "test_component");
     
     test_assert("Component loading for reference counting", component != NULL);
     
     // Increase reference count
     if (component) {
         component->ref_count++;
         test_assert("Increased reference count", component->ref_count == 2);
         
         // Unload once - should decrease ref count but not unload
         NexusResult result = nexus_unload_component(ctx, component);
         test_assert("First unload decreases ref count", 
                     result == NEXUS_SUCCESS && component->ref_count == 1);
         
         // Unload again - should actually unload
         // Note: After this point, component is freed and should not be accessed
         result = nexus_unload_component(ctx, component);
         test_assert("Second unload succeeds", result == NEXUS_SUCCESS);
         
         // component is now invalid, don't access it
     }
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_component_handle_registry() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Initialize handle registry
     NexusHandleRegistry* registry = nexus_init_handle_registry();
     test_assert("Handle registry initialization", registry != NULL);
     
     // Register a handle
     void* handle = mock_dlopen("./libtest_component.so", RTLD_LAZY);
     NexusResult result = nexus_register_component_handle(registry, handle, 
                                                        "./libtest_component.so", 
                                                        "test_component");
     
     test_assert("Register component handle", result == NEXUS_SUCCESS);
     
     // Find the handle
     void* found_handle = nexus_find_component_handle(registry, "./libtest_component.so");
     test_assert("Find component handle", found_handle == handle);
     
     // Clean up
     nexus_cleanup_handle_registry(registry);
     nexus_destroy_context(ctx);
 }
 
 void test_component_unloading() {
     // Create context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest_component.so", "test_component");
     
     test_assert("Component loading for unloading test", component != NULL);
     
     // Resolve a symbol
     void* symbol_addr = NULL;
     if (component) {
         symbol_addr = nexus_resolve_component_symbol(ctx, component, "test_function");
         test_assert("Symbol resolution for unloading test", symbol_addr != NULL);
     }
     
     // Unload component
     if (component) {
         NexusResult result = nexus_unload_component(ctx, component);
         test_assert("Component unloading", result == NEXUS_SUCCESS);
         
         // component is now invalid, don't access it
     }
     
     // Check if symbol is still in exported table
     // In a real implementation, this would depend on the reference counting policy
     // For this test, we'll just assert that the behavior is consistent
     NexusSymbol* symbol = nexus_symbol_table_find(&ctx->symbols->exported, "test_function");
     if (symbol) {
         printf("Note: Symbols remain in tables after component unloading (reference counted)\n");
     } else {
         printf("Note: Symbols are removed from tables when component is unloaded\n");
     }
     
     // This is not a pass/fail assertion since either behavior could be valid
     // depending on implementation choices
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 int main() {
     test_init("Component Lifecycle");
     
     test_component_loading();
     test_symbol_resolution_from_component();
     test_component_reference_counting();
     test_component_handle_registry();
     test_component_unloading();
     
     return test_finalize();
 }