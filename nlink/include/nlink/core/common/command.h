/**
 * @file command.h
 * @brief Unified command interface for NexusLink
 * 
 * Provides common command structures and functions used throughout
 * the NexusLink system for command registration and execution.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMAND_H
 #define NLINK_COMMAND_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include <stdbool.h>
 #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Forward declaration for command parameters
  */
 typedef struct NlinkCommandParams NlinkCommandParams;
 
 /**
  * @brief Command execution function type (basic)
  */
 typedef int (*NlinkCommandExecFunc)(NexusContext* ctx, int argc, char** argv);
 
 /**
  * @brief Command handler function type (basic)
  */
 typedef NexusResult (*NlinkCommandHandler)(NexusContext* ctx);
 
 /**
  * @brief Command handler function type with parameter support
  */
 typedef NexusResult (*NlinkCommandHandlerWithParams)(NexusContext* ctx, NlinkCommandParams* params);
 
 /**
  * @brief Command argument parsing function type
  */
 typedef bool (*NlinkCommandParseArgsFunc)(int argc, char** argv, void** command_data);
 
 /**
  * @brief Command help print function type
  */
 typedef void (*NlinkCommandPrintHelpFunc)(void);
 
 /**
  * @brief Command data cleanup function type
  */
 typedef void (*NlinkCommandFreeDataFunc)(void* command_data);
 
 /**
  * @brief Command structure
  */
 typedef struct NexusCommand {
     const char* name;                       /**< Command name */
     const char* short_name;                 /**< Short name/alias (can be NULL) */
     const char* description;                /**< Command description */
     const char* help;                       /**< Help text */
     NlinkCommandHandler handler;            /**< Basic handler function */
     NlinkCommandHandlerWithParams handler_with_params; /**< Parameter-aware handler */
     NlinkCommandParseArgsFunc parse_args;   /**< Argument parsing function */
     NlinkCommandPrintHelpFunc print_help;   /**< Help printing function */
     NlinkCommandExecFunc execute;           /**< Execution function */
     NlinkCommandFreeDataFunc free_data;     /**< Data cleanup function */
     void* data;                             /**< Command-specific data */
 } NexusCommand;
 
 /**
  * @brief Command registry structure
  */
 typedef struct CommandRegistry {
     NexusCommand** commands;                /**< Array of registered commands */
     size_t count;                           /**< Number of commands */
     size_t capacity;                        /**< Allocated capacity */
 } CommandRegistry;
 
 /**
  * @brief Initialize a command registry
  * 
  * @param registry Registry to initialize
  * @return bool True if initialization successful, false otherwise
  */
 bool command_registry_init(CommandRegistry* registry);
 
 /**
  * @brief Register a command with the registry
  * 
  * @param registry Registry to register with
  * @param command Command to register
  * @return bool True if registration successful, false otherwise
  */
 bool command_registry_register(CommandRegistry* registry, NexusCommand* command);
 
 /**
  * @brief Find a command by name
  * 
  * @param registry Registry to search
  * @param name Name of command to find
  * @return NexusCommand* Found command or NULL if not found
  */
 NexusCommand* command_registry_find(CommandRegistry* registry, const char* name);
 
 /**
  * @brief Execute a command by name
  * 
  * @param registry Registry to execute from
  * @param ctx Context for execution
  * @param name Command name
  * @param argc Argument count
  * @param argv Argument values
  * @return int Command execution result
  */
 int command_registry_execute(CommandRegistry* registry, 
                             NexusContext* ctx,
                             const char* name, 
                             int argc, 
                             char** argv);
 
 /**
  * @brief List all commands in the registry
  * 
  * @param registry Registry to list
  */
 void command_registry_list(CommandRegistry* registry);
 
 /**
  * @brief Clean up a command registry
  * 
  * @param registry Registry to clean up
  */
 void command_registry_cleanup(CommandRegistry* registry);
 
 /**
  * @brief Create a new command
  * 
  * @param name Command name
  * @param description Command description
  * @param handler Command handler function
  * @return NexusCommand* New command or NULL on failure
  */
 NexusCommand* nexus_command_create(const char* name, const char* description, 
                                  NlinkCommandHandler handler);
 
 /**
  * @brief Free command resources
  * 
  * @param command Command to free
  */
 void nexus_command_destroy(NexusCommand* command);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMAND_H */