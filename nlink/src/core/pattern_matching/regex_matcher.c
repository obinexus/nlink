/**
 * @file regex_matcher.c
 * @brief Implementation of regular expression matching for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pattern_matching/regex_matcher.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

struct nlink_regex {
    char* pattern;           // Original pattern
    nlink_regex_options options;  // Compilation options
    void* compiled;          // Compiled regex (implementation-specific)
};

nlink_regex_options nlink_regex_default_options(void) {
    nlink_regex_options options = {
        .case_insensitive = false,
        .multiline = false,
        .dot_all = false,
        .extended = true
    };
    
    return options;
}

nlink_regex* nlink_regex_compile(const char* pattern, nlink_regex_options options) {
    if (pattern == NULL) {
        return NULL;
    }
    
    // Allocate regex structure
    nlink_regex* regex = malloc(sizeof(nlink_regex));
    if (regex == NULL) {
        return NULL;
    }
    
    // Copy pattern
    regex->pattern = strdup(pattern);
    if (regex->pattern == NULL) {
        free(regex);
        return NULL;
    }
    
    // Store options
    regex->options = options;
    
    // Compile pattern (placeholder implementation)
    regex->compiled = NULL;  // Would be a real compilation result
    
    return regex;
}

void nlink_regex_free(nlink_regex* regex) {
    if (regex == NULL) {
        return;
    }
    
    free(regex->pattern);
    // Free compiled pattern (implementation-specific)
    free(regex);
}

bool nlink_regex_match(nlink_regex* regex, const char* string, nlink_regex_match* match) {
    if (regex == NULL || string == NULL) {
        return false;
    }
    
    // This is a placeholder implementation
    // A complete implementation would use a real regex engine
    
    // For demonstration, just check if the pattern appears in the string
    bool found = strstr(string, regex->pattern) != NULL;
    
    // If caller wants match details and we found a match
    if (match != NULL && found) {
        const char* start = strstr(string, regex->pattern);
        match->start = start - string;
        match->end = match->start + strlen(regex->pattern);
        match->group_count = 0;
        match->group_starts = NULL;
        match->group_ends = NULL;
    }
    
    return found;
}

void nlink_regex_match_free(nlink_regex_match* match) {
    if (match == NULL) {
        return;
    }
    
    free(match->group_starts);
    free(match->group_ends);
    // Don't free match itself - caller owns it
}

char* nlink_regex_get_group(const char* string, nlink_regex_match* match, size_t group) {
    if (string == NULL || match == NULL) {
        return NULL;
    }
    
    size_t start, end;
    
    if (group == 0) {
        // Group 0 is the entire match
        start = match->start;
        end = match->end;
    } else if (group <= match->group_count) {
        // Get specified capture group
        start = match->group_starts[group - 1];
        end = match->group_ends[group - 1];
    } else {
        // Invalid group
        return NULL;
    }
    
    // Extract substring
    size_t length = end - start;
    char* result = malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }
    
    strncpy(result, string + start, length);
    result[length] = '\0';
    
    return result;
}

char* nlink_regex_escape(const char* string) {
    if (string == NULL) {
        return NULL;
    }
    
    // Count characters that need escaping
    size_t escape_count = 0;
    for (const char* p = string; *p != '\0'; p++) {
        if (strchr(".*+?()[]{}|^$\\", *p) != NULL) {
            escape_count++;
        }
    }
    
    // Allocate result with space for escapes
    size_t length = strlen(string);
    char* result = malloc(length + escape_count + 1);
    if (result == NULL) {
        return NULL;
    }
    
    // Copy with escaping
    char* out = result;
    for (const char* p = string; *p != '\0'; p++) {
        if (strchr(".*+?()[]{}|^$\\", *p) != NULL) {
            *out++ = '\\';
        }
        *out++ = *p;
    }
    *out = '\0';
    
    return result;
}
