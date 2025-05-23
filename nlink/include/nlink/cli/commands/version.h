/**
 * @file version.h
 * @brief Version command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_VERSION_COMMAND_H
 #define NEXUS_VERSION_COMMAND_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command.h"  /* This is needed for NexusCommand */
 #include "nlink/cli/command_router.h"  /* This is needed for NlinkCommandRouter */
 #include "nlink/cli/command_params.h"  /* This is needed for NlinkCommandParams */
 #include "nlink/cli/commands/version_utils.h"  /* This is needed for version utilities */
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Version command structure
  */
 extern NexusCommand version_command;
 
 /**
  * @brief Get the version command structure
  * 
  * @return NexusCommand* Pointer to the command structure
  */
 NexusCommand* nlink_get_version_command(void);
 
 /**
  * @brief Register the version command with parameter extraction
  * 
  * @param router Command router to register with
  * @return NexusResult Registration result
  */
 NexusResult version_command_register_with_params(NlinkCommandRouter* router);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NEXUS_VERSION_COMMAND_H */