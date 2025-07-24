/**
 * @file test_config_parsing_lib.c
 * @brief Unit tests using production NexusLink library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Include production library headers
#include "nlink_qa_poc/nlink.h"
#include "nlink_qa_poc/core/config.h"

// Test framework macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s\n", message); \
            return 0; \
        } \
        printf("PASS: %s\n", message); \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running %s...\n", #test_func); \
        if (!test_func()) { \
            printf("Test %s FAILED\n", #test_func); \
            return 1; \
        } \
        printf("Test %s PASSED\n\n", #test_func); \
    } while(0)

int test_library_initialization() {
    printf("Testing library initialization...\n");
    
    // Test library initialization
    int result = nlink_init();
    TEST_ASSERT(result == 0, "Library initialization");
    
    // Test version retrieval
    const char* version = nlink_version();
    TEST_ASSERT(version != NULL, "Version retrieval");
    TEST_ASSERT(strcmp(version, "1.0.0") == 0, "Version string");
    
    // Cleanup
    nlink_cleanup();
    
    return 1;
}

int test_config_functions() {
    printf("Testing configuration functions...\n");
    
    nlink_config_t config;
    
    // Test configuration initialization
    int result = nlink_config_init(&config);
    TEST_ASSERT(result == 0, "Configuration initialization");
    
    // Test default values
    TEST_ASSERT(strcmp(config.project_name, "nlink_qa_poc") == 0, 
                "Default project name");
    TEST_ASSERT(strcmp(config.version, "1.0.0") == 0, 
                "Default version");
    TEST_ASSERT(config.worker_count == 4, 
                "Default worker count");
    
    // Test configuration validation
    result = nlink_config_validate(&config);
    TEST_ASSERT(result == 0, "Configuration validation");
    
    // Cleanup
    nlink_config_cleanup(&config);
    
    return 1;
}

int test_parsing_functions() {
    printf("Testing parsing functions...\n");
    
    char output[128];
    uint32_t worker_count;
    
    // Test project name parsing
    int result = nlink_parse_project_name("test_project", output, sizeof(output));
    TEST_ASSERT(result == 0, "Project name parsing");
    TEST_ASSERT(strcmp(output, "test_project") == 0, "Parsed project name");
    
    // Test version parsing
    result = nlink_parse_version("2.0.0", output, sizeof(output));
    TEST_ASSERT(result == 0, "Version parsing");
    TEST_ASSERT(strcmp(output, "2.0.0") == 0, "Parsed version");
    
    // Test worker count parsing
    result = nlink_parse_worker_count("8", &worker_count);
    TEST_ASSERT(result == 0, "Worker count parsing");
    TEST_ASSERT(worker_count == 8, "Parsed worker count");
    
    return 1;
}

int main() {
    printf("NexusLink Library-Linked Unit Test Suite\n");
    printf("=========================================\n\n");
    
    RUN_TEST(test_library_initialization);
    RUN_TEST(test_config_functions);
    RUN_TEST(test_parsing_functions);
    
    printf("All library-linked unit tests passed!\n");
    return 0;
}
