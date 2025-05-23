/**
 * @file pipeline_pass.h
 * @brief Component Pipeline Pass System (CPPass)
 * 
 * Defines the component pass system for optimizing pipeline flows
 * by analyzing dependencies and execution patterns.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_PIPELINE_PASS_H
 #define NLINK_PIPELINE_PASS_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/pipeline/nlink_pipeline.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /* Forward declarations */
 typedef struct NlinkPipelinePass NlinkPipelinePass;
 typedef struct NlinkPassManager NlinkPassManager;
 
 /**
  * @brief Pass type enumeration
  */
 typedef enum {
     NLINK_PASS_ANALYSIS,       /**< Analysis pass, doesn't modify components */
     NLINK_PASS_TRANSFORMATION, /**< Transformation pass, modifies components */
     NLINK_PASS_OPTIMIZATION    /**< Optimization pass, specialized transformation */
 } NlinkPassType;
 
 /**
  * @brief Pass function prototype
  */
 typedef NexusResult (*NlinkPassFunc)(NlinkPipeline* pipeline, void* user_data);
 
 /**
  * @brief Pipeline pass structure
  */
 struct NlinkPipelinePass {
     char* name;                /**< Pass name */
     NlinkPassType type;        /**< Pass type */
     NlinkPassFunc func;        /**< Pass function */
     void* user_data;           /**< User data for pass function */
 };
 
 /**
  * @brief Pass manager implementation
  */
 struct NlinkPassManager {
     NexusContext* ctx;         /**< NexusLink context */
     NlinkPipelinePass** passes; /**< Array of passes */
     unsigned pass_count;       /**< Number of passes */
     unsigned capacity;         /**< Allocated capacity for passes array */
 };
 
 /**
  * @brief Create a new pipeline pass
  * 
  * @param name Pass name
  * @param type Pass type
  * @param func Pass function
  * @param user_data User data passed to the pass function
  * @return NlinkPipelinePass* New pass or NULL on failure
  */
 NlinkPipelinePass* nlink_pipeline_pass_create(const char* name,
                                             NlinkPassType type,
                                             NlinkPassFunc func,
                                             void* user_data);
 
 /**
  * @brief Create a new pass manager
  * 
  * @param ctx NexusLink context
  * @return NlinkPassManager* New pass manager or NULL on failure
  */
 NlinkPassManager* nlink_pass_manager_create(NexusContext* ctx);
 
 /**
  * @brief Add a pass to the manager
  * 
  * @param manager Pass manager
  * @param pass Pass to add
  * @return NexusResult Result code
  */
 NexusResult nlink_pass_manager_add_pass(NlinkPassManager* manager, NlinkPipelinePass* pass);
 
 /**
  * @brief Run all passes on a pipeline
  * 
  * @param manager Pass manager
  * @param pipeline Target pipeline
  * @return NexusResult Result code
  */
 NexusResult nlink_pass_manager_run(NlinkPassManager* manager, NlinkPipeline* pipeline);
 
 /**
  * @brief Destroy a pipeline pass
  * 
  * @param pass Pass to destroy
  */
 void nlink_pipeline_pass_destroy(NlinkPipelinePass* pass);
 
 /**
  * @brief Destroy a pass manager
  * 
  * @param manager Pass manager to destroy
  */
 void nlink_pass_manager_destroy(NlinkPassManager* manager);
 
 /**
  * @brief Built-in dependency analysis pass
  * 
  * Analyzes component dependencies and identifies circular references.
  * 
  * @param pipeline Target pipeline
  * @param user_data User data
  * @return NexusResult Result code
  */
 NexusResult nlink_dependency_analysis_pass(NlinkPipeline* pipeline, void* user_data);
 
 /**
  * @brief Built-in pipeline optimizer pass
  * 
  * Optimizes the pipeline by rearranging stages, combining compatible
  * stages, and applying stage-specific optimizations.
  * 
  * @param pipeline Target pipeline
  * @param user_data User data
  * @return NexusResult Result code
  */
 NexusResult nlink_pipeline_optimizer_pass(NlinkPipeline* pipeline, void* user_data);
 
 /**
  * @brief Built-in pipeline validator pass
  * 
  * Validates the pipeline conforms to schema and design constraints.
  * 
  * @param pipeline Target pipeline
  * @param user_data User data
  * @return NexusResult Result code
  */
 NexusResult nlink_pipeline_validator_pass(NlinkPipeline* pipeline, void* user_data);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_PIPELINE_PASS_H */