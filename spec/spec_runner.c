/**
 * @file spec_runner.c
 * @brief OBINexus QA Specification Runner
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "nlink/core/etps/telemetry.h"

// QA Spec Framework
typedef enum {
    SPEC_PASS,
    SPEC_FAIL,
    SPEC_SKIP,
    SPEC_PENDING
} spec_result_t;

typedef struct {
    const char* description;
    spec_result_t (*test_fn)(void);
    spec_result_t result;
    double duration_ms;
    char* failure_message;
} spec_test_t;

typedef struct {
    const char* suite_name;
    spec_test_t* tests;
    size_t test_count;
    size_t capacity;
    etps_context_t* etps_ctx;
} spec_suite_t;

// Spec Helpers
#define SPEC_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            spec_current_test->failure_message = strdup(message); \
            return SPEC_FAIL; \
        } \
    } while(0)

#define SPEC_EXPECT_EQ(actual, expected) \
    SPEC_ASSERT((actual) == (expected), "Expected equality")

#define SPEC_EXPECT_STR_EQ(actual, expected) \
    SPEC_ASSERT(strcmp((actual), (expected)) == 0, "Expected string equality")

// Global current test pointer for assertions
static spec_test_t* spec_current_test = NULL;

// Create spec suite
spec_suite_t* spec_suite_create(const char* name) {
    spec_suite_t* suite = calloc(1, sizeof(spec_suite_t));
    suite->suite_name = name;
    suite->capacity = 100;
    suite->tests = calloc(suite->capacity, sizeof(spec_test_t));
    suite->etps_ctx = etps_context_create(name);
    return suite;
}

// Add test to suite
void spec_add_test(spec_suite_t* suite, 
                   const char* description,
                   spec_result_t (*test_fn)(void)) {
    if (suite->test_count < suite->capacity) {
        spec_test_t* test = &suite->tests[suite->test_count++];
        test->description = description;
        test->test_fn = test_fn;
        test->result = SPEC_PENDING;
    }
}

// Run spec suite
int spec_suite_run(spec_suite_t* suite) {
    printf("\n=== Running Spec Suite: %s ===\n", suite->suite_name);
    
    int passed = 0, failed = 0, skipped = 0;
    
    for (size_t i = 0; i < suite->test_count; i++) {
        spec_test_t* test = &suite->tests[i];
        spec_current_test = test;
        
        printf("  [%zu] %s ... ", i + 1, test->description);
        fflush(stdout);
        
        clock_t start = clock();
        test->result = test->test_fn();
        clock_t end = clock();
        
        test->duration_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
        
        switch (test->result) {
            case SPEC_PASS:
                printf("✅ PASS (%.2fms)\n", test->duration_ms);
                passed++;
                break;
            case SPEC_FAIL:
                printf("❌ FAIL (%.2fms)\n", test->duration_ms);
                if (test->failure_message) {
                    printf("      Reason: %s\n", test->failure_message);
                }
                failed++;
                break;
            case SPEC_SKIP:
                printf("⏭️  SKIP\n");
                skipped++;
                break;
            default:
                printf("⏸️  PENDING\n");
                break;
        }
        
        // Log to ETPS
        etps_log_info(suite->etps_ctx, ETPS_COMPONENT_CORE,
                     "spec_test", test->description);
    }
    
    printf("\n=== Summary ===\n");
    printf("Total: %zu | Passed: %d | Failed: %d | Skipped: %d\n",
           suite->test_count, passed, failed, skipped);
    
    return failed == 0 ? 0 : 1;
}

// Destroy spec suite
void spec_suite_destroy(spec_suite_t* suite) {
    for (size_t i = 0; i < suite->test_count; i++) {
        free(suite->tests[i].failure_message);
    }
    free(suite->tests);
    etps_context_destroy(suite->etps_ctx);
    free(suite);
}
