/**
 * @file command.h
 * @brief Command structure definitions for NexusLink CLI
 * 
 * Defines the command structure and related functions for the NexusLink CLI system.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMAND_H
 #define NLINK_COMMAND_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command_params.h"
 #include <stddef.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Forward declarations
  */
 typedef struct NexusCommand NexusCommand;
 
 /**
  * @brief Command handler function type (basic form)
  */
 typedef NexusResult (*NexusCommandHandler)(NexusContext* ctx);
 
 /**
  * @brief Command handler function type with parameters
  */
 typedef NexusResult (*NexusCommandHandlerWithParams)(NexusContext* ctx, NlinkCommandParams* params);
 
 /**
  * @brief Legacy command execution function type
  */
 typedef int (*NexusCommandExecute)(NexusContext* ctx, int argc, char** argv);
 
 /**
  * @brief Command information structure
  */
struct NexusCommand {
    const char* name;                       /**< Command name */
    const char* description;                /**< Command description */
    const char* usage;                      /**< Command usage string */
    const char* examples;                   /**< Command usage examples */
    const char* options;                    /**< Command options description */
    
    NexusCommandHandler handler;            /**< Basic command handler */
    NexusCommandHandlerWithParams handler_with_params; /**< Handler with parameters */
    NexusCommandExecute execute;            /**< Legacy execution function (deprecated) */
    
    bool internal;                          /**< Whether this is an internal command */
    const char* category;                   /**< Command category for grouping */
    const char* version;                    /**< Command version string */
    
    void* data;                             /**< Command-specific data */
};
 
 /**
  * @brief Create a new command
  * 
  * @param name Command name
  * @param description Command description
  * @param handler Command handler function
  * @return NexusCommand* New command or NULL on failure
  */
 NexusCommand* nlink_command_create(
     const char* name,
     const char* description,
     NexusCommandHandler handler);
 
 /**
  * @brief Create a new command with parameter support
  * 
  * @param name Command name
  * @param description Command description
  * @param handler Command handler function with parameters
  * @return NexusCommand* New command or NULL on failure
  */
 NexusCommand* nlink_command_create_with_params(
     const char* name,
     const char* description,
     NexusCommandHandlerWithParams handler);
 
 /**
  * @brief Free a command
  * 
  * @param command Command to free
  */
 void nlink_command_free(NexusCommand* command);
 
 /**
  * @brief Set command attributes
  * 
  * @param command Command to modify
  * @param usage Usage string
  * @param examples Examples string
  * @param options Options string
  * @param category Category string
  * @return NexusResult Result code
  */
 NexusResult nlink_command_set_attributes(
     NexusCommand* command,
     const char* usage,
     const char* examples,
     const char* options,
     const char* category);
 
 /**
  * @brief Print command help
  * 
  * @param command Command to print help for
  */
 void nlink_command_print_help(const NexusCommand* command);
 
 /**
  * @brief Compare two commands by name
  * 
  * @param a First command
  * @param b Second command
  * @return int Comparison result (-1, 0, 1)
  */
 int nlink_command_compare(const NexusCommand* a, const NexusCommand* b);
 
 /**
  * @brief Wrapper to adapt legacy command to new parameter-based format
  * 
  * @param ctx NexusLink context
  * @param params Command parameters
  * @param execute Legacy execution function
  * @return NexusResult Result code
  */
 NexusResult nlink_command_adapt_legacy(
     NexusContext* ctx,
     NlinkCommandParams* params,
     NexusCommandExecute execute);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMAND_H */