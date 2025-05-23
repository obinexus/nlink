/**
 * @file command_router.c
 * @brief Implementation of the command routing system for NexusLink CLI
 * 
 * Provides command routing capabilities using pattern matching to
 * enable flexible CLI command handling with parameter extraction.
 * 
 * Copyright © 2025 OBINexus Computing
 */
#include "nlink/cli/command_registry.h"
#include "nlink/cli/command_params.h"

 
 

/**
 * @brief Find commands matching a pattern
 * 
 * @param router Command router
 * @param pattern Pattern to match
 * @param commands Array to store matching commands
 * @param max_commands Maximum number of commands to return
 * @return size_t Number of commands found
 */
size_t nlink_command_router_find_commands(
    struct NlinkCommandRouter* router, 
    const char* pattern,
    NexusCommand** commands,
    size_t max_commands) {
    
    if (!router || !pattern || !commands || max_commands == 0) {
        return 0;
    }
    
    // Clear the command array
    for (size_t i = 0; i < max_commands; i++) {
        commands[i] = NULL;
    }

    // Check if the pattern is empty    
    if (strlen(pattern) == 0) {
        return 0;
    }

    // Create appropriate pattern matcher based on pattern type
    NlinkPatternFlags flags = NLINK_PATTERN_FLAG_CASE_INSENSITIVE;
    
    // Check if the pattern is a glob pattern
    if (strchr(pattern, '*') || strchr(pattern, '?')) {
        flags |= NLINK_PATTERN_FLAG_GLOB;
    } else {
        // Otherwise use regex
        flags |= NLINK_PATTERN_FLAG_REGEX;
    }
    
    NlinkPatternMatcher* matcher = nlink_pattern_create(pattern, flags);
    if (!matcher) {
        return 0;
    }
    
    // Find matching commands
    size_t found = 0;
    NlinkCommandRoute* route = router->routes;
    
    while (route && found < max_commands) {
        if (command_matches(route->command, matcher)) {
            commands[found++] = route->command;
        }
        
        route = route->next;
    }
    
    // Clean up
    nlink_pattern_destroy(matcher);
    
    return found;
}

 
 

 /**
  * @brief Find a command that matches the input
  * 
  * @param router Command router
  * @param input Command input string
  * @param params Pointer to store extracted parameters
  * @return NexusCommand* Matching command or NULL if none found
  */
static NexusCommand* find_matching_command(
     struct NlinkCommandRouter* router, 
     const char* input,
     NlinkCommandParams** params) {
     
     if (!router || !input) {
         return NULL;
     }
     
     // Initialize params if provided
     if (params) {
         *params = nlink_command_params_create();
         if (!*params) {
             return NULL;
         }
     }
     
     // Search through routes
     NlinkCommandRoute* route = router->routes;
     while (route) {
         // Check if this route matches with parameter extraction
         NlinkMatchInfo* match_info = NULL;
         if (nlink_pattern_match_with_params(route->matcher, input, &match_info)) {
             // If we need parameters and have a match with info
             if (params && match_info) {
                 // Get the number of groups
                 size_t group_count = nlink_match_info_get_group_count(match_info);
                 
                 // Skip group 0 (full match)
                 for (size_t i = 1; i < group_count; i++) {
                     const char* value = nlink_match_info_get_group(match_info, i);
                     
                     // Use named parameter if available
                     const char* name;
                     if (i <= route->param_count && route->param_names && route->param_names[i-1]) {
                         name = route->param_names[i-1];
                     } else {
                         // Default to param1, param2, etc.
                         char auto_name[32]; // Increased buffer size for larger numbers
                         snprintf(auto_name, sizeof(auto_name), "param%zu", i);
                         name = strdup(auto_name); // Create persistent copy of the name
                     }
                     
                     // Add parameter to the list
                     nlink_command_params_add(*params, name, value);
                 }
             }
             
             // Clean up
             if (match_info) {
                 nlink_match_info_destroy(match_info);
             }
             
             return route->command;
         }
         
         // Clean up if match failed
         if (match_info) {
             nlink_match_info_destroy(match_info);
         }
         
         route = route->next;
     }
     
     // No match found, clean up
     if (params && *params) {
         nlink_command_params_destroy(*params);
         *params = NULL;
     }
     
     return NULL;
 }
 

 
