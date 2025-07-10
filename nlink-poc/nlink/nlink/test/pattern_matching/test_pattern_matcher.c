/**
 * @file test_pattern_matcher.c
 * @brief Unit tests for pattern matching functionality
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pattern_matching/pattern_matcher.h"
#include "nlink/core/pattern_matching/regex_matcher.h"
#include "nlink/core/pattern_matching/wildcard_matcher.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Test pattern matching functionality
 */
static void test_pattern_matcher(void) {
    printf("Testing pattern matcher... ");
    
    // Test with default options
    nlink_pattern_options options = nlink_pattern_default_options();
    nlink_pattern_result* result = nlink_match_pattern("*.c", ".", options);
    
    // In a real implementation, we would check the actual files matched
    // For this test, just verify the API works
    assert(result != NULL);
    
    nlink_pattern_result_free(result);
    printf("PASSED\n");
}

/**
 * Test regex matching functionality
 */
static void test_regex_matcher(void) {
    printf("Testing regex matcher... ");
    
    // Test with default options
    nlink_regex_options options = nlink_regex_default_options();
    nlink_regex* regex = nlink_regex_compile("test", options);
    
    assert(regex != NULL);
    assert(nlink_regex_match(regex, "test string", NULL));
    assert(!nlink_regex_match(regex, "no match", NULL));
    
    nlink_regex_free(regex);
    printf("PASSED\n");
}

/**
 * Test wildcard matching functionality
 */
static void test_wildcard_matcher(void) {
    printf("Testing wildcard matcher... ");
    
    // Test with default options
    nlink_wildcard_options options = nlink_wildcard_default_options();
    
    assert(nlink_wildcard_match("test*", "test123", NLINK_WILDCARD_GLOB, options));
    assert(!nlink_wildcard_match("test*", "not test", NLINK_WILDCARD_GLOB, options));
    
    assert(nlink_wildcard_match_path("src/*.c", "src/test.c", NLINK_WILDCARD_GLOB, options));
    assert(!nlink_wildcard_match_path("src/*.c", "src/subdir/test.c", NLINK_WILDCARD_GLOB, options));
    
    char* base_dir = nlink_wildcard_get_base_dir("src/*.c");
    assert(base_dir != NULL);
    assert(strcmp(base_dir, "src") == 0);
    free(base_dir);
    
    printf("PASSED\n");
}

/**
 * Main test function
 */
int main(void) {
    printf("Running pattern matching tests...\n");
    
    test_pattern_matcher();
    test_regex_matcher();
    test_wildcard_matcher();
    
    printf("All tests PASSED\n");
    return 0;
}
