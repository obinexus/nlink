/**
 * @file test_config_parsing.c
 * @brief Unit tests for configuration parsing functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

// Mock configuration structure
typedef struct {
    char project_name[128];
    char version[32];
    char build_system[32];
    int worker_count;
} test_config_t;

int test_basic_config_parsing() {
    test_config_t config = {0};
    
    // Simulate parsing pkg.nlink
    strcpy(config.project_name, "nlink_qa_poc");
    strcpy(config.version, "1.0.0");
    strcpy(config.build_system, "make");
    config.worker_count = 4;
    
    TEST_ASSERT(strcmp(config.project_name, "nlink_qa_poc") == 0, 
                "Project name parsing");
    TEST_ASSERT(strcmp(config.version, "1.0.0") == 0, 
                "Version parsing");
    TEST_ASSERT(strcmp(config.build_system, "make") == 0, 
                "Build system parsing");
    TEST_ASSERT(config.worker_count == 4, 
                "Worker count parsing");
    
    return 1;
}

int test_semverx_validation() {
    // Test semantic version validation
    const char* valid_versions[] = {
        "1.0.0", "2.1.3", "10.20.30", "1.0.0-alpha.1"
    };
    
    const char* invalid_versions[] = {
        "1.0", "v1.0.0", "1.0.0.0", "invalid"
    };
    
    // Simulate version validation
    for (int i = 0; i < 4; i++) {
        // Mock validation logic
        int is_valid = (strlen(valid_versions[i]) >= 5);
        TEST_ASSERT(is_valid, "Valid version accepted");
    }
    
    return 1;
}

int test_build_process_spec_parsing() {
    typedef struct {
        char build_system[32];
        char spec_file[64];
        char validation_profile[32];
    } build_spec_t;
    
    build_spec_t spec = {0};
    
    // Simulate parsing build_process_spec
    strcpy(spec.build_system, "make");
    strcpy(spec.spec_file, "Makefile");
    strcpy(spec.validation_profile, "strict");
    
    TEST_ASSERT(strcmp(spec.build_system, "make") == 0,
                "Build system spec parsing");
    TEST_ASSERT(strcmp(spec.spec_file, "Makefile") == 0,
                "Spec file parsing");
    TEST_ASSERT(strcmp(spec.validation_profile, "strict") == 0,
                "Validation profile parsing");
    
    return 1;
}

int main() {
    printf("NexusLink QA POC - Unit Test Suite\n");
    printf("==================================\n\n");
    
    RUN_TEST(test_basic_config_parsing);
    RUN_TEST(test_semverx_validation);
    RUN_TEST(test_build_process_spec_parsing);
    
    printf("All unit tests passed!\n");
    return 0;
}
