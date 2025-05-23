/**
 * @file command_registry.h
 * @brief CLI-specific command registry for NexusLink CLI
 * 
 * Provides CLI-specific command registry functionality that doesn't
 * conflict with the core command registry implementation.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CLI_COMMAND_REGISTRY_H
 #define NLINK_CLI_COMMAND_REGISTRY_H
 
 #include <stdbool.h>
 #include <stddef.h>
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/command.h"
 #include "nlink/core/pattern/matcher.h"
 #include <stdlib.h>
 #include <string.h>
 #include <stdio.h>
 #include <stdbool.h>

 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief CLI Command registry structure
  * 
  * This is a CLI-specific registry that doesn't conflict with the core registry.
  */
 typedef struct {
     NexusCommand** commands;  /* Array of command pointers */
     size_t count;             /* Number of registered commands */
     size_t capacity;          /* Allocated capacity */
 } CliCommandRegistry;
 
 
 /**
  * @brief Forward declaration for match info structure
  */
 typedef struct NlinkMatchInfo NlinkMatchInfo;
 /**
    * @brief Command route entry structure
    */
 typedef struct NlinkCommandRoute {
         NlinkPatternMatcher* matcher;     /**< Pattern matcher */
         NexusCommand* command;            /**< Command to execute */
         char** param_names;               /**< Parameter names (mapped to capturing groups) */
         size_t param_count;               /**< Number of parameter names */
         struct NlinkCommandRoute* next;   /**< Next route in the list */
 } NlinkCommandRoute;
 
 /**
    * @brief Command router structure
    */
 typedef struct NlinkCommandRouter {
         NlinkCommandRoute* routes;     /**< Linked list of routes */
         size_t route_count;            /**< Number of routes */
 } NlinkCommandRouter;

 /**
 * @brief Register a command with parameter mapping
 * @param router Command router
 * @param pattern Command pattern
 * @param command Command to execute
 * @param flags Pattern matching flags
 * @param param_names Parameter names
 * @param param_count Parameter count
 * @return NexusResult status
 */
NexusResult nlink_command_router_register_with_params(
    struct NlinkCommandRouter* router,
    const char* pattern,
    NexusCommand* command,
    NlinkPatternFlags flags,
    const char** param_names,
    size_t param_count);
 /**
  * @brief Initialize a CLI command registry
  * @param registry Pointer to registry to initialize
  * @return true on success, false on failure
  */
 bool cli_command_registry_init(CliCommandRegistry* registry);
 
 /**
  * @brief Register a command with the CLI registry
  * @param registry Target registry
  * @param command Command to register
  * @return true on success, false on failure
  */
 bool cli_command_registry_register(CliCommandRegistry* registry, NexusCommand* command);
 
 /**
  * @brief Find a command by name in the CLI registry
  * @param registry Registry to search in
  * @param name Command name to find
  * @return Pointer to command if found, NULL otherwise
  */
 NexusCommand* cli_command_registry_find(CliCommandRegistry* registry, const char* name);
 
 /**
  * @brief Execute a command by name from the CLI registry
  * @param registry Registry containing commands
  * @param ctx NexusLink context
  * @param name Name of command to execute
  * @param argc Argument count
  * @param argv Argument vector
  * @return Command's return value or error code if command not found
  */
 int cli_command_registry_execute(CliCommandRegistry* registry, 
                                 NexusContext* ctx,
                                 const char* name, 
                                 int argc, char** argv);
 
 /**
  * @brief List all commands in the CLI registry
  * @param registry Registry to list
  */
 void cli_command_registry_list(CliCommandRegistry* registry);
 
 /**
  * @brief Free all resources used by the CLI registry
  * @param registry Registry to clean up
  */
 void cli_command_registry_cleanup(CliCommandRegistry* registry);

 /**
  * @brief Execute a command with parameter map
  * @param router Command router
  * @param command_string Command string to execute
  * @param ctx NexusLink context
  * @param params Output parameter map
  * @return NexusResult status
  */
 NexusResult nlink_command_router_execute_with_params(
     struct NlinkCommandRouter* router,
     const char* command_string,
     NexusContext* ctx,
     NlinkCommandParams** params);

 /**
 /**
  * @brief Register a command with parameter mapping
  * @param router Command router
  * @param pattern Command pattern
  * @param command Command to execute
  * @param flags Pattern matching flags
  * @param param_names Parameter names
  * @param param_count Parameter count
  * @return NexusResult status
  */
 NexusResult nlink_command_router_register_with_params(
     struct NlinkCommandRouter* router,
     const char* pattern,
     NexusCommand* command,
     NlinkPatternFlags flags,
     const char** param_names,
     size_t param_count);
 /**
  * @brief Parse command helper
  */
 NexusCommand* parse_command(const char* name);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CLI_COMMAND_REGISTRY_H */