/**
 * @file command.c
 * @brief Implementation of command functionality for NexusLink CLI
 * 
 * Provides implementation for command creation, management, and destruction.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/command.h"
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * @brief Create a new command
  * 
  * @param name Command name
  * @param description Command description
  * @param handler Command handler function
  * @return NexusCommand* New command or NULL on failure
  */
 NexusCommand* nexus_command_create(const char* name, const char* description, 
                                   NexusCommandHandler handler) {
     if (!name) {
         return NULL;
     }
     
     NexusCommand* command = (NexusCommand*)malloc(sizeof(NexusCommand));
     if (!command) {
         return NULL;
     }
     
     // Copy the name and description
     char* name_copy = strdup(name);
     if (!name_copy) {
         free(command);
         return NULL;
     }
     
     char* desc_copy = NULL;
     if (description) {
         desc_copy = strdup(description);
         if (!desc_copy) {
             free(name_copy);
             free(command);
             return NULL;
         }
     }
     
     // Initialize the command
     command->name = name_copy;
     command->description = desc_copy;
     command->handler = handler;
     
     return command;
 }
 
 /**
  * @brief Create a new extended command with parameter support
  * 
  * @param name Command name
  * @param description Command description
  * @param handler Basic command handler (can be NULL if using handler_with_params)
  * @param handler_with_params Parameter-aware command handler (can be NULL if using handler)
  * @return NexusCommand* New command or NULL on failure
  */
 NexusCommand* nexus_command_ex_create(const char* name, const char* description,
                                     NexusCommandHandler handler,
                                     NexusCommandHandlerWithParams handler_with_params) {
    NexusCommand* command = nexus_command_create(name, description, handler);
    if (command) {
        command->handler_with_params = handler_with_params;
    }
    return command;
 }
 
 /**
  * @brief Free command resources
  * 
  * @param command Command to free
  */
 void nexus_command_destroy(NexusCommand* command) {
     if (!command) {
         return;
     }
     
     // Free allocated resources
     free((void*)command->name);
     free((void*)command->description);
     
     // Free the command itself
     free(command);
 }
 
/**
 * @brief Free extended command resources (deprecated)
 * 
 * @param command Extended command to free
 * @deprecated Use nexus_command_destroy instead
 */
void nexus_command_ex_destroy(NexusCommand* command) {
     nexus_command_destroy(command);
}

