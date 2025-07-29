/**
 * @file regex_matcher.h
 * @brief Regular expression matching for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides regular expression processing capabilities
 * for the NexusLink scripting system, supporting pattern matching.
 */

#ifndef NLINK_REGEX_MATCHER_H
#define NLINK_REGEX_MATCHER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Regular expression compilation options
 */
typedef struct nlink_regex_options {
    bool case_insensitive;   // Case insensitive matching
    bool multiline;          // Multiline mode (^ and $ match line boundaries)
    bool dot_all;            // Dot matches all characters including newlines
    bool extended;           // Extended regular expression syntax
} nlink_regex_options;

/**
 * Opaque type for compiled regular expressions
 */
typedef struct nlink_regex nlink_regex;

/**
 * Regular expression match result
 */
typedef struct nlink_regex_match {
    size_t start;            // Start position of match
    size_t end;              // End position of match
    size_t group_count;      // Number of capture groups
    size_t* group_starts;    // Array of group start positions
    size_t* group_ends;      // Array of group end positions
} nlink_regex_match;

/**
 * @brief Create default regular expression options
 * 
 * @return Default regex options
 */
nlink_regex_options nlink_regex_default_options(void);

/**
 * @brief Compile a regular expression pattern
 * 
 * @param pattern The regex pattern to compile
 * @param options Compilation options
 * @return Compiled regex or NULL on error
 */
nlink_regex* nlink_regex_compile(const char* pattern, nlink_regex_options options);

/**
 * @brief Free a compiled regular expression
 * 
 * @param regex The regex to free
 */
void nlink_regex_free(nlink_regex* regex);

/**
 * @brief Match a string against a regular expression
 * 
 * @param regex The compiled regex to match against
 * @param string The string to test
 * @param match Optional pointer to receive match details (can be NULL)
 * @return true if the string matches, false otherwise
 */
bool nlink_regex_match(nlink_regex* regex, const char* string, nlink_regex_match* match);

/**
 * @brief Free regex match resources
 * 
 * @param match The match to free
 */
void nlink_regex_match_free(nlink_regex_match* match);

/**
 * @brief Get a captured group from a match
 * 
 * @param string The original string that was matched
 * @param match The match result
 * @param group The group index (0 for the whole match)
 * @return Newly allocated string with the captured text or NULL
 */
char* nlink_regex_get_group(const char* string, nlink_regex_match* match, size_t group);

/**
 * @brief Escape special regex characters in a string
 * 
 * @param string The string to escape
 * @return Newly allocated string with escaped characters
 */
char* nlink_regex_escape(const char* string);

#endif /* NLINK_REGEX_MATCHER_H */
