/**
 * @file minimize.h
 * @brief Minimize command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_MINIMIZE_COMMAND_H
 #define NEXUS_MINIMIZE_COMMAND_H
 
 #include "nlink/core/minimizer/nexus_minimizer.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /**
  * @brief Get the minimize command structure
  * 
  * @return NexusCommand* Pointer to the command structure
  */
 NexusCommand* nlink_get_minimize_command(void);
 
 #endif /* NEXUS_MINIMIZE_COMMAND_H */