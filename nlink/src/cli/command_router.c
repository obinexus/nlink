/**
 * @file command_router.c
 * @brief Implementation of the command routing system for NexusLink CLI
 * 
 * Provides command routing capabilities using pattern matching to
 * enable flexible CLI command handling with parameter extraction.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/core/pattern/matcher.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdlib.h>
 #include <string.h>
 #include <stdio.h>
 
 /**
  * @brief Forward declaration for match info structure
  */
 typedef struct NlinkMatchInfo NlinkMatchInfo;
 
 /**
  * @brief Command route entry
  */
 typedef struct NlinkCommandRoute {
     NlinkPatternMatcher* matcher;     /**< Pattern matcher */
     NexusCommand* command;            /**< Command to execute */
     char** param_names;               /**< Parameter names (mapped to capturing groups) */
     size_t param_count;               /**< Number of parameter names */
     struct NlinkCommandRoute* next;   /**< Next route in the list */
 } NlinkCommandRoute;
 
 /**
  * @brief Command router implementation
  */
 struct NlinkCommandRouter {
     NlinkCommandRoute* routes;     /**< Linked list of routes */
     size_t route_count;            /**< Number of routes */
 };
 
 /* Forward declarations */
 static bool command_matches(NexusCommand* command, NlinkPatternMatcher* matcher);
 static NexusCommand* find_matching_command(
     NlinkCommandRouter* router, 
     const char* input,
     NlinkCommandParams** params);
 
 /**
  * @brief Create a new command router
  */
 NlinkCommandRouter* nlink_command_router_create(void) {
     NlinkCommandRouter* router = (NlinkCommandRouter*)malloc(sizeof(NlinkCommandRouter));
     if (!router) {
         return NULL;
     }
     
     /* Initialize router */
     router->routes = NULL;
     router->route_count = 0;
     
     return router;
 }
 
 /**
  * @brief Register a command with a pattern
  */
 NexusResult nlink_command_router_register(
     NlinkCommandRouter* router, 
     const char* pattern, 
     NexusCommand* command,
     NlinkPatternFlags flags) {
     
     return nlink_command_router_register_with_params(
         router, pattern, command, flags, NULL, 0);
 }
 
 /**
  * @brief Register a command with a pattern and parameter names
  */
 NexusResult nlink_command_router_register_with_params(
     NlinkCommandRouter* router, 
     const char* pattern, 
     NexusCommand* command,
     NlinkPatternFlags flags,
     const char** param_names,
     size_t param_count) {
     
     if (!router || !pattern || !command) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Create pattern matcher */
     NlinkPatternMatcher* matcher = nlink_pattern_create(pattern, flags);
     if (!matcher) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Create new route */
     NlinkCommandRoute* route = (NlinkCommandRoute*)malloc(sizeof(NlinkCommandRoute));
     if (!route) {
         nlink_pattern_destroy(matcher);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Initialize route */
     route->matcher = matcher;
     route->command = command;
     route->param_names = NULL;
     route->param_count = 0;
     
     /* If parameter names are provided, copy them */
     if (param_names && param_count > 0) {
         route->param_names = (char**)malloc(param_count * sizeof(char*));
         if (!route->param_names) {
             nlink_pattern_destroy(matcher);
             free(route);
             return NEXUS_OUT_OF_MEMORY;
         }
         
         for (size_t i = 0; i < param_count; i++) {
             if (param_names[i]) {
                 route->param_names[i] = strdup(param_names[i]);
                 if (!route->param_names[i]) {
                     /* Cleanup on failure */
                     for (size_t j = 0; j < i; j++) {
                         free(route->param_names[j]);
                     }
                     free(route->param_names);
                     nlink_pattern_destroy(matcher);
                     free(route);
                     return NEXUS_OUT_OF_MEMORY;
                 }
             } else {
                 route->param_names[i] = NULL;
             }
         }
         
         route->param_count = param_count;
     }
     
     /* Add to the linked list (prepend for simplicity) */
     route->next = router->routes;
     router->routes = route;
     router->route_count++;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Find a command that matches the input
  */
 static NexusCommand* find_matching_command(
     NlinkCommandRouter* router, 
     const char* input,
     NlinkCommandParams** params) {
     
     if (!router || !input) {
         return NULL;
     }
     
     /* Initialize params if provided */
     if (params) {
         *params = nlink_command_params_create();
         if (!*params) {
             return NULL;
         }
     }
     
     /* Search through routes */
     NlinkCommandRoute* route = router->routes;
     while (route) {
         /* Check if this route matches with parameter extraction */
         NlinkMatchInfo* match_info = NULL;
         if (nlink_pattern_match_with_params(route->matcher, input, &match_info)) {
             /* If we need parameters and have a match with info */
             if (params && match_info) {
                 /* Get the number of groups */
                 size_t group_count = nlink_match_info_get_group_count(match_info);
                 
                 /* Skip group 0 (full match) */
                 for (size_t i = 1; i < group_count; i++) {
                     const char* value = nlink_match_info_get_group(match_info, i);
                     
                     /* Use named parameter if available */
                     const char* param_name = (i-1 < route->param_count && route->param_names && route->param_names[i-1]) 
                         ? route->param_names[i-1] : NULL;
                     
                     if (param_name) {
                         nlink_command_params_add(*params, param_name, value);
                     } else {
                         /* If no name, use numbered parameter */
                         char buffer[32];
                         snprintf(buffer, sizeof(buffer), "param%zu", i);
                         nlink_command_params_add(*params, buffer, value);
                     }
                 }
                 
                 /* Clean up match info */
                 nlink_match_info_destroy(match_info);
             }
             
             /* Return the matched command */
             return route->command;
         }
         
         /* Clean up if match failed */
         if (match_info) {
             nlink_match_info_destroy(match_info);
         }
         
         route = route->next;
     }
     
     /* No match found, clean up */
     if (params && *params) {
         nlink_command_params_destroy(*params);
         *params = NULL;
     }
     
     return NULL;
 }
 
 /**
  * @brief Execute a command that matches the input
  */
 NexusResult nlink_command_router_execute(
     NlinkCommandRouter* router, 
     const char* input,
     NexusContext* ctx) {
     
     return nlink_command_router_execute_with_params(
         router, input, ctx, NULL);
 }
 
 /**
  * @brief Execute a command that matches the input, with parameter extraction
  */
 NexusResult nlink_command_router_execute_with_params(
     NlinkCommandRouter* router, 
     const char* input,
     NexusContext* ctx,
     NlinkCommandParams** params) {
     
     if (!router || !input || !ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Find matching command with parameter extraction */
     NlinkCommandParams* extracted_params = NULL;
     NexusCommand* command = find_matching_command(router, input, &extracted_params);
     
     if (!command) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "No command found matching '%s'", input);
         return NEXUS_NOT_FOUND;
     }
     
     /* If caller wants the parameters, transfer ownership */
     if (params) {
         *params = extracted_params;
     } else {
         /* Otherwise, clean up */
         nlink_command_params_destroy(extracted_params);
         extracted_params = NULL;
     }
     
     /* Execute the command with parameters */
     if (command->handler_with_params && extracted_params) {
         return command->handler_with_params(ctx, extracted_params);
     } else if (command->handler) {
         /* Fall back to basic handler */
         return command->handler(ctx);
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Find commands matching a pattern
  */
 size_t nlink_command_router_find(
     NlinkCommandRouter* router,
     const char* pattern,
     NexusCommand** commands,
     size_t max_commands) {
     
     if (!router || !pattern || !commands || max_commands == 0) {
         return 0;
     }
     
     /* Create pattern matcher for the search pattern */
     NlinkPatternMatcher* matcher = nlink_pattern_create(
         pattern, 
         NLINK_PATTERN_FLAG_GLOB | NLINK_PATTERN_FLAG_CASE_INSENSITIVE
     );
     
     if (!matcher) {
         return 0;
     }
     
     /* Find matching commands */
     size_t found = 0;
     NlinkCommandRoute* route = router->routes;
     
     while (route && found < max_commands) {
         if (command_matches(route->command, matcher)) {
             commands[found++] = route->command;
         }
         
         route = route->next;
     }
     
     /* Clean up */
     nlink_pattern_destroy(matcher);
     
     return found;
 }
 
 /**
  * @brief Check if a command matches a pattern (by name)
  */
 static bool command_matches(NexusCommand* command, NlinkPatternMatcher* matcher) {
     if (!command || !command->name || !matcher) {
         return false;
     }
     
     return nlink_pattern_match(matcher, command->name);
 }
 
 /**
  * @brief Free command router resources
  */
 void nlink_command_router_destroy(NlinkCommandRouter* router) {
     if (!router) {
         return;
     }
     
     /* Free all routes */
     NlinkCommandRoute* route = router->routes;
     while (route) {
         NlinkCommandRoute* next = route->next;
         
         /* Free the pattern matcher */
         nlink_pattern_destroy(route->matcher);
         
         /* Free parameter names */
         if (route->param_names) {
             for (size_t i = 0; i < route->param_count; i++) {
                 free(route->param_names[i]);
             }
             free(route->param_names);
         }
         
         /* Note: We don't free the command since it's owned elsewhere */
         
         /* Free the route itself */
         free(route);
         
         route = next;
     }
     
     /* Free the router itself */
     free(router);
 }
