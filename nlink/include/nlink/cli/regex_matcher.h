/**
 * @file regex_matcher.h
 * @brief Regular expression matching for CLI commands
 * 
 * Provides pattern matching capabilities for command routing.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_REGEX_MATCHER_H
 #define NLINK_REGEX_MATCHER_H
 
 #include <stdbool.h>
 #include <regex.h>
 #include <stddef.h>
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/command.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Regex matcher structure
  */
 typedef struct {
     regex_t compiled;     /**< Compiled regex pattern */
     bool is_compiled;     /**< Whether the pattern is compiled */
 } RegexMatcher;
 
 /**
  * @brief Pattern matching flags
  */
 typedef enum {
     NLINK_PATTERN_FLAG_NONE = 0,     /**< No flags */
     NLINK_PATTERN_FLAG_REGEX = 1,    /**< Use regex matching */
     NLINK_PATTERN_FLAG_CASE_INSENSITIVE = 2, /**< Case insensitive matching */
     NLINK_PATTERN_FLAG_GLOB = 4,     /**< Use glob pattern matching */
     NLINK_PATTERN_FLAG_EXTENDED = 8  /**< Extended regex syntax */
 } NlinkPatternFlags;
 
 /**
  * @brief Create a new regex matcher
  * 
  * @return RegexMatcher* New matcher or NULL on failure
  */
 RegexMatcher* regex_matcher_create(void);
 
 /**
  * @brief Destroy a regex matcher
  * 
  * @param matcher Matcher to destroy
  */
 void regex_matcher_destroy(RegexMatcher* matcher);
 
 /**
  * @brief Match a string against a pattern
  * 
  * @param matcher Matcher to use
  * @param pattern Pattern to match against
  * @param string String to match
  * @param flags Matching flags
  * @param matches Array to store matches (can be NULL)
  * @param max_matches Maximum number of matches to store
  * @return int Number of matches found or -1 on error
  */
 int regex_matcher_match(RegexMatcher* matcher, 
                         const char* pattern, 
                         const char* string,
                         NlinkPatternFlags flags,
                         regmatch_t* matches,
                         size_t max_matches);
 
 /**
  * @brief Find commands matching a pattern
  * 
  * @param matcher Matcher to use
  * @param pattern Pattern to match against
  * @param commands Array of commands to search
  * @param count Number of commands in the array
  * @param matches Array to store matching commands
  * @param max_matches Maximum number of matches to store
  * @return size_t Number of matches found
  */
 size_t regex_matcher_find_commands(RegexMatcher* matcher, 
                                   const char* pattern,
                                   NexusCommand** commands, 
                                   size_t count,
                                   NexusCommand** matches, 
                                   size_t max_matches);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_REGEX_MATCHER_H */