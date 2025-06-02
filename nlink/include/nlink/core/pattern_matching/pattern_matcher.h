/**
 * @file pattern_matcher.h
 * @brief Pattern matching system for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides wildcard pattern matching functionality for
 * the NexusLink scripting system, enabling declarative path resolution.
 */

#ifndef NLINK_PATTERN_MATCHER_H
#define NLINK_PATTERN_MATCHER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Pattern matching options
 */
typedef struct nlink_pattern_options {
    bool case_sensitive;     // Whether matching should be case sensitive
    bool dot_special;        // Whether dot files are handled specially
    bool extended_glob;      // Whether to use extended glob syntax
    bool follow_symlinks;    // Whether to follow symbolic links
} nlink_pattern_options;

/**
 * Pattern matching result with matched files
 */
typedef struct nlink_pattern_result {
    char** matches;          // Array of matched paths
    size_t count;            // Number of matches
    size_t capacity;         // Capacity of matches array
} nlink_pattern_result;

/**
 * @brief Create default pattern matching options
 * 
 * @return Default pattern options
 */
nlink_pattern_options nlink_pattern_default_options(void);

/**
 * @brief Match files using a glob pattern
 * 
 * @param pattern The glob pattern to match against
 * @param base_dir The base directory to start matching from
 * @param options Pattern matching options
 * @return Pattern matching result with matched files
 */
nlink_pattern_result* nlink_match_pattern(const char* pattern, 
                                         const char* base_dir,
                                         nlink_pattern_options options);

/**
 * @brief Free pattern matching result
 * 
 * @param result The result to free
 */
void nlink_pattern_result_free(nlink_pattern_result* result);

/**
 * @brief Test if a string matches a glob pattern
 * 
 * @param pattern The glob pattern to match
 * @param string The string to test
 * @param options Pattern matching options
 * @return true if the string matches the pattern, false otherwise
 */
bool nlink_pattern_match_string(const char* pattern, 
                               const char* string,
                               nlink_pattern_options options);

/**
 * @brief Convert a glob pattern to a regular expression
 * 
 * @param pattern The glob pattern to convert
 * @param options Pattern matching options
 * @return Newly allocated string containing the regex pattern
 */
char* nlink_pattern_to_regex(const char* pattern, 
                            nlink_pattern_options options);

#endif /* NLINK_PATTERN_MATCHER_H */
