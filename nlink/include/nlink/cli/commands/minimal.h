/**
 * @file minimal.h
 * @brief Minimal command interface for NexusLink CLI
 * 
 * Provides a minimal syntax mode for NexusLink CLI, allowing shortcuts
 * for common operations with a concise syntax.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_MINIMAL_COMMAND_H
 #define NEXUS_MINIMAL_COMMAND_H


 #include "nlink/core/common/nexus_loader.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/command.h"
#include "nlink/core/minimizer/okpala_automaton.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <sys/stat.h>
 #include <time.h>
 
 /**
  * @brief Minimal command structure
  */
 extern NexusCommand minimal_command;

    /**
    * @brief Get the minimal command structure
    * 
    * @return NexusCommand* Pointer to the command structure
    */
    NexusCommand* nlink_get_minimal_command(void);
    
 
 /**
  * @brief Parse a minimal command string
  * 
  * Format: [component][@version][:function][=args]
  * Examples:
  *   - "logger" - Load the logger component
  *   - "logger@1.2.3" - Load logger version 1.2.3
  *   - "logger:log" - Load logger and call log function
  *   - "logger@1.2.3:log=Hello World" - Load logger 1.2.3, call log with "Hello World"
  * 
  * @param input The minimal command string to parse
  * @param component Output for component name (must be freed by caller)
  * @param version Output for version (must be freed by caller, can be NULL)
  * @param function Output for function name (must be freed by caller, can be NULL)
  * @param args Output for arguments (must be freed by caller, can be NULL)
  * @return true if parsing was successful, false otherwise
  */
 bool minimal_parse(const char* input, 
                   char** component, 
                   char** version, 
                   char** function, 
                   char** args);
 
 #endif /* NEXUS_MINIMAL_COMMAND_H */