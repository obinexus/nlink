/**
 * @file matcher_test.c
 * @brief Test suite for the NexusLink pattern matcher
 * 
 * Comprehensive tests for the pattern matcher including regex, glob,
 * and literal string matching.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/pattern/matcher.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 
 /**
  * @brief Test result structure
  */
 typedef struct {
     int passed;
     int failed;
     int total;
 } TestResults;
 
 /**
  * @brief Initialize test results
  * 
  * @param results Test results structure
  */
 static void init_test_results(TestResults* results) {
     results->passed = 0;
     results->failed = 0;
     results->total = 0;
 }
 
 /**
  * @brief Report a test result
  * 
  * @param results Test results structure
  * @param name Test name
  * @param success Whether the test passed
  */
 static void report_test(TestResults* results, const char* name, bool success) {
     results->total++;
     
     if (success) {
         results->passed++;
         printf("✓ %s\n", name);
     } else {
         results->failed++;
         printf("✗ %s\n", name);
     }
 }
 
 /**
  * @brief Run a single pattern test
  * 
  * @param results Test results structure
  * @param name Test name
  * @param pattern Pattern string
  * @param flags Pattern flags
  * @param test_string String to match
  * @param expected_result Expected match result
  */
 static void test_pattern(TestResults* results, const char* name, const char* pattern, 
                         NlinkPatternFlags flags, const char* test_string, bool expected_result) {
     // Create the matcher
     NlinkPatternMatcher* matcher = nlink_pattern_create(pattern, flags);
     assert(matcher != NULL);
     
     // Check pattern and flags
     assert(strcmp(nlink_pattern_get_pattern(matcher), pattern) == 0);
     assert(nlink_pattern_get_flags(matcher) == flags);
     
     // Match the string
     bool result = nlink_pattern_match(matcher, test_string);
     
     // Report the result
     report_test(results, name, result == expected_result);
     
     // Free the matcher
     nlink_pattern_destroy(matcher);
 }
 
 /**
  * @brief Test literal string matching
  * 
  * @param results Test results structure
  */
 static void test_literal_matching(TestResults* results) {
     printf("\n=== Literal String Matching ===\n");
     
     // Exact match
     test_pattern(results, "Literal exact match", 
                 "hello", NLINK_PATTERN_FLAG_NONE, "hello", true);
     
     // Mismatch
     test_pattern(results, "Literal mismatch", 
                 "hello", NLINK_PATTERN_FLAG_NONE, "world", false);
     
     // Case insensitive match
     test_pattern(results, "Literal case insensitive match", 
                 "Hello", NLINK_PATTERN_FLAG_CASE_INSENSITIVE, "hello", true);
     
     // Case sensitive mismatch
     test_pattern(results, "Literal case sensitive mismatch", 
                 "Hello", NLINK_PATTERN_FLAG_NONE, "hello", false);
     
     // Empty string match
     test_pattern(results, "Empty string match", 
                 "", NLINK_PATTERN_FLAG_NONE, "", true);
     
     // Empty string mismatch
     test_pattern(results, "Empty string mismatch", 
                 "", NLINK_PATTERN_FLAG_NONE, "nonempty", false);
 }
 
 /**
  * @brief Test glob pattern matching
  * 
  * @param results Test results structure
  */
 static void test_glob_matching(TestResults* results) {
     printf("\n=== Glob Pattern Matching ===\n");
     
     // Simple wildcard match
     test_pattern(results, "Simple wildcard match", 
                 "hello*", NLINK_PATTERN_FLAG_GLOB, "hello world", true);
     
     // Simple wildcard mismatch
     test_pattern(results, "Simple wildcard mismatch", 
                 "hello*", NLINK_PATTERN_FLAG_GLOB, "world hello", false);
     
     // Multiple wildcards
     test_pattern(results, "Multiple wildcards", 
                 "*hello*world*", NLINK_PATTERN_FLAG_GLOB, "say hello to the world please", true);
     
     // Question mark wildcard
     test_pattern(results, "Question mark wildcard", 
                 "h?llo", NLINK_PATTERN_FLAG_GLOB, "hello", true);
     
     // Question mark wildcard mismatch
     test_pattern(results, "Question mark wildcard mismatch", 
                 "h?llo", NLINK_PATTERN_FLAG_GLOB, "heello", false);
     
     // Character class match
     test_pattern(results, "Character class match", 
                 "h[aeiou]llo", NLINK_PATTERN_FLAG_GLOB, "hallo", true);
     
     // Character class match alt
     test_pattern(results, "Character class match alt", 
                 "h[aeiou]llo", NLINK_PATTERN_FLAG_GLOB, "hello", true);
     
     // Character class mismatch
     test_pattern(results, "Character class mismatch", 
                 "h[aeiou]llo", NLINK_PATTERN_FLAG_GLOB, "hyllo", false);
     
     // Character class range
     test_pattern(results, "Character class range", 
                 "h[a-z]llo", NLINK_PATTERN_FLAG_GLOB, "hello", true);
     
     // Negated character class
     test_pattern(results, "Negated character class", 
                 "h[^aeiou]llo", NLINK_PATTERN_FLAG_GLOB, "hyllo", true);
     
     // Negated character class mismatch
     test_pattern(results, "Negated character class mismatch", 
                 "h[^aeiou]llo", NLINK_PATTERN_FLAG_GLOB, "hello", false);
     
     // Complex glob pattern
     test_pattern(results, "Complex glob pattern", 
                 "a*b?c[de]f*g", NLINK_PATTERN_FLAG_GLOB, "axxxbycefxxxg", true);
     
     // Case insensitive glob match
     test_pattern(results, "Case insensitive glob match", 
                 "Hello*", NLINK_PATTERN_FLAG_GLOB | NLINK_PATTERN_FLAG_CASE_INSENSITIVE, 
                 "hello world", true);
 }
 
 /**
  * @brief Test regex pattern matching
  * 
  * @param results Test results structure
  */
 static void test_regex_matching(TestResults* results) {
     printf("\n=== Regex Pattern Matching ===\n");
     
     // Simple regex match
     test_pattern(results, "Simple regex match", 
                 "hello.*", NLINK_PATTERN_FLAG_REGEX, "hello world", true);
     
     // Regex with anchors
     test_pattern(results, "Regex with anchors", 
                 "^hello$", NLINK_PATTERN_FLAG_REGEX, "hello", true);
     
     // Regex with anchors mismatch
     test_pattern(results, "Regex with anchors mismatch", 
                 "^hello$", NLINK_PATTERN_FLAG_REGEX, "hello world", false);
     
     // Character classes
     test_pattern(results, "Character classes", 
                 "h[aeiou]llo", NLINK_PATTERN_FLAG_REGEX, "hello", true);
     
     // Alternation
     test_pattern(results, "Alternation", 
                 "cat|dog", NLINK_PATTERN_FLAG_REGEX, "dog", true);
     
     // Alternation mismatch
     test_pattern(results, "Alternation mismatch", 
                 "cat|dog", NLINK_PATTERN_FLAG_REGEX, "bird", false);
     
     // Repetition
     test_pattern(results, "Repetition", 
                 "a{2,4}", NLINK_PATTERN_FLAG_REGEX, "aaa", true);
     
     // Repetition mismatch
     test_pattern(results, "Repetition mismatch", 
                 "a{2,4}", NLINK_PATTERN_FLAG_REGEX, "a", false);
     
     // Plus operator
     test_pattern(results, "Plus operator", 
                 "ca+t", NLINK_PATTERN_FLAG_REGEX, "caaat", true);
     
     // Plus operator mismatch
     test_pattern(results, "Plus operator mismatch", 
                 "ca+t", NLINK_PATTERN_FLAG_REGEX, "ct", false);
     
     // Question mark operator
     test_pattern(results, "Question mark operator", 
                 "colou?r", NLINK_PATTERN_FLAG_REGEX, "color", true);
     
     // Question mark operator alt
     test_pattern(results, "Question mark operator alt", 
                 "colou?r", NLINK_PATTERN_FLAG_REGEX, "colour", true);
     
     // Grouping
     test_pattern(results, "Grouping", 
                 "(ab)+", NLINK_PATTERN_FLAG_REGEX, "ababab", true);
     
     // Case insensitive regex
     test_pattern(results, "Case insensitive regex", 
                 "HELLO", NLINK_PATTERN_FLAG_REGEX | NLINK_PATTERN_FLAG_CASE_INSENSITIVE, 
                 "hello", true);
     
     // Complex regex pattern
     test_pattern(results, "Complex regex pattern", 
                 "^(https?|ftp)://[^\\s/$.?#].[^\\s]*$", NLINK_PATTERN_FLAG_REGEX,
                 "https://example.com/path", true);
     
     // Extended flag test
     test_pattern(results, "Extended regex flag test", 
                 "hello|world", NLINK_PATTERN_FLAG_EXTENDED, "world", true);
 }
 
 /**
  * @brief Test edge cases
  * 
  * @param results Test results structure
  */
 static void test_edge_cases(TestResults* results) {
     printf("\n=== Edge Cases ===\n");
     
     // Auto-detect regex pattern
     test_pattern(results, "Auto-detect regex pattern", 
                 "hello|world", NLINK_PATTERN_FLAG_NONE, "world", true);
     
     // Auto-detect glob pattern
     test_pattern(results, "Auto-detect glob pattern", 
                 "hello*", NLINK_PATTERN_FLAG_NONE, "hello world", true);
     
     // Force regex for glob pattern
     test_pattern(results, "Force regex for glob pattern", 
                 "hello.*", NLINK_PATTERN_FLAG_REGEX, "hello world", true);
     
     // Regex escape sequence
     test_pattern(results, "Regex escape sequence", 
                 "\\d+", NLINK_PATTERN_FLAG_REGEX, "12345", true);
     
     // Regex escape sequence mismatch
     test_pattern(results, "Regex escape sequence mismatch", 
                 "\\d+", NLINK_PATTERN_FLAG_REGEX, "abcde", false);
     
     // Very long pattern
     const char* long_pattern = "a*b*c*d*e*f*g*h*i*j*k*l*m*n*o*p*q*r*s*t*u*v*w*x*y*z*";
     const char* long_string = "abcdefghijklmnopqrstuvwxyz";
     test_pattern(results, "Very long pattern", 
                 long_pattern, NLINK_PATTERN_FLAG_GLOB, long_string, true);
     
     // Special characters in literal match
     test_pattern(results, "Special characters in literal match", 
                 "hello.world", NLINK_PATTERN_FLAG_NONE, "hello.world", true);
     
     // Special characters in literal mismatch
     test_pattern(results, "Special characters in literal mismatch", 
                 "hello.world", NLINK_PATTERN_FLAG_NONE, "helloxworld", false);
 }
 
 /**
  * @brief Print test summary
  * 
  * @param results Test results structure
  */
 static void print_summary(const TestResults* results) {
     printf("\n=== Test Summary ===\n");
     printf("Total tests: %d\n", results->total);
     printf("Passed: %d\n", results->passed);
     printf("Failed: %d\n", results->failed);
     printf("Success rate: %.2f%%\n", (float)results->passed / results->total * 100);
 }
 
 /**
  * @brief Main entry point
  * 
  * @return int Exit code (0 for success, non-zero for failure)
  */
 int main(int argc, char** argv) {
     printf("NexusLink Pattern Matcher Test Suite\n");
     printf("====================================\n");
     
     TestResults results;
     init_test_results(&results);
     
     // Run test suites
     test_literal_matching(&results);
     test_glob_matching(&results);
     test_regex_matching(&results);
     test_edge_cases(&results);
     
     // Print summary
     print_summary(&results);
     
     // Return success if all tests passed
     return (results.failed > 0) ? 1 : 0;
 }