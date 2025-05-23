/**
 * @file load.h
 * @brief Load command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_LOAD_COMMAND_H
 #define NEXUS_LOAD_COMMAND_H

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
  * @brief Load command structure
  */
 extern NexusCommand load_command;
 
 /**
  * @brief Get the load command structure
  * 
  * @return NexusCommand* Pointer to the command structure
  */
 NexusCommand* nlink_get_load_command(void);
 
 /**
  * @brief Execute function with minimization support
  * Exposed for use by other commands (e.g., minimal mode)
  */
 int load_command_execute(NexusContext* ctx, int argc, char** argv);
 
 #endif /* NEXUS_LOAD_COMMAND_H */