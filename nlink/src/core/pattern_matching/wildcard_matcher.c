/**
 * @file wildcard_matcher.c
 * @brief Implementation of wildcard pattern matching for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pattern_matching/wildcard_matcher.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

nlink_wildcard_options nlink_wildcard_default_options(void) {
    nlink_wildcard_options options = {
        .case_sensitive = true,
        .path_mode = true
    };
    
    return options;
}

static bool is_path_separator(char c) {
    return c == '/' || c == '\\';
}

static bool match_glob(const char* pattern, 
                      const char* string, 
                      nlink_wildcard_options options) {
    // This is a placeholder implementation
    // A complete implementation would implement proper glob matching
    
    // For demonstration, just check if pattern appears in string
    return strstr(string, pattern) != NULL;
}

static bool match_extended_glob(const char* pattern, 
                               const char* string, 
                               nlink_wildcard_options options) {
    // This is a placeholder implementation
    // A complete implementation would implement extended glob matching
    
    // For demonstration, just check if pattern appears in string
    return strstr(string, pattern) != NULL;
}

static bool match_regex(const char* pattern, 
                       const char* string, 
                       nlink_wildcard_options options) {
    // This is a placeholder implementation
    // A complete implementation would use the regex matcher
    
    // For demonstration, just check if pattern appears in string
    return strstr(string, pattern) != NULL;
}

bool nlink_wildcard_match(const char* pattern, 
                         const char* string,
                         nlink_wildcard_type type,
                         nlink_wildcard_options options) {
    if (pattern == NULL || string == NULL) {
        return false;
    }
    
    // Choose matching function based on type
    switch (type) {
        case NLINK_WILDCARD_GLOB:
            return match_glob(pattern, string, options);
            
        case NLINK_WILDCARD_EXTENDED:
            return match_extended_glob(pattern, string, options);
            
        case NLINK_WILDCARD_REGEX:
            return match_regex(pattern, string, options);
            
        default:
            return false;
    }
}

bool nlink_wildcard_match_path(const char* pattern, 
                              const char* path,
                              nlink_wildcard_type type,
                              nlink_wildcard_options options) {
    // Ensure path mode is set
    options.path_mode = true;
    
    // Use the general matcher
    return nlink_wildcard_match(pattern, path, type, options);
}

bool nlink_wildcard_needs_recursion(const char* pattern, 
                                   nlink_wildcard_type type) {
    if (pattern == NULL) {
        return false;
    }
    
    // Check for patterns that require recursion
    switch (type) {
        case NLINK_WILDCARD_GLOB:
            // In glob patterns, ** doesn't exist, so recursion is
            // only needed if there are multiple directories
            return strchr(pattern, '/') != NULL || strchr(pattern, '\\') != NULL;
            
        case NLINK_WILDCARD_EXTENDED:
            // In extended patterns, ** means recurse
            return strstr(pattern, "**") != NULL;
            
        case NLINK_WILDCARD_REGEX:
            // In regex, complex patterns may imply recursion
            // This is a simplification
            return strchr(pattern, '/') != NULL || strchr(pattern, '\\') != NULL;
            
        default:
            return false;
    }
}

char* nlink_wildcard_get_base_dir(const char* pattern) {
    if (pattern == NULL) {
        return NULL;
    }
    
    // Find the last path separator before any wildcard
    const char* wildcard = strpbrk(pattern, "*?[{");
    if (wildcard == NULL) {
        // No wildcards, so the base dir is the pattern up to the last separator
        const char* last_sep = strrchr(pattern, '/');
        if (last_sep == NULL) {
            last_sep = strrchr(pattern, '\\');
        }
        
        if (last_sep == NULL) {
            // No separator, so base dir is current directory
            return strdup(".");
        } else {
            // Base dir is pattern up to the separator
            size_t len = last_sep - pattern;
            char* result = malloc(len + 1);
            if (result == NULL) {
                return NULL;
            }
            
            strncpy(result, pattern, len);
            result[len] = '\0';
            return result;
        }
    } else {
        // Find the last separator before the first wildcard
        const char* last_sep = NULL;
        for (const char* p = pattern; p < wildcard; p++) {
            if (is_path_separator(*p)) {
                last_sep = p;
            }
        }
        
        if (last_sep == NULL) {
            // No separator before wildcard, so base dir is current directory
            return strdup(".");
        } else {
            // Base dir is pattern up to the separator
            size_t len = last_sep - pattern;
            char* result = malloc(len + 1);
            if (result == NULL) {
                return NULL;
            }
            
            strncpy(result, pattern, len);
            result[len] = '\0';
            return result;
        }
    }
}

char* nlink_wildcard_canonicalize(const char* pattern, 
                                 nlink_wildcard_type type) {
    if (pattern == NULL) {
        return NULL;
    }
    
    // For this simplified implementation, just return a copy
    return strdup(pattern);
}
