/**
 * @file pattern_matcher.c
 * @brief Implementation of pattern matching for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pattern_matching/pattern_matcher.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#define INITIAL_MATCHES_CAPACITY 16

nlink_pattern_options nlink_pattern_default_options(void) {
    nlink_pattern_options options = {
        .case_sensitive = true,
        .dot_special = true,
        .extended_glob = true,
        .follow_symlinks = false
    };
    
    return options;
}

static bool is_path_separator(char c) {
    return c == '/' || c == '\\';
}

static char* join_path(const char* dir, const char* file) {
    size_t dir_len = strlen(dir);
    size_t file_len = strlen(file);
    size_t need_separator = (dir_len > 0 && !is_path_separator(dir[dir_len - 1])) ? 1 : 0;
    
    char* result = malloc(dir_len + need_separator + file_len + 1);
    if (result == NULL) {
        return NULL;
    }
    
    strcpy(result, dir);
    if (need_separator) {
        result[dir_len] = '/';
    }
    strcpy(result + dir_len + need_separator, file);
    
    return result;
}

static bool add_match(nlink_pattern_result* result, const char* path) {
    if (result->count >= result->capacity) {
        size_t new_capacity = result->capacity * 2;
        char** new_matches = realloc(result->matches, new_capacity * sizeof(char*));
        if (new_matches == NULL) {
            return false;
        }
        
        result->matches = new_matches;
        result->capacity = new_capacity;
    }
    
    result->matches[result->count] = strdup(path);
    if (result->matches[result->count] == NULL) {
        return false;
    }
    
    result->count++;
    return true;
}

static void process_directory(nlink_pattern_result* result,
                             const char* base_dir,
                             const char* pattern,
                             const char* current_path,
                             nlink_pattern_options options) {
    // This is a placeholder implementation
    // A complete implementation would:
    // 1. Open the directory
    // 2. Iterate through each entry
    // 3. Apply pattern matching to each entry
    // 4. Recursively process subdirectories as needed
    
    // For demonstration, we'll just add the pattern as if it matched
    char* full_path = join_path(base_dir, pattern);
    if (full_path != NULL) {
        add_match(result, full_path);
        free(full_path);
    }
}

nlink_pattern_result* nlink_match_pattern(const char* pattern, 
                                         const char* base_dir,
                                         nlink_pattern_options options) {
    if (pattern == NULL) {
        return NULL;
    }
    
    // Create result structure
    nlink_pattern_result* result = malloc(sizeof(nlink_pattern_result));
    if (result == NULL) {
        return NULL;
    }
    
    result->matches = malloc(INITIAL_MATCHES_CAPACITY * sizeof(char*));
    if (result->matches == NULL) {
        free(result);
        return NULL;
    }
    
    result->count = 0;
    result->capacity = INITIAL_MATCHES_CAPACITY;
    
    // Process pattern (simplified implementation)
    const char* base = base_dir != NULL ? base_dir : ".";
    process_directory(result, base, pattern, "", options);
    
    return result;
}

void nlink_pattern_result_free(nlink_pattern_result* result) {
    if (result == NULL) {
        return;
    }
    
    for (size_t i = 0; i < result->count; i++) {
        free(result->matches[i]);
    }
    
    free(result->matches);
    free(result);
}

bool nlink_pattern_match_string(const char* pattern, 
                               const char* string,
                               nlink_pattern_options options) {
    // This is a placeholder implementation
    // A complete implementation would implement glob pattern matching
    
    // For demonstration, just check if pattern appears in string
    return strstr(string, pattern) != NULL;
}

char* nlink_pattern_to_regex(const char* pattern, 
                            nlink_pattern_options options) {
    // This is a placeholder implementation
    // A complete implementation would convert glob patterns to regex
    
    // For demonstration, just return a copy of the pattern
    return strdup(pattern);
}
