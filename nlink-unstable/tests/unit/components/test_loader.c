/**
 * @file test_loader.c
 * @brief Unit tests for NexusLink loader functionality
 * 
 * Tests the dynamic component loading and related features of the NexusLink system.
 * These tests verify the loader's ability to handle component loading, handle
 * registration, and component lifecycle management.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_core.h"
 #include "nexus_loader.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 
 // Mock functions for testing dynamic loading
 static void* mock_dlopen(const char* path, int flags) {
     printf("MOCK: dlopen(%s, %d)\n", path, flags);
     return (void*)0x12345678; // Fake handle
 }
 
 static void* mock_dlsym(void* handle, const char* symbol) {
     printf("MOCK: dlsym(%p, %s)\n", handle, symbol);
     return (void*)mock_function; // Return address of mock function
 }
 
 static int mock_dlclose(void* handle) {
     printf("MOCK: dlclose(%p)\n", handle);
     return 0;
 }
 
 void test_handle_registry_initialization() {
     // Initialize handle registry
     NexusHandleRegistry* registry = nexus_init_handle_registry();
     
     test_assert("Handle registry initialization", 
                 registry != NULL &&
                 registry->handles == NULL &&
                 registry->paths == NULL &&
                 registry->components == NULL &&
                 registry->count == 0 &&
                 registry->capacity == 0);
     
     // Clean up
     nexus_cleanup_handle_registry(registry);
 }
 
 void test_handle_registration() {
     // Initialize handle registry
     NexusHandleRegistry* registry = nexus_init_handle_registry();
     
     // Register a handle
     void* handle = mock_dlopen("./libtest.so", RTLD_LAZY);
     NexusResult result = nexus_register_component_handle(registry, handle, "./libtest.so", "test_component");
     
     test_assert("Handle registration result", result == NEXUS_SUCCESS);
     test_assert("Handle registry count after registration", registry->count == 1);
     test_assert("Handle storage", registry->handles[0] == handle);
     test_assert("Path storage", strcmp(registry->paths[0], "./libtest.so") == 0);
     test_assert("Component ID storage", strcmp(registry->components[0], "test_component") == 0);
     
     // Register another handle
     void* handle2 = mock_dlopen("./libtest2.so", RTLD_LAZY);
     result = nexus_register_component_handle(registry, handle2, "./libtest2.so", "test_component2");
     
     test_assert("Second handle registration", result == NEXUS_SUCCESS);
     test_assert("Handle registry count after second registration", registry->count == 2);
     
     // Test capacity expansion
     // Register enough handles to trigger capacity expansion
     for (int i = 0; i < NEXUS_DEFAULT_REGISTRY_SIZE; i++) {
         char path[32];
         char component_id[32];
         snprintf(path, sizeof(path), "./lib%d.so", i);
         snprintf(component_id, sizeof(component_id), "component%d", i);
         
         void* h = mock_dlopen(path, RTLD_LAZY);
         nexus_register_component_handle(registry, h, path, component_id);
     }
     
     test_assert("Registry capacity expanded", registry->capacity > NEXUS_DEFAULT_REGISTRY_SIZE);
     
     // Clean up
     nexus_cleanup_handle_registry(registry);
 }
 
 void test_find_component_handle() {
     // Initialize handle registry
     NexusHandleRegistry* registry = nexus_init_handle_registry();
     
     // Register handles
     void* handle1 = mock_dlopen("./libtest1.so", RTLD_LAZY);
     void* handle2 = mock_dlopen("./libtest2.so", RTLD_LAZY);
     
     nexus_register_component_handle(registry, handle1, "./libtest1.so", "component1");
     nexus_register_component_handle(registry, handle2, "./libtest2.so", "component2");
     
     // Find handles
     void* found_handle1 = nexus_find_component_handle(registry, "./libtest1.so");
     void* found_handle2 = nexus_find_component_handle(registry, "./libtest2.so");
     void* nonexistent = nexus_find_component_handle(registry, "./nonexistent.so");
     
     test_assert("Find first handle", found_handle1 == handle1);
     test_assert("Find second handle", found_handle2 == handle2);
     test_assert("Non-existent handle returns NULL", nonexistent == NULL);
     
     // Clean up
     nexus_cleanup_handle_registry(registry);
 }
 
 void test_load_component() {
     // Create a context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest.so", "test_component");
     
     test_assert("Component loading", component != NULL);
     
     if (component) {
         test_assert("Component path", strcmp(component->path, "./libtest.so") == 0);
         test_assert("Component ID", strcmp(component->id, "test_component") == 0);
         test_assert("Initial ref count", component->ref_count == 1);
     }
     
     // Clean up
     if (component) {
         nexus_unload_component(ctx, component);
     }
     nexus_destroy_context(ctx);
 }
 
 void test_component_lifecycle() {
     // Create a context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest.so", "test_component");
     test_assert("Component loading for lifecycle test", component != NULL);
     
     if (component) {
         // Increase reference count
         component->ref_count++;
         test_assert("Reference count after increment", component->ref_count == 2);
         
         // First unload - should decrement but not free
         NexusResult result = nexus_unload_component(ctx, component);
         test_assert("First unload result", result == NEXUS_SUCCESS);
         test_assert("Reference count after first unload", component->ref_count == 1);
         
         // Second unload - should free the component
         result = nexus_unload_component(ctx, component);
         test_assert("Second unload result", result == NEXUS_SUCCESS);
         
         // Component is now invalid - don't access it
     }
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_resolve_component_symbol() {
     // Create a context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Load a component
     NexusComponent* component = nexus_load_component(ctx, "./libtest.so", "test_component");
     test_assert("Component loading for symbol resolution", component != NULL);
     
     if (component) {
         // Resolve a symbol
         void* symbol = nexus_resolve_component_symbol(ctx, component, "test_function");
         test_assert("Symbol resolution", symbol != NULL);
         
         // Verify the symbol is in the exported table
         if (symbol) {
             NexusSymbol* exported_symbol = nexus_symbol_table_find(
                 &ctx->symbols->exported, "test_function");
             
             test_assert("Symbol added to exported table", 
                         exported_symbol != NULL &&
                         exported_symbol->address == symbol &&
                         strcmp(exported_symbol->component_id, "test_component") == 0);
         }
         
         // Try to resolve a non-existent symbol
         void* nonexistent = nexus_resolve_component_symbol(ctx, component, "nonexistent_function");
         test_assert("Non-existent symbol resolution returns NULL", nonexistent == NULL);
         
         // Unload the component
         nexus_unload_component(ctx, component);
     }
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 void test_invalid_parameters() {
     // Create a context
     NexusContext* ctx = nexus_create_context(NULL);
     
     // Test NULL parameters
     NexusComponent* null_path = nexus_load_component(ctx, NULL, "test_component");
     test_assert("NULL path returns NULL component", null_path == NULL);
     
     NexusComponent* null_id = nexus_load_component(ctx, "./libtest.so", NULL);
     test_assert("NULL component ID returns NULL component", null_id == NULL);
     
     NexusResult unload_result = nexus_unload_component(ctx, NULL);
     test_assert("Unload NULL component returns NEXUS_INVALID_PARAMETER", 
                 unload_result == NEXUS_INVALID_PARAMETER);
     
     void* null_symbol = nexus_resolve_component_symbol(ctx, NULL, "test_function");
     test_assert("Resolve symbol from NULL component returns NULL", null_symbol == NULL);
     
     // Clean up
     nexus_destroy_context(ctx);
 }
 
 int main() {
     test_init("Loader Functionality");
     
     test_handle_registry_initialization();
     test_handle_registration();
     test_find_component_handle();
     test_load_component();
     test_component_lifecycle();
     test_resolve_component_symbol();
     test_invalid_parameters();
     
     return test_finalize();
 }