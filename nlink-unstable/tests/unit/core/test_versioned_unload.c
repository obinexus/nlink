// tests/unit/test_versioned_unload.c
// Unit tests for library unloading functionality in NexusLink

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // for sleep()
#include "nexus_lazy_versioned.h"
#include "nexus_versioned_symbols.h"
#include "nexus_semver.h"

// Test fixture - global variables for testing
static VersionedSymbolRegistry* test_registry = NULL;
static const char* test_lib_path = "./libtest_component.so";

// Mock functions for testing without actually loading libraries
void* mock_dlopen(const char* path, int flags) {
    printf("MOCK: dlopen(%s, %d)\n", path, flags);
    return (void*)0x12345678; // Fake handle
}

void* mock_dlsym(void* handle, const char* symbol) {
    printf("MOCK: dlsym(%p, %s)\n", handle, symbol);
    return (void*)0x87654321; // Fake function pointer
}

int mock_dlclose(void* handle) {
    printf("MOCK: dlclose(%p)\n", handle);
    return 0;
}

// Initialize test fixture
static void setup_test() {
    // Create a new registry
    test_registry = nexus_versioned_registry_create();
    
    // Configure versioned lazy loading
    NexusVersionedLazyConfig config = {
        .library_path = test_lib_path,
        .component_id = "test_component",
        .version = "1.0.0",
        .version_constraint = NULL,
        .auto_unload = true,
        .unload_timeout_sec = 2, // Short timeout for testing
        .registry = test_registry
    };
    
    nexus_set_versioned_lazy_config(&config);
    
    // Add some test symbols to the registry
    versioned_symbol_table_add(&test_registry->exported, 
                               "test_func1", "1.0.0", 
                               (void*)0x1111, VSYMBOL_FUNCTION, 
                               "test_component", 10);
    
    versioned_symbol_table_add(&test_registry->exported, 
                               "test_func2", "1.0.0", 
                               (void*)0x2222, VSYMBOL_FUNCTION, 
                               "test_component", 10);
    
    // Add a component dependency relationship
    nexus_add_component_dependency(test_registry,
                                  "main_app",
                                  "test_component",
                                  "^1.0.0",
                                  false);
}

// Clean up test fixture
static void teardown_test() {
    if (test_registry) {
        nexus_versioned_registry_free(test_registry);
        test_registry = NULL;
    }
}

// Test: No unloading when reference count is positive
static int test_no_unload_with_refs() {
    printf("\n===== TEST: No unloading when symbols are referenced =====\n");
    
    // Set up test
    setup_test();
    
    // Set reference counts
    for (size_t i = 0; i < test_registry->exported.size; i++) {
        test_registry->exported.symbols[i].ref_count = 1;
        test_registry->exported.symbols[i].last_used = time(NULL) - 10; // 10 seconds ago
    }
    
    // Initialize handle registry
    extern NexusHandleRegistry* nexus_handle_registry;
    nexus_init_handle_registry();
    
    // Add a handle to the registry
    void* handle = mock_dlopen(test_lib_path, 0);
    nexus_register_handle(handle, test_lib_path);
    
    // Run the function under test
    nexus_check_unused_versioned_libraries(test_registry);
    
    // Verify: No libraries should be unloaded since ref_count > 0
    int success = (nexus_handle_registry->count == 1);
    
    // Clean up
    nexus_cleanup_handle_registry();
    teardown_test();
    
    printf("Test result: %s\n", success ? "PASSED" : "FAILED");
    return success;
}

// Test: Unloading when timeout expires and ref count is zero
static int test_unload_when_timeout_expired() {
    printf("\n===== TEST: Unloading when timeout expires =====\n");
    
    // Set up test
    setup_test();
    
    // Set reference counts to zero and old timestamp
    for (size_t i = 0; i < test_registry->exported.size; i++) {
        test_registry->exported.symbols[i].ref_count = 0;
        test_registry->exported.symbols[i].last_used = time(NULL) - 10; // 10 seconds ago
    }
    
    // Initialize handle registry
    extern NexusHandleRegistry* nexus_handle_registry;
    nexus_init_handle_registry();
    
    // Add a handle to the registry
    void* handle = mock_dlopen(test_lib_path, 0);
    nexus_register_handle(handle, test_lib_path);
    
    // Run the function under test
    nexus_check_unused_versioned_libraries(test_registry);
    
    // Verify: Library should be unloaded
    int success = (nexus_handle_registry->count == 0);
    
    // Clean up
    nexus_cleanup_handle_registry();
    teardown_test();
    
    printf("Test result: %s\n", success ? "PASSED" : "FAILED");
    return success;
}

