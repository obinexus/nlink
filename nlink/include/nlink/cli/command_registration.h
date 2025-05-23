/**
 * @file command_registration.h
 * @brief Command registration functions for NexusLink CLI
 * 
 * Provides functions for registering commands with pattern matching and
 * parameter extraction capabilities.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMAND_REGISTRATION_H
 #define NLINK_COMMAND_REGISTRATION_H
 
 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command.h"
 #include "nlink/core/common/result.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Register commands for the load functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult load_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register commands for the minimal functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult minimal_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register commands for the minimize functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult minimize_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register commands for the pipeline functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult pipeline_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register commands for the version functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult version_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register commands for the parse functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult parse_command_register_with_params(NlinkCommandRouter* router);
 
 /**
  * @brief Register all built-in commands
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult register_all_commands(NlinkCommandRouter* router);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMAND_REGISTRATION_H */