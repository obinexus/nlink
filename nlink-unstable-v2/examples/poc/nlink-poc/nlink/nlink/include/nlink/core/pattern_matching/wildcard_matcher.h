/**
 * @file wildcard_matcher.h
 * @brief Wildcard pattern matching for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides specialized wildcard pattern matching
 * for the NexusLink scripting system, optimized for file paths.
 */

#ifndef NLINK_WILDCARD_MATCHER_H
#define NLINK_WILDCARD_MATCHER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Wildcard pattern type
 */
typedef enum nlink_wildcard_type {
    NLINK_WILDCARD_GLOB,         // Standard glob patterns (*, ?, [abc])
    NLINK_WILDCARD_EXTENDED,     // Extended glob patterns (**, ?(pat), *(pat), etc.)
    NLINK_WILDCARD_REGEX         // Regular expression patterns
} nlink_wildcard_type;

/**
 * Wildcard pattern options
 */
typedef struct nlink_wildcard_options {
    bool case_sensitive;     // Whether matching should be case sensitive
    bool path_mode;          // Whether to handle as file paths (/ is special)
} nlink_wildcard_options;

/**
 * @brief Create default wildcard options
 * 
 * @return Default wildcard options
 */
nlink_wildcard_options nlink_wildcard_default_options(void);

/**
 * @brief Match a string against a wildcard pattern
 * 
 * @param pattern The wildcard pattern to match
 * @param string The string to test
 * @param type The type of wildcard pattern
 * @param options Matching options
 * @return true if the string matches the pattern, false otherwise
 */
bool nlink_wildcard_match(const char* pattern, 
                         const char* string,
                         nlink_wildcard_type type,
                         nlink_wildcard_options options);

/**
 * @brief Match a file path against a wildcard pattern
 * 
 * This is optimized for file path matching with proper path separator handling.
 * 
 * @param pattern The wildcard pattern to match
 * @param path The file path to test
 * @param type The type of wildcard pattern
 * @param options Matching options
 * @return true if the path matches the pattern, false otherwise
 */
bool nlink_wildcard_match_path(const char* pattern, 
                              const char* path,
                              nlink_wildcard_type type,
                              nlink_wildcard_options options);

/**
 * @brief Test if a wildcard pattern needs directory recursion
 * 
 * @param pattern The pattern to test
 * @param type The type of wildcard pattern
 * @return true if the pattern requires directory recursion, false otherwise
 */
bool nlink_wildcard_needs_recursion(const char* pattern, 
                                   nlink_wildcard_type type);

/**
 * @brief Extract the base directory from a wildcard pattern
 * 
 * @param pattern The pattern to analyze
 * @return Newly allocated string with the base directory, or NULL
 */
char* nlink_wildcard_get_base_dir(const char* pattern);

/**
 * @brief Convert a wildcard pattern to its canonical form
 * 
 * @param pattern The pattern to canonicalize
 * @param type The type of wildcard pattern
 * @return Newly allocated string with the canonical pattern
 */
char* nlink_wildcard_canonicalize(const char* pattern, 
                                 nlink_wildcard_type type);

#endif /* NLINK_WILDCARD_MATCHER_H */
