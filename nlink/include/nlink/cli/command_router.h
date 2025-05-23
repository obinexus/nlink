/**
 * @file command_router.h
 * @brief Command routing system for NexusLink CLI
 * 
 * Provides a command routing system that uses pattern matching to
 * route and execute CLI commands with parameter extraction.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMAND_ROUTER_H
 #define NLINK_COMMAND_ROUTER_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/pattern/matcher.h"
 #include "nlink/cli/command.h"
 #include "nlink/cli/command_params.h"
 
 #include <stddef.h>
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Command router object for routing and executing CLI commands
  */
 typedef struct NlinkCommandRouter NlinkCommandRouter;
 
 /**
  * @brief Create a new command router
  * 
  * @return NlinkCommandRouter* New router instance
  */
 NlinkCommandRouter* nlink_command_router_create(void);
 
 /**
  * @brief Register a command with a pattern
  * 
  * @param router Command router
  * @param pattern Pattern to match
  * @param command Command to execute
  * @param flags Pattern matching flags
  * @return NexusResult Result code
  */
 NexusResult nlink_command_router_register(
     NlinkCommandRouter* router, 
     const char* pattern, 
     NexusCommand* command,
     NlinkPatternFlags flags);
 
 /**
  * @brief Register a command with a pattern and parameter names
  * 
  * @param router Command router
  * @param pattern Pattern to match
  * @param command Command to execute
  * @param flags Pattern matching flags
  * @param param_names Array of parameter names (mapped to capturing groups)
  * @param param_count Number of parameter names
  * @return NexusResult Result code
  */
 NexusResult nlink_command_router_register_with_params(
     NlinkCommandRouter* router, 
     const char* pattern, 
     NexusCommand* command,
     NlinkPatternFlags flags,
     const char** param_names,
     size_t param_count);
 
 /**
  * @brief Execute a command that matches the input
  * 
  * @param router Command router
  * @param input Command input string
  * @param ctx Execution context
  * @return NexusResult Result code
  */
 NexusResult nlink_command_router_execute(
     NlinkCommandRouter* router, 
     const char* input,
     NexusContext* ctx);
 
 /**
  * @brief Execute a command that matches the input, with parameter extraction
  * 
  * @param router Command router
  * @param input Command input string
  * @param ctx Execution context
  * @param params Pointer to store extracted parameters (can be NULL)
  * @return NexusResult Result code
  */
 NexusResult nlink_command_router_execute_with_params(
     NlinkCommandRouter* router, 
     const char* input,
     NexusContext* ctx,
     NlinkCommandParams** params);
 
 /**
  * @brief Find commands matching a pattern
  * 
  * @param router Command router
  * @param pattern Pattern to match
  * @param commands Array to store matching commands
  * @param max_commands Maximum number of commands to return
  * @return size_t Number of commands found
  */
 size_t nlink_command_router_find(
     NlinkCommandRouter* router,
     const char* pattern,
     NexusCommand** commands,
     size_t max_commands);
 
 /**
  * @brief Free command router resources
  * 
  * @param router Command router to free
  */
 void nlink_command_router_destroy(NlinkCommandRouter* router);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMAND_ROUTER_H */