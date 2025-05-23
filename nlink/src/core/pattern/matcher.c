/**
 * @file matcher.c
 * @brief Implementation of pattern matching functionality for NexusLink
 * 
 * Provides efficient pattern matching capabilities including
 * full regular expressions and glob-style wildcards for command matching.
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pattern/matcher.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <regex.h>
 
 /**
  * @brief Helper function to determine if a pattern is a glob pattern
  * 
  * @param pattern Pattern string
  * @return true if glob pattern, false otherwise
  */
 static bool is_glob_pattern(const char* pattern) {
     if (!pattern) return false;
     
     return (strchr(pattern, '*') != NULL || 
             strchr(pattern, '?') != NULL ||
             strchr(pattern, '[') != NULL);
 }
 
 /**
  * @brief Helper function to determine if a pattern is a regex pattern
  * This looks for regex-specific syntax that wouldn't be in glob patterns
  * 
  * @param pattern Pattern string
  * @return true if regex pattern, false otherwise
  */
static bool is_regex_pattern(const char* pattern) {
    if (!pattern) return false;
    
    // Look for regex-specific characters
    return (strchr(pattern, '^') != NULL ||   // Start anchor
            strchr(pattern, '$') != NULL ||   // End anchor
            strchr(pattern, '(') != NULL ||   // Grouping
            strchr(pattern, '|') != NULL ||   // Alternation
            strchr(pattern, '+') != NULL ||   // One or more
            strchr(pattern, '.') != NULL);    // Any character
}
 
 /**
  * @brief Helper function for simple glob matching
  * 
  * Matches patterns with:
  * - * (any number of characters)
  * - ? (exactly one character)
  * - [abc] (character class)
  * 
  * @param pattern Glob pattern
  * @param string String to match
  * @param case_insensitive Whether to ignore case
  * @return true if matched, false otherwise
  */
 static bool glob_match(const char* pattern, const char* string, bool case_insensitive) {
     if (!pattern || !string) return false;
     
     // Empty pattern matches only empty string
     if (*pattern == '\0') return *string == '\0';
     
     // Handle wildcard *
     if (*pattern == '*') {
         // Skip consecutive *
         while (*(pattern+1) == '*') pattern++;
         
         // * at end of pattern matches everything
         if (*(pattern+1) == '\0') return true;
         
         // Try match with * consuming zero or more characters
         while (*string) {
             if (glob_match(pattern+1, string, case_insensitive) ||
                 glob_match(pattern, string+1, case_insensitive))
                 return true;
             string++;
         }
         
         // Try match with * consuming zero characters
         return glob_match(pattern+1, string, case_insensitive);
     }
     
     // Handle ? (match exactly one char)
     if (*pattern == '?' && *string != '\0') {
         return glob_match(pattern+1, string+1, case_insensitive);
     }
     
     // Handle character class [abc]
     if (*pattern == '[') {
         const char* end_bracket = strchr(pattern, ']');
         
         // Malformed pattern, no closing bracket
         if (!end_bracket) return false;
         
         bool match = false;
         const char* char_class = pattern + 1;
         char current = *string;
         
         // Handle negated class [^abc]
         bool negate = *char_class == '^';
         if (negate) char_class++;
         
         // Check if current character is in the class
         while (char_class < end_bracket) {
             char class_char = *char_class;
             
             // Handle range a-z
             if (char_class + 2 < end_bracket && *(char_class + 1) == '-') {
                 char start = class_char;
                 char end = *(char_class + 2);
                 
                 if (case_insensitive) {
                     current = tolower(current);
                     start = tolower(start);
                     end = tolower(end);
                 }
                 
                 if (current >= start && current <= end) {
                     match = true;
                     break;
                 }
                 
                 char_class += 3;
             } else {
                 // Simple character match
                 if (case_insensitive) {
                     if (tolower(current) == tolower(class_char)) {
                         match = true;
                         break;
                     }
                 } else {
                     if (current == class_char) {
                         match = true;
                         break;
                     }
                 }
                 
                 char_class++;
             }
         }
         
         // Apply negation
         if (negate) match = !match;
         
         // If no match in character class, return false
         if (!match || *string == '\0') return false;
         
         // Continue matching after the character class
         return glob_match(end_bracket + 1, string + 1, case_insensitive);
     }
     
     // Regular character match
     if (*string != '\0') {
         bool chars_match;
         
         if (case_insensitive) {
             chars_match = (tolower(*pattern) == tolower(*string));
         } else {
             chars_match = (*pattern == *string);
         }
         
         if (chars_match) {
             return glob_match(pattern+1, string+1, case_insensitive);
         }
     }
     
     return false;
 }
 
 /**
  * @brief Compile the pattern to its internal representation
  * 
  * @param matcher The pattern matcher to compile
  * @return true if compilation successful, false otherwise
  */
 static bool compile_pattern(NlinkPatternMatcher* matcher) {
     if (!matcher || !matcher->pattern || matcher->is_compiled) {
         return false;
     }
     
     // Don't need to compile literal patterns
     if (matcher->is_literal) {
         matcher->is_compiled = true;
         return true;
     }
     
     // Compile regex pattern
     if (matcher->is_regex) {
         int cflags = REG_EXTENDED;  // Use extended regex syntax
         
         if (matcher->flags & NLINK_PATTERN_FLAG_CASE_INSENSITIVE) {
             cflags |= REG_ICASE;  // Case insensitive matching
         }
         
         int result = regcomp(&matcher->compiled.regex, matcher->pattern, cflags);
         if (result != 0) {
             return false;
         }
         
         matcher->is_compiled = true;
         return true;
     }
     
     // Glob patterns don't need compilation (handled by glob_match)
     if (matcher->is_glob) {
         matcher->is_compiled = true;
         return true;
     }
     
     return false;
 }
 
 /**
  * @brief Create a new pattern matcher
  * 
  * @param pattern Pattern string
  * @param flags Behavior flags
  * @return NlinkPatternMatcher* New matcher instance or NULL on failure
  */
 NlinkPatternMatcher* nlink_pattern_create(const char* pattern, NlinkPatternFlags flags) {
     if (!pattern) {
         return NULL;
     }
     
     NlinkPatternMatcher* matcher = (NlinkPatternMatcher*)malloc(sizeof(NlinkPatternMatcher));
     if (!matcher) {
         return NULL;
     }
     
     // Initialize the matcher
     memset(matcher, 0, sizeof(NlinkPatternMatcher));
     matcher->pattern = strdup(pattern);
     if (!matcher->pattern) {
         free(matcher);
         return NULL;
     }
     
     matcher->flags = flags;
     
     // Determine pattern type
     if (flags & NLINK_PATTERN_FLAG_REGEX || is_regex_pattern(pattern)) {
         matcher->is_regex = true;
     } else if (flags & NLINK_PATTERN_FLAG_GLOB || is_glob_pattern(pattern)) {
         matcher->is_glob = true;
     } else {
         matcher->is_literal = true;
     }
     
     // Force regex mode if extended flag is set
     if (flags & NLINK_PATTERN_FLAG_EXTENDED) {
         matcher->is_regex = true;
         matcher->is_glob = false;
         matcher->is_literal = false;
     }
     
     // Compile the pattern if needed
     if (!compile_pattern(matcher)) {
         nlink_pattern_destroy(matcher);
         return NULL;
     }
     
     return matcher;
 }
 
 /**
  * @brief Match a string against the pattern
  * 
  * @param matcher Pattern matcher
  * @param string String to match
  * @return bool True if matched, false otherwise
  */
 bool nlink_pattern_match(NlinkPatternMatcher* matcher, const char* string) {
     if (!matcher || !matcher->pattern || !string) {
         return false;
     }
     
     // Ensure pattern is compiled
     if (!matcher->is_compiled && !compile_pattern(matcher)) {
         return false;
     }
     
     // Handle different pattern types
     if (matcher->is_regex) {
         // Execute the regex
         int result = regexec(&matcher->compiled.regex, string, 0, NULL, 0);
         return (result == 0);
     } else if (matcher->is_glob) {
         // Use glob matching
         return glob_match(
             matcher->pattern, 
             string, 
             (matcher->flags & NLINK_PATTERN_FLAG_CASE_INSENSITIVE) != 0
         );
     } else {
         // Simple literal string comparison
         if (matcher->flags & NLINK_PATTERN_FLAG_CASE_INSENSITIVE) {
             return strcasecmp(matcher->pattern, string) == 0;
         } else {
             return strcmp(matcher->pattern, string) == 0;
         }
     }
 }
 
 /**
  * @brief Get the pattern string
  * 
  * @param matcher Pattern matcher
  * @return const char* Pattern string or NULL on error
  */
 const char* nlink_pattern_get_pattern(const NlinkPatternMatcher* matcher) {
     if (!matcher) {
         return NULL;
     }
     
     return matcher->pattern;
 }
 
 /**
  * @brief Get the pattern flags
  * 
  * @param matcher Pattern matcher
  * @return NlinkPatternFlags Pattern flags
  */
 NlinkPatternFlags nlink_pattern_get_flags(const NlinkPatternMatcher* matcher) {
     if (!matcher) {
         return NLINK_PATTERN_FLAG_NONE;
     }
     
     return matcher->flags;
 }
 
 /**
  * @brief Check if the pattern is a regex pattern
  * 
  * @param matcher Pattern matcher
  * @return bool True if regex pattern, false otherwise
  */
 bool nlink_pattern_is_regex(const NlinkPatternMatcher* matcher) {
     if (!matcher) {
         return false;
     }
     
     return matcher->is_regex;
 }
 
 /**
  * @brief Check if the pattern is a glob pattern
  * 
  * @param matcher Pattern matcher
  * @return bool True if glob pattern, false otherwise
  */
 bool nlink_pattern_is_glob(const NlinkPatternMatcher* matcher) {
     if (!matcher) {
         return false;
     }
     
     return matcher->is_glob;
 }
 
 /**
  * @brief Free pattern matcher resources
  * 
  * @param matcher Pattern matcher to free
  */
 void nlink_pattern_destroy(NlinkPatternMatcher* matcher) {
     if (!matcher) {
         return;
     }
     
     // Free allocated resources
     free(matcher->pattern);
     
     // Free compiled regex if any
     if (matcher->is_compiled && matcher->is_regex) {
         regfree(&matcher->compiled.regex);
     }
     
     // Free the matcher itself
     free(matcher);
 }

