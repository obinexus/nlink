/**
 * @file matcher.h
 * @brief Pattern matching functionality for NexusLink
 * 
 * Provides efficient pattern matching capabilities including regular expressions and
 * glob-style wildcards for flexible string matching, with support for
 * parameter extraction through capturing groups.
 * 
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_PATTERN_MATCHER_H
#define NLINK_PATTERN_MATCHER_H

#include <stdbool.h>
#include <stddef.h>
#include <regex.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Match information structure
 * 
 * Contains information about a successful match, including
 * captured substrings from regex groups.
 */
typedef struct NlinkMatchInfo NlinkMatchInfo;

/**
 * @brief Pattern matching flags
 */
typedef enum {
    NLINK_PATTERN_FLAG_NONE              = 0x00,  /**< No special handling */
    NLINK_PATTERN_FLAG_CASE_INSENSITIVE  = 0x01,  /**< Case insensitive matching */
    NLINK_PATTERN_FLAG_GLOB              = 0x02,  /**< Force glob-style pattern matching */
    NLINK_PATTERN_FLAG_REGEX             = 0x04,  /**< Force regex pattern matching */
    NLINK_PATTERN_FLAG_EXTENDED          = 0x08,  /**< Use extended regex syntax */
    NLINK_PATTERN_FLAG_LITERAL           = 0x10   /**< Force literal string matching */
} NlinkPatternFlags;


 
 /**
  * @brief Pattern matcher implementation
  */
 typedef struct NlinkPatternMatcher NlinkPatternMatcher;
 struct NlinkPatternMatcher {
    char* pattern;              // Original pattern string
    NlinkPatternFlags flags;    // Behavior flags
    
    // Pattern type
    bool is_glob;               // Whether this is a glob pattern
    bool is_regex;              // Whether this is a regex pattern
    bool is_literal;            // Whether this is a literal string match
    
    // Internal compiled representation
    union {
        regex_t regex;          // Compiled regex pattern
        void* reserved;         // Reserved for future pattern types
    } compiled;
    
    bool is_compiled;           // Whether the pattern has been compiled
};

 /**
  * @brief Create a new pattern matcher
  * 
  * @param pattern Pattern string
  * @param flags Behavior flags
  * @return NlinkPatternMatcher* New matcher instance or NULL on failure
  */
 NlinkPatternMatcher* nlink_pattern_create(const char* pattern, NlinkPatternFlags flags);
 
 /**
  * @brief Match a string against the pattern
  * 
  * @param matcher Pattern matcher
  * @param string String to match
  * @return bool True if matched, false otherwise
  */
 bool nlink_pattern_match(NlinkPatternMatcher* matcher, const char* string);
 
 /**
  * @brief Match a string against the pattern, extracting parameters
  * 
  * For regex patterns, this extracts capturing groups.
  * For glob patterns, wildcard matches can be extracted.
  * For literal patterns, no parameters are extracted.
  * 
  * @param matcher Pattern matcher
  * @param string String to match
  * @param info Pointer to store match information
  * @return bool True if matched, false otherwise
  */
 bool nlink_pattern_match_with_params(NlinkPatternMatcher* matcher, 
                                      const char* string, 
                                      NlinkMatchInfo** info);
 
 /**
  * @brief Create a new match information structure
  * 
  * @return NlinkMatchInfo* New match info or NULL on failure
  */
 NlinkMatchInfo* nlink_match_info_create(void);
 
 /**
  * @brief Get the number of capturing groups in the match
  * 
  * @param info Match information
  * @return size_t Number of groups (0 if no match or no groups)
  */
 size_t nlink_match_info_get_group_count(const NlinkMatchInfo* info);
 
 /**
  * @brief Get a captured group string
  * 
  * @param info Match information
  * @param group_index Group index (0 for full match, 1+ for capturing groups)
  * @return const char* Captured string or NULL if not found
  */
 const char* nlink_match_info_get_group(const NlinkMatchInfo* info, size_t group_index);
 
 /**
  * @brief Get the start position of a captured group
  * 
  * @param info Match information
  * @param group_index Group index (0 for full match, 1+ for capturing groups)
  * @return size_t Start position or (size_t)-1 if not found
  */
 size_t nlink_match_info_get_group_start(const NlinkMatchInfo* info, size_t group_index);
 
 /**
  * @brief Get the end position of a captured group
  * 
  * @param info Match information
  * @param group_index Group index (0 for full match, 1+ for capturing groups)
  * @return size_t End position or (size_t)-1 if not found
  */
 size_t nlink_match_info_get_group_end(const NlinkMatchInfo* info, size_t group_index);
 
 /**
  * @brief Replace pattern matches in a string
  * 
  * @param matcher Pattern matcher
  * @param string String to modify
  * @param replacement Replacement string (can include $1, $2, etc. for captured groups)
  * @param result Buffer to store the result
  * @param result_size Size of the result buffer
  * @return size_t Number of replacements made, or 0 on error
  */
 size_t nlink_pattern_replace(NlinkPatternMatcher* matcher,
                              const char* string,
                              const char* replacement,
                              char* result,
                              size_t result_size);
 
 /**
  * @brief Free match information resources
  * 
  * @param info Match information to free
  */
 void nlink_match_info_destroy(NlinkMatchInfo* info);
 
 /**
  * @brief Get the pattern string
  * 
  * @param matcher Pattern matcher
  * @return const char* Pattern string or NULL on error
  */
 const char* nlink_pattern_get_pattern(const NlinkPatternMatcher* matcher);
 
 /**
  * @brief Get the pattern flags
  * 
  * @param matcher Pattern matcher
  * @return NlinkPatternFlags Pattern flags
  */
 NlinkPatternFlags* nlink_pattern_get_flags(const NlinkPatternMatcher* matcher);
 
 /**
  * @brief Check if the pattern is a regex pattern
  * 
  * @param matcher Pattern matcher
  * @return bool True if regex pattern, false otherwise
  */
 bool nlink_pattern_is_regex(const NlinkPatternMatcher* matcher);
 
 /**
  * @brief Check if the pattern is a glob pattern
  * 
  * @param matcher Pattern matcher
  * @return bool True if glob pattern, false otherwise
  */
 bool nlink_pattern_is_glob(const NlinkPatternMatcher* matcher);
 
 /**
  * @brief Get the number of capturing groups in the pattern
  * 
  * @param matcher Pattern matcher
  * @return size_t Number of groups (0 for non-regex patterns)
  */
 size_t nlink_pattern_get_group_count(const NlinkPatternMatcher* matcher);
/**
 * @brief Free pattern matcher resources
 * 
 * @param matcher Pattern matcher to free
 */
void nlink_pattern_destroy(NlinkPatternMatcher* matcher);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_PATTERN_MATCHER_H */