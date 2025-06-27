/**
 * @file test_etps_telemetry.c
 * @brief TDD Test Suite for ETPS Telemetry System
 * @methodology TDD Retrofit - Testing Existing Implementation
 * @phase VALIDATION - Comprehensive Testing of Existing Functionality
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "nlink_qa_poc/etps/telemetry.h"

// =============================================================================
// Test Configuration and Setup
// =============================================================================

static int test_count = 0;
static int passed_tests = 0;
static int failed_tests = 0;

#define RUN_TEST(test_func) do { \
    printf("Running %s...", #test_func); \
    test_count++; \
    if (test_func()) { \
        printf(" ✅ PASSED\n"); \
        passed_tests++; \
    } else { \
        printf(" ❌ FAILED\n"); \
        failed_tests++; \
    } \
} while(0)

// =============================================================================
// Core ETPS Functionality Tests
// =============================================================================

int test_etps_initialization() {
    // Test ETPS system initialization
    int result = etps_init();
    if (result != 0) {
        printf("\n    Error: etps_init() returned %d, expected 0", result);
        return 0;
    }
    
    // Verify initialization state
    if (!etps_is_initialized()) {
        printf("\n    Error: etps_is_initialized() returned false after init");
        return 0;
    }
    
    return 1;
}

int test_etps_context_creation() {
    // Test context creation with valid name
    etps_context_t* ctx = etps_context_create("test_context");
    if (ctx == NULL) {
        printf("\n    Error: etps_context_create() returned NULL");
        return 0;
    }
    
    // Cleanup
    etps_context_destroy(ctx);
    return 1;
}

int test_etps_context_null_handling() {
    // Test context creation with NULL name
    etps_context_t* ctx = etps_context_create(NULL);
    if (ctx != NULL) {
        printf("\n    Error: etps_context_create(NULL) should return NULL");
        etps_context_destroy(ctx);
        return 0;
    }
    
    return 1;
}

int test_etps_semverx_component_registration() {
    etps_context_t* ctx = etps_context_create("semverx_test");
    if (ctx == NULL) {
        return 0;
    }
    
    // Create test component
    semverx_component_t test_component = {
        .name = "test_component",
        .version = "1.0.0",
        .range_state = SEMVERX_RANGE_STABLE,
        .compatible_range = "^1.0.0",
        .hot_swap_enabled = false,
        .migration_policy = "none",
        .component_id = 12345
    };
    
    // Test component registration
    int result = etps_register_component(ctx, &test_component);
    if (result != 0) {
        printf("\n    Error: etps_register_component() returned %d", result);
        etps_context_destroy(ctx);
        return 0;
    }
    
    etps_context_destroy(ctx);
    return 1;
}

int test_etps_compatibility_validation() {
    etps_context_t* ctx = etps_context_create("compat_test");
    if (ctx == NULL) {
        return 0;
    }
    
    // Create compatible components
    semverx_component_t source = {
        .name = "source_component",
        .version = "1.0.0",
        .range_state = SEMVERX_RANGE_STABLE,
        .compatible_range = "^1.0.0",
        .hot_swap_enabled = false,
        .migration_policy = "none",
        .component_id = 1001
    };
    
    semverx_component_t target = {
        .name = "target_component", 
        .version = "1.1.0",
        .range_state = SEMVERX_RANGE_STABLE,
        .compatible_range = "^1.0.0",
        .hot_swap_enabled = false,
        .migration_policy = "none",
        .component_id = 1002
    };
    
    etps_semverx_event_t event;
    compatibility_result_t result = etps_validate_component_compatibility(
        ctx, &source, &target, &event
    );
    
    if (result == COMPAT_DENIED) {
        printf("\n    Error: Compatible components marked as DENIED");
        etps_context_destroy(ctx);
        return 0;
    }
    
    etps_context_destroy(ctx);
    return 1;
}

int test_etps_utility_functions() {
    // Test range state to string conversion
    const char* stable_str = etps_range_state_to_string(SEMVERX_RANGE_STABLE);
    if (stable_str == NULL || strlen(stable_str) == 0) {
        printf("\n    Error: etps_range_state_to_string() returned invalid result");
        return 0;
    }
    
    // Test compatibility result to string conversion
    const char* allowed_str = etps_compatibility_result_to_string(COMPAT_ALLOWED);
    if (allowed_str == NULL || strlen(allowed_str) == 0) {
        printf("\n    Error: etps_compatibility_result_to_string() returned invalid result");
        return 0;
    }
    
    // Test timestamp generation
    uint64_t timestamp = etps_get_current_timestamp();
    if (timestamp == 0) {
        printf("\n    Error: etps_get_current_timestamp() returned 0");
        return 0;
    }
    
    return 1;
}

int test_etps_validation_functions() {
    etps_context_t* ctx = etps_context_create("validation_test");
    if (ctx == NULL) {
        return 0;
    }
    
    // Test input validation
    bool valid = etps_validate_input(ctx, "test_param", "test_value", "string");
    if (!valid) {
        printf("\n    Error: etps_validate_input() failed for valid input");
        etps_context_destroy(ctx);
        return 0;
    }
    
    // Test invalid input
    bool invalid = etps_validate_input(ctx, "test_param", NULL, "string");
    if (invalid) {
        printf("\n    Error: etps_validate_input() should fail for NULL input");
        etps_context_destroy(ctx);
        return 0;
    }
    
    etps_context_destroy(ctx);
    return 1;
}

int test_etps_cleanup() {
    // Test system cleanup
    etps_shutdown();
    
    // Verify cleanup state
    if (etps_is_initialized()) {
        printf("\n    Error: etps_is_initialized() returned true after shutdown");
        return 0;
    }
    
    return 1;
}

// =============================================================================
// Performance and Stress Tests
// =============================================================================

int test_etps_performance_context_creation() {
    // Test multiple context creation/destruction cycles
    for (int i = 0; i < 100; i++) {
        etps_context_t* ctx = etps_context_create("perf_test");
        if (ctx == NULL) {
            printf("\n    Error: Context creation failed at iteration %d", i);
            return 0;
        }
        etps_context_destroy(ctx);
    }
    
    return 1;
}

int test_etps_stress_component_registration() {
    etps_context_t* ctx = etps_context_create("stress_test");
    if (ctx == NULL) {
        return 0;
    }
    
    // Register multiple components
    for (int i = 0; i < 50; i++) {
        semverx_component_t component = {
            .name = "stress_component",
            .version = "1.0.0",
            .range_state = SEMVERX_RANGE_STABLE,
            .compatible_range = "^1.0.0",
            .hot_swap_enabled = false,
            .migration_policy = "none",
            .component_id = 2000 + i
        };
        
        int result = etps_register_component(ctx, &component);
        if (result != 0) {
            printf("\n    Error: Component registration failed at iteration %d", i);
            etps_context_destroy(ctx);
            return 0;
        }
    }
    
    etps_context_destroy(ctx);
    return 1;
}

// =============================================================================
// Main Test Runner
// =============================================================================

int main() {
    printf("===============================================================\n");
    printf("TDD Test Suite: ETPS Telemetry System\n");
    printf("Phase: VALIDATION (Testing Existing Implementation)\n");
    printf("Methodology: Systematic Functional Testing\n");
    printf("===============================================================\n\n");
    
    // Initialize ETPS for testing
    if (etps_init() != 0) {
        printf("❌ CRITICAL: Failed to initialize ETPS system\n");
        return 1;
    }
    
    // Core functionality tests
    RUN_TEST(test_etps_initialization);
    RUN_TEST(test_etps_context_creation);
    RUN_TEST(test_etps_context_null_handling);
    RUN_TEST(test_etps_semverx_component_registration);
    RUN_TEST(test_etps_compatibility_validation);
    RUN_TEST(test_etps_utility_functions);
    RUN_TEST(test_etps_validation_functions);
    
    // Performance and stress tests
    RUN_TEST(test_etps_performance_context_creation);
    RUN_TEST(test_etps_stress_component_registration);
    
    // Cleanup tests (run last)
    RUN_TEST(test_etps_cleanup);
    
    // Test summary
    printf("\n===============================================================\n");
    printf("Test Results Summary:\n");
    printf("  Total Tests: %d\n", test_count);
    printf("  Passed: %d\n", passed_tests);
    printf("  Failed: %d\n", failed_tests);
    printf("  Success Rate: %.2f%%\n", (float)passed_tests / test_count * 100);
    printf("===============================================================\n");
    
    if (failed_tests == 0) {
        printf("🎉 All tests passed! ETPS implementation validation successful.\n");
        return 0;
    } else {
        printf("⚠️  %d test(s) failed. Implementation requires attention.\n", failed_tests);
        return 1;
    }
}
