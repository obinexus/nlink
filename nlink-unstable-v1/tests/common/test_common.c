/**
 * @file test_common.c
 * @brief Common utilities for NexusLink tests
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 
 // Global test counters
 static int total_tests = 0;
 static int passed_tests = 0;
 
 // Initialize test
 void test_init(const char* test_name) {
     printf("\n=== Running Test: %s ===\n", test_name);
     total_tests = 0;
     passed_tests = 0;
 }
 
 // Finalize test and return appropriate exit code
 int test_finalize() {
     printf("\n=== Test Summary ===\n");
     printf("Passed: %d/%d\n", passed_tests, total_tests);
     printf("===================\n");
     
     return (passed_tests == total_tests) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
 
 // Assert a condition and print result
 void test_assert(const char* description, bool condition) {
     total_tests++;
     
     if (condition) {
         passed_tests++;
         printf("[PASS] %s\n", description);
     } else {
         printf("[FAIL] %s\n", description);
     }
 }
 
 // Create a temporary file with specified content
 char* create_temp_file(const char* content) {
     char* filename = strdup("/tmp/nexuslink_test_XXXXXX");
     int fd = mkstemp(filename);
     
     if (fd == -1) {
         free(filename);
         return NULL;
     }
     
     if (content) {
         write(fd, content, strlen(content));
     }
     
     close(fd);
     return filename;
 }
 
 // Create a mock shared library for testing
 char* create_mock_library(const char* name, const char* symbol_name, void* symbol_address) {
     // This function would dynamically create a shared library
     // For simplicity, we're just returning a predefined path
     // In a real implementation, this would compile and link a shared library
     
     (void)name;      // Unused parameter
     (void)symbol_name;   // Unused parameter
     (void)symbol_address;   // Unused parameter
     
     return strdup("./libmock.so");
 }
 
 // Mock function for symbol resolution testing
 void mock_function() {
     printf("Mock function called\n");
 }