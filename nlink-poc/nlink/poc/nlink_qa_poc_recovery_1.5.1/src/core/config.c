/**
 * NexusLink QA POC - Core Configuration Implementation
 * CRITICAL FIX: Includes stddef.h to resolve size_t compilation errors
 */

// CRITICAL FIX: Include stddef.h FIRST to resolve size_t compilation errors
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "nlink_qa_poc/core/config.h"

int nlink_parse_project_name(const char* input, char* output, size_t max_len) {
    if (!input || !output || max_len == 0) {
        return -1;
    }
    
    size_t input_len = strlen(input);
    if (input_len >= max_len) {
        return -1;
    }
    
    strncpy(output, input, max_len - 1);
    output[max_len - 1] = '\0';
    return 0;
}

int nlink_parse_version(const char* input, char* output, size_t max_len) {
    if (!input || !output || max_len == 0) {
        return -1;
    }
    
    size_t input_len = strlen(input);
    if (input_len >= max_len) {
        return -1;
    }
    
    strncpy(output, input, max_len - 1);
    output[max_len - 1] = '\0';
    return 0;
}

int nlink_load_config(const char* filename, nlink_config_t* config) {
    if (!filename || !config) {
        return -1;
    }
    
    // Initialize config with defaults
    memset(config, 0, sizeof(nlink_config_t));
    strcpy(config->project_name, "nlink_qa_poc");
    strcpy(config->version, "1.0.0");
    strcpy(config->entry_point, "src/main.c");
    
    return 0;
}

// Add missing validation functions
int nlink_validate_config(nlink_config_t* config) {
    return config ? 0 : -1;
}

void nlink_cleanup(void) {
    // Cleanup implementation
}
