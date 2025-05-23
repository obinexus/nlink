/**
 * @file pipeline.h
 * @brief Pipeline command implementation for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_PIPELINE_COMMAND_H
 #define NEXUS_PIPELINE_COMMAND_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/cli/command_router.h"
 #include "nlink/core/pipeline/nlink_pipeline.h"

/* Forward declarations for any types not defined in included headers */
typedef struct NlinkPassManager NlinkPassManager;

 /* Define this if not available in result.h */
 #ifndef NEXUS_FAILURE
 #define NEXUS_FAILURE -1
 #endif
 
 /**
  * @brief Pipeline command data structure
  */
 typedef struct {
     NlinkPipeline* pipeline;        /**< Active pipeline */
     NlinkPassManager* pass_manager; /**< Active pass manager */
     char* config_path;              /**< Path to config file */
     NlinkPipelineMode forced_mode;  /**< Forced execution mode */
     bool enable_optimization;       /**< Enable optimization */
 } PipelineCommandData;
 

 /**
  * @brief Get the pipeline command structure
  * 
  * @return NexusCommand* Pointer to the command structure
  */
 NexusCommand* nlink_get_pipeline_command(void);
 
 #endif /* NEXUS_PIPELINE_COMMAND_H */