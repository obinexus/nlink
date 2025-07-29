/**
 * @file config_pipeline.c
 * @brief Implementation of configuration pipeline
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/config/config_pipeline.h"
#include "nlink/core/config/config_validator.h"
#include "nlink/core/pattern_matching/wildcard_matcher.h"
#include "nlink/core/common/nexus_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETURN_IF_ERROR(res) if ((res).status != NEXUS_STATUS_SUCCESS) return (res)

// Internal structure to track pattern resolution state
typedef struct {
    char** included_files;
    size_t included_count;
    char** excluded_patterns;
    size_t excluded_count;
} PatternResolutionState;

NexusResult config_tokenize_stage(NexusContext* ctx, NexusBuffer* buffer) {
    if (!buffer || !buffer->data) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Invalid buffer for tokenization",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Simplified implementation - in a real system, this would parse the 
    // configuration text and build a token tree
    const char* config_text = (const char*)buffer->data;
    
    // Create a root section token
    nlink_token_config_section* root = (nlink_token_config_section*)
        nlink_token_create(NLINK_TOKEN_CONFIG_SECTION, "root", 1, 1);
    
    if (!root) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_OUT_OF_MEMORY,
            "Failed to create root configuration section",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    root->section_name = strdup("root");
    root->properties = NULL;
    root->property_count = 0;
    
    // In a real implementation, we would parse the config text here
    // and populate the root section with properties and subsections
    
    // Update the buffer to point to the tokenized data
    buffer->data = root;
    buffer->size = sizeof(nlink_token_config_section);
    
    return nexus_success(root, NULL);
}

NexusResult config_validate_stage(NexusContext* ctx, NexusBuffer* buffer) {
    if (!buffer || !buffer->data) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Invalid buffer for validation",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Extract the root section
    nlink_token_config_section* root = (nlink_token_config_section*)buffer->data;
    
    // Validate the configuration tree
    return nlink_validate_config_tree(root, ctx);
}

NexusResult config_resolve_patterns_stage(NexusContext* ctx, NexusBuffer* buffer) {
    if (!buffer || !buffer->data) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Invalid buffer for pattern resolution",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Extract the root section
    nlink_token_config_section* root = (nlink_token_config_section*)buffer->data;
    
    // Collect pattern tokens
    // In a real implementation, we would traverse the tree and find all pattern tokens
    
    // Initialize resolution state
    PatternResolutionState state = {0};
    
    // Process include patterns
    for (size_t i = 0; i < 0; i++) {  // Placeholder loop
        // In a real implementation, we would process each pattern token
        const char* pattern = "*.c";  // Example pattern
        
        // Find all matching files
        char** matches = NULL;
        size_t match_count = 0;
        
        // Example - in reality, this would use a filesystem API
        if (match_with_wildcards(pattern, "test.c")) {
            match_count++;
            matches = realloc(matches, match_count * sizeof(char*));
            matches[match_count - 1] = strdup("test.c");
        }
        
        // Add to included files
        for (size_t j = 0; j < match_count; j++) {
            // Check exclusions
            bool excluded = false;
            for (size_t k = 0; k < state.excluded_count; k++) {
                if (match_with_wildcards(state.excluded_patterns[k], matches[j])) {
                    excluded = true;
                    break;
                }
            }
            
            // If not excluded, add to included files
            if (!excluded) {
                state.included_count++;
                state.included_files = realloc(state.included_files, 
                                             state.included_count * sizeof(char*));
                state.included_files[state.included_count - 1] = matches[j];
            } else {
                free(matches[j]);
            }
        }
        
        free(matches);
    }
    
    // The buffer is unchanged, but the resolution state would be used downstream
    return nexus_success(root, NULL);
}

PipelineStage create_default_config_pipeline(void) {
    // Create individual stages
    PipelineStage tokenize = config_tokenize_stage;
    PipelineStage validate = config_validate_stage;
    PipelineStage resolve = config_resolve_patterns_stage;
    
    // Compose them together
    PipelineStage validate_and_resolve = compose(validate, resolve);
    PipelineStage complete_pipeline = compose(tokenize, validate_and_resolve);
    
    return complete_pipeline;
}

NexusResult process_config_file(
    const char* filename,
    NexusContext* ctx,
    nlink_token_config_section** out_root
) {
    if (!filename || !out_root) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Invalid arguments for processing configuration file",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Read the file
    FILE* file = fopen(filename, "r");
    if (!file) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_FILE_NOT_FOUND,
            "Failed to open configuration file",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file content
    char* content = malloc(size + 1);
    if (!content) {
        fclose(file);
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_OUT_OF_MEMORY,
            "Failed to allocate memory for configuration file content",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    size_t read = fread(content, 1, size, file);
    fclose(file);
    
    content[read] = '\0';
    
    // Create a buffer
    NexusBuffer buffer;
    buffer.data = content;
    buffer.size = read;
    
    // Create a pipeline
    PipelineStage pipeline = create_default_config_pipeline();
    
    // Process the buffer
    NexusResult result = pipeline(ctx, &buffer);
    free(content);  // We don't need the content anymore
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Extract the processed configuration
    *out_root = (nlink_token_config_section*)buffer.data;
    
    return nexus_success(*out_root, NULL);
}