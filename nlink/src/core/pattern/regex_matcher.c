/**
 * @file regex_matcher.c
 * @brief Regex matcher implementation for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/regex_matcher.h"
 #include <stdlib.h>
 #include <string.h>
 
 RegexMatcher* regex_matcher_create(void) {
     RegexMatcher* matcher = (RegexMatcher*)malloc(sizeof(RegexMatcher));
     if (!matcher) {
         return NULL;
     }
     
     matcher->is_compiled = false;
     return matcher;
 }
 
 void regex_matcher_destroy(RegexMatcher* matcher) {
     if (!matcher) {
         return;
     }
     
     if (matcher->is_compiled) {
         regfree(&matcher->compiled);
     }
     
     free(matcher);
 }
 
 int regex_matcher_match(RegexMatcher* matcher, 
                        const char* pattern, 
                        const char* string,
                        NlinkPatternFlags flags,
                        regmatch_t* matches,
                        size_t max_matches) {
     if (!matcher || !pattern || !string) {
         return -1;
     }
     
     // Free previously compiled pattern
     if (matcher->is_compiled) {
         regfree(&matcher->compiled);
         matcher->is_compiled = false;
     }
     
     // Set up compilation flags
     int cflags = REG_EXTENDED;  // Default to extended regex
     
     // Apply additional flags
     if (flags & NLINK_PATTERN_FLAG_CASE_INSENSITIVE) {
         cflags |= REG_ICASE;
     }
     
     // Compile the pattern
     int result = regcomp(&matcher->compiled, pattern, cflags);
     if (result != 0) {
         return -1;
     }
     
     matcher->is_compiled = true;
     
     // Perform the match
     if (matches && max_matches > 0) {
         result = regexec(&matcher->compiled, string, max_matches, matches, 0);
     } else {
         regmatch_t dummy;
         result = regexec(&matcher->compiled, string, 1, &dummy, 0);
     }
     
     if (result == REG_NOMATCH) {
         return 0;  // No matches
     } else if (result != 0) {
         return -1;  // Error
     }
     
     // Count the number of matches
     if (matches && max_matches > 0) {
         size_t count = 0;
         for (size_t i = 0; i < max_matches; i++) {
             if (matches[i].rm_so == -1) {
                 break;
             }
             count++;
         }
         return (int)count;
     }
     
     // Just a single match
     return 1;
 }
 
 size_t regex_matcher_find_commands(RegexMatcher* matcher, 
                                  const char* pattern,
                                  NexusCommand** commands, 
                                  size_t count,
                                  NexusCommand** matches, 
                                  size_t max_matches) {
     if (!matcher || !pattern || !commands || !matches || count == 0 || max_matches == 0) {
         return 0;
     }
     
     size_t match_count = 0;
     
     for (size_t i = 0; i < count && match_count < max_matches; i++) {
         if (commands[i]->name &&
             regex_matcher_match(matcher, pattern, commands[i]->name, 
                               NLINK_PATTERN_FLAG_CASE_INSENSITIVE, NULL, 0) > 0) {
             matches[match_count++] = commands[i];
         }
         else if (commands[i]->short_name &&
                  regex_matcher_match(matcher, pattern, commands[i]->short_name, 
                                    NLINK_PATTERN_FLAG_CASE_INSENSITIVE, NULL, 0) > 0) {
             matches[match_count++] = commands[i];
         }
     }
     
     return match_count;
 }