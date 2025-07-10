/**
 * @file test_assert.h
 * @brief NexusLink Testing Assertion Framework
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 */

#ifndef NLINK_TEST_ASSERT_H
#define NLINK_TEST_ASSERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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

// Test configuration templates
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
  "enable_work_stealing = true\n"

#endif // NLINK_TEST_ASSERT_H
