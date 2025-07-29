/**
 * @file test_common.h
 * @brief Common utilities for NexusLink tests
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef TEST_COMMON_H
 #define TEST_COMMON_H
 
 #include <stdbool.h>
 #include <stddef.h>
 #include <unistd.h>
 
 /**
  * Initialize a test
  * 
  * @param test_name Name of the test
  */
 void test_init(const char* test_name);
 
 /**
  * Finalize a test and return appropriate exit code
  * 
  * @return EXIT_SUCCESS if all tests passed, EXIT_FAILURE otherwise
  */
 int test_finalize();
 
 /**
  * Assert a condition and print result
  * 
  * @param description Description of the test
  * @param condition Condition to assert
  */
 void test_assert(const char* description, bool condition);
 
 /**
  * Create a temporary file with specified content
  * 
  * @param content Content to write to the file, or NULL for empty file
  * @return Path to the created file, caller must free
  */
 char* create_temp_file(const char* content);
 
 /**
  * Create a mock shared library for testing
  * 
  * @param name Name of the library
  * @param symbol_name Name of the symbol to expose
  * @param symbol_address Address of the symbol
  * @return Path to the created library, caller must free
  */
 char* create_mock_library(const char* name, const char* symbol_name, void* symbol_address);
 
 /**
  * Mock function for symbol resolution testing
  */
 void mock_function();
 
 #endif // TEST_COMMON_H