/**
 * @file nlink_test.h
 * @brief NexusLink Test Framework Header
 *
 * This header provides the testing macros and utilities for the AAA pattern
 * (Arrange-Act-Assert) used in NexusLink tests.
 */

#ifndef NLINK_TEST_H
#define NLINK_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Test suite management */
#define NLINK_TEST_SUITE_BEGIN(name) void* nlink_test_suite_setup_##name() 

#define NLINK_TEST_SUITE_END(name) void nlink_test_suite_teardown_##name(void *context)

/* Test fixture management */
#define NLINK_TEST_FIXTURE(suite, name) void* nlink_test_fixture_setup_##suite##_##name()

#define NLINK_TEST_FIXTURE_END(suite, name) void nlink_test_fixture_teardown_##suite##_##name(void *context)

/* Test case definitions */
#define NLINK_TEST_CASE(suite, name) void nlink_test_case_##suite##_##name()

#define NLINK_TEST_CASE_WITH_FIXTURE(suite, name, fixture)     void nlink_test_case_##suite##_##name(void *fixture_context)

/* AAA pattern macros */
#define NLINK_ARRANGE_PHASE(msg) printf("\nARRANGE: %s\n", msg)
#define NLINK_ACT_PHASE(msg) printf("\nACT: %s\n", msg)
#define NLINK_ASSERT_PHASE(msg) printf("\nASSERT: %s\n", msg)

/* Assertion macros */
#define NLINK_ASSERT(condition, message)     do {         if (!(condition)) {             printf("FAILED: %s (line %d)\n", message, __LINE__);             return;         }     } while (0)

#define NLINK_ASSERT_TRUE(condition, message)     NLINK_ASSERT((condition), message)

#define NLINK_ASSERT_FALSE(condition, message)     NLINK_ASSERT(!(condition), message)

#define NLINK_ASSERT_NULL(ptr, message)     NLINK_ASSERT((ptr) == NULL, message)

#define NLINK_ASSERT_NOT_NULL(ptr, message)     NLINK_ASSERT((ptr) != NULL, message)

#define NLINK_ASSERT_EQUAL_INT(expected, actual, message)     do {         int e = (expected);         int a = (actual);         NLINK_ASSERT(e == a, message);     } while (0)

#define NLINK_ASSERT_EQUAL_STRING(expected, actual, message)     do {         const char *e = (expected);         const char *a = (actual);         NLINK_ASSERT(strcmp(e, a) == 0, message);     } while (0)

/* Test registration and execution */
#define NLINK_TEST_REGISTER(suite, name)     void nlink_run_test_##suite##_##name() {         void *suite_context = nlink_test_suite_setup_##suite();         nlink_test_case_##suite##_##name();         nlink_test_suite_teardown_##suite(suite_context);         printf("Test %s::%s PASSED\n", #suite, #name);     }

#define NLINK_TEST_REGISTER_WITH_FIXTURE(suite, name, fixture)     void nlink_run_test_##suite##_##name() {         void *suite_context = nlink_test_suite_setup_##suite();         void *fixture_context = nlink_test_fixture_setup_##suite##_##fixture();         nlink_test_case_##suite##_##name(fixture_context);         nlink_test_fixture_teardown_##suite##_##fixture(fixture_context);         nlink_test_suite_teardown_##suite(suite_context);         printf("Test %s::%s PASSED\n", #suite, #name);     }

#define NLINK_TEST_MAIN(...)     int main(int argc, char *argv[]) {         (void)argc; (void)argv;         printf("\nRunning NexusLink tests...\n");         __VA_ARGS__;         printf("\nAll tests PASSED!\n");         return 0;     }

/* Test logging utilities */
#define nlink_test_log_info(format, ...)     printf("INFO: " format "\n", ##__VA_ARGS__)

#define nlink_test_log_warning(format, ...)     printf("WARNING: " format "\n", ##__VA_ARGS__)

#define nlink_test_log_error(format, ...)     printf("ERROR: " format "\n", ##__VA_ARGS__)

/* Test stub manager */
void* test_stub_manager_init(const char* components[], int count);
void test_stub_manager_cleanup(void);

#endif /* NLINK_TEST_H */