// Test: No unloading when auto_unload is disabled
static int test_no_unload_when_disabled() {
    printf("\n===== TEST: No unloading when auto_unload is disabled =====\n");
    
    // Set up test
    setup_test();
    
    // Disable auto-unloading
    NexusVersionedLazyConfig config = nexus_versioned_lazy_config;
    config.auto_unload = false;
    nexus_set_versioned_lazy_config(&config);
    
    // Set reference counts to zero and old timestamp
    for (size_t i = 0; i < test_registry->exported.size; i++) {
        test_registry->exported.symbols[i].ref_count = 0;
        test_registry->exported.symbols[i].last_used = time(NULL) - 10; // 10 seconds ago
    }
    
    // Initialize handle registry
    extern NexusHandleRegistry* nexus_handle_registry;
    nexus_init_handle_registry();
    
    // Add a handle to the registry
    void* handle = mock_dlopen(test_lib_path, 0);
    nexus_register_handle(handle, test_lib_path);
    
    // Run the function under test
    nexus_check_unused_versioned_libraries(test_registry);
    
    // Verify: Library should not be unloaded because auto_unload is disabled
    int success = (nexus_handle_registry->count == 1);
    
    // Clean up
    nexus_cleanup_handle_registry();
    teardown_test();
    
    printf("Test result: %s\n", success ? "PASSED" : "FAILED");
    return success;
}

// Test: Multiple component handling
static int test_multiple_components() {
    printf("\n===== TEST: Multiple component handling =====\n");
    
    // Set up test
    setup_test();
    
    // Add another component with active references
    versioned_symbol_table_add(&test_registry->exported, 
                               "other_func", "2.0.0", 
                               (void*)0x3333, VSYMBOL_FUNCTION, 
                               "other_component", 10);
    
    // Set reference counts - only test_component should be unloaded
    for (size_t i = 0; i < test_registry->exported.size; i++) {
        VersionedSymbol* symbol = &test_registry->exported.symbols[i];
        if (strcmp(symbol->component_id, "test_component") == 0) {
            symbol->ref_count = 0;
            symbol->last_used = time(NULL) - 10; // 10 seconds ago
        } else {
            symbol->ref_count = 1;
            symbol->last_used = time(NULL);
        }
    }
    
    // Initialize handle registry
    extern NexusHandleRegistry* nexus_handle_registry;
    nexus_init_handle_registry();
    
    // Add handles to the registry
    void* handle1 = mock_dlopen(test_lib_path, 0);
    nexus_register_handle(handle1, test_lib_path);
    
    void* handle2 = mock_dlopen("./libother_component.so", 0);
    nexus_register_handle(handle2, "./libother_component.so");
    
    // Run the function under test
    nexus_check_unused_versioned_libraries(test_registry);
    
    // Verify: Only test_component should be unloaded
    int success = (nexus_handle_registry->count == 1);
    
    // Clean up
    nexus_cleanup_handle_registry();
    teardown_test();
    
    printf("Test result: %s\n", success ? "PASSED" : "FAILED");
    return success;
}

// Main test runner
int main() {
    printf("Running NexusLink versioned unloading tests...\n");
    
    int passed = 0;
    int total = 4;
    
    // Run all tests
    passed += test_no_unload_with_refs();
    passed += test_unload_when_timeout_expired();
    passed += test_no_unload_when_disabled();
    passed += test_multiple_components();
    
    // Print summary
    printf("\n===== TEST SUMMARY =====\n");
    printf("Passed: %d/%d\n", passed, total);
    
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}