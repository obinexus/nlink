/**
 * @file test_assert.h
 * @brief NexusLink Testing Assertion Framework
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Comprehensive assertion library for NexusLink POC validation and quality assurance.
 * Implements systematic testing patterns with detailed failure reporting.
 */

#ifndef NLINK_TEST_ASSERT_H
#define NLINK_TEST_ASSERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// TEST ASSERTION MACROS
// =============================================================================

/**
 * @brief Basic assertion with failure reporting
 */
#define NLINK_TEST_ASSERT(cond, msg) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  Condition: %s\n", #cond); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

/**
 * @brief String equality assertion
 */
#define NLINK_TEST_ASSERT_STR_EQ(actual, expected, msg) \
  do { \
    if (strcmp((actual), (expected)) != 0) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  Expected: \"%s\"\n", expected); \
      fprintf(stderr, "  Actual:   \"%s\"\n", actual); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

/**
 * @brief Integer equality assertion
 */
#define NLINK_TEST_ASSERT_INT_EQ(actual, expected, msg) \
  do { \
    if ((actual) != (expected)) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  Expected: %d\n", expected); \
      fprintf(stderr, "  Actual:   %d\n", actual); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

/**
 * @brief Pointer non-null assertion
 */
#define NLINK_TEST_ASSERT_NOT_NULL(ptr, msg) \
  do { \
    if ((ptr) == NULL) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  Pointer is NULL\n"); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

/**
 * @brief Configuration result assertion
 */
#define NLINK_TEST_ASSERT_CONFIG_SUCCESS(result, msg) \
  do { \
    if ((result) != NLINK_CONFIG_SUCCESS) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  Config result: %d (expected: NLINK_CONFIG_SUCCESS)\n", result); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

/**
 * @brief CLI result assertion
 */
#define NLINK_TEST_ASSERT_CLI_SUCCESS(result, msg) \
  do { \
    if ((result) != NLINK_CLI_SUCCESS) { \
      fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
      fprintf(stderr, "  CLI result: %d (expected: NLINK_CLI_SUCCESS)\n", result); \
      exit(1); \
    } else { \
      fprintf(stdout, "[ASSERT PASS] %s\n", msg); \
    } \
  } while(0)

// =============================================================================
// TEST SUITE MANAGEMENT
// =============================================================================

typedef struct {
  char test_name[128];
  int tests_run;
  int tests_passed;
  int tests_failed;
  bool verbose_output;
} nlink_test_context_t;

/**
 * @brief Initialize test context
 */
#define NLINK_TEST_INIT(ctx, name) \
  do { \
    strncpy((ctx)->test_name, (name), sizeof((ctx)->test_name) - 1); \
    (ctx)->test_name[sizeof((ctx)->test_name) - 1] = '\0'; \
    (ctx)->tests_run = 0; \
    (ctx)->tests_passed = 0; \
    (ctx)->tests_failed = 0; \
    (ctx)->verbose_output = true; \
    printf("=== Starting Test Suite: %s ===\n", (ctx)->test_name); \
  } while(0)

/**
 * @brief Run a test function with context tracking
 */
#define NLINK_TEST_RUN(ctx, test_func) \
  do { \
    printf("\n--- Running: %s ---\n", #test_func); \
    (ctx)->tests_run++; \
    test_func(); \
    (ctx)->tests_passed++; \
    printf("--- %s: PASSED ---\n", #test_func); \
  } while(0)

/**
 * @brief Display test suite results
 */
#define NLINK_TEST_RESULTS(ctx) \
  do { \
    printf("\n=== Test Suite Results: %s ===\n", (ctx)->test_name); \
    printf("Tests Run:    %d\n", (ctx)->tests_run); \
    printf("Tests Passed: %d\n", (ctx)->tests_passed); \
    printf("Tests Failed: %d\n", (ctx)->tests_failed); \
    if ((ctx)->tests_failed == 0) { \
      printf("✅ ALL TESTS PASSED\n"); \
    } else { \
      printf("❌ %d TEST(S) FAILED\n", (ctx)->tests_failed); \
    } \
    printf("==============================\n"); \
  } while(0)

// =============================================================================
// CONFIGURATION TESTING UTILITIES
// =============================================================================

/**
 * @brief Create temporary test configuration file
 */
static inline void nlink_test_create_pkg_config(const char* path, const char* content) {
  FILE* f = fopen(path, "w");
  NLINK_TEST_ASSERT_NOT_NULL(f, "Failed to create test config file");
  fprintf(f, "%s", content);
  fclose(f);
}

/**
 * @brief Clean up test configuration file
 */
static inline void nlink_test_cleanup_file(const char* path) {
  if (remove(path) != 0) {
    printf("[WARNING] Failed to cleanup test file: %s\n", path);
  }
}

/**
 * @brief Standard test configuration template
 */
#define NLINK_TEST_PKG_CONFIG_TEMPLATE \
  "[project]\n" \
  "name = test_project\n" \
  "version = 1.0.0\n" \
  "entry_point = main.c\n" \
  "\n" \
  "[build]\n" \
  "pass_mode = single\n" \
  "experimental_mode = false\n" \
  "strict_mode = true\n" \
  "\n" \
  "[threading]\n" \
  "worker_count = 4\n" \
  "queue_depth = 64\n" \
  "stack_size_kb = 512\n" \
  "enable_work_stealing = true\n" \
  "\n" \
  "[features]\n" \
  "unicode_normalization = true\n" \
  "isomorphic_reduction = true\n" \
  "debug_symbols = false\n"

/**
 * @brief Standard component configuration template
 */
#define NLINK_TEST_COMPONENT_CONFIG_TEMPLATE \
  "[component]\n" \
  "name = test_component\n" \
  "version = 1.0.0\n" \
  "\n" \
  "[compilation]\n" \
  "optimization_level = 2\n" \
  "max_compile_time = 60\n" \
  "parallel_allowed = true\n"

#endif // NLINK_TEST_ASSERT_H
