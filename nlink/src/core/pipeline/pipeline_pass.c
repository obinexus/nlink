/**
 * @file pipeline_pass.c
 * @brief Implementation of the Pipeline Pass system
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/cpppass/pipeline_pass.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdlib.h>
 #include <string.h>
 
 
 NlinkPipelinePass* nlink_pipeline_pass_create(const char* name,
                                            NlinkPassType type,
                                            NlinkPassFunc func,
                                            void* user_data) {
     if (!name || !func) {
         return NULL;
     }
     
     NlinkPipelinePass* pass = (NlinkPipelinePass*)malloc(sizeof(NlinkPipelinePass));
     if (!pass) {
         return NULL;
     }
     
     pass->name = strdup(name);
     if (!pass->name) {
         free(pass);
         return NULL;
     }
     
     pass->type = type;
     pass->func = func;
     pass->user_data = user_data;
     
     return pass;
 }
 
 NlinkPassManager* nlink_pass_manager_create(NexusContext* ctx) {
     if (!ctx) {
         return NULL;
     }
     
     NlinkPassManager* manager = (NlinkPassManager*)malloc(sizeof(NlinkPassManager));
     if (!manager) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pass manager");
         return NULL;
     }
     
     manager->ctx = ctx;
     manager->passes = NULL;
     manager->pass_count = 0;
     manager->capacity = 0;
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Created pipeline pass manager");
     return manager;
 }
 
 NexusResult nlink_pass_manager_add_pass(NlinkPassManager* manager, NlinkPipelinePass* pass) {
     if (!manager || !pass) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Check if we need to resize the passes array */
     if (manager->pass_count >= manager->capacity) {
         unsigned new_capacity = manager->capacity == 0 ? 4 : manager->capacity * 2;
         NlinkPipelinePass** new_passes = (NlinkPipelinePass**)realloc(
             manager->passes, 
             new_capacity * sizeof(NlinkPipelinePass*)
         );
         
         if (!new_passes) {
             nexus_log(manager->ctx, NEXUS_LOG_ERROR, "Failed to allocate memory for passes");
             return NEXUS_OUT_OF_MEMORY;
         }
         
         manager->passes = new_passes;
         manager->capacity = new_capacity;
     }
     
     /* Add the pass */
     manager->passes[manager->pass_count++] = pass;
     
     nexus_log(manager->ctx, NEXUS_LOG_DEBUG, "Added pass '%s' (type: %d) to manager", 
               pass->name, pass->type);
     return NEXUS_SUCCESS;
 }
 
 NexusResult nlink_pass_manager_run(NlinkPassManager* manager, NlinkPipeline* pipeline) {
     if (!manager || !pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusContext* ctx = manager->ctx;
     NexusResult result = NEXUS_SUCCESS;
     
     /* First run all analysis passes */
     nexus_log(ctx, NEXUS_LOG_INFO, "Running analysis passes...");
     for (unsigned i = 0; i < manager->pass_count && result == NEXUS_SUCCESS; i++) {
         NlinkPipelinePass* pass = manager->passes[i];
         if (pass->type == NLINK_PASS_ANALYSIS) {
             nexus_log(ctx, NEXUS_LOG_DEBUG, "Running analysis pass '%s'", pass->name);
             result = pass->func(pipeline, pass->user_data);
             
             if (result != NEXUS_SUCCESS) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Analysis pass '%s' failed with result %d", 
                           pass->name, result);
                 return result;
             }
         }
     }
     
     /* Then run all transformation passes */
     nexus_log(ctx, NEXUS_LOG_INFO, "Running transformation passes...");
     for (unsigned i = 0; i < manager->pass_count && result == NEXUS_SUCCESS; i++) {
         NlinkPipelinePass* pass = manager->passes[i];
         if (pass->type == NLINK_PASS_TRANSFORMATION) {
             nexus_log(ctx, NEXUS_LOG_DEBUG, "Running transformation pass '%s'", pass->name);
             result = pass->func(pipeline, pass->user_data);
             
             if (result != NEXUS_SUCCESS) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Transformation pass '%s' failed with result %d", 
                           pass->name, result);
                 return result;
             }
         }
     }
     
     /* Finally, run all optimization passes */
     nexus_log(ctx, NEXUS_LOG_INFO, "Running optimization passes...");
     for (unsigned i = 0; i < manager->pass_count && result == NEXUS_SUCCESS; i++) {
         NlinkPipelinePass* pass = manager->passes[i];
         if (pass->type == NLINK_PASS_OPTIMIZATION) {
             nexus_log(ctx, NEXUS_LOG_DEBUG, "Running optimization pass '%s'", pass->name);
             result = pass->func(pipeline, pass->user_data);
             
             if (result != NEXUS_SUCCESS) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Optimization pass '%s' failed with result %d", 
                           pass->name, result);
                 return result;
             }
         }
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Completed running %u passes on pipeline", manager->pass_count);
     return result;
 }
 
 void nlink_pipeline_pass_destroy(NlinkPipelinePass* pass) {
     if (!pass) {
         return;
     }
     
     free(pass->name);
     free(pass);
 }
 
 void nlink_pass_manager_destroy(NlinkPassManager* manager) {
     if (!manager) {
         return;
     }
     
     /* Free all passes */
     for (unsigned i = 0; i < manager->pass_count; i++) {
         nlink_pipeline_pass_destroy(manager->passes[i]);
     }
     
     free(manager->passes);
     free(manager);
 }
 
 /**
  * Implementation of built-in passes
  */
 
 NexusResult nlink_dependency_analysis_pass(NlinkPipeline* pipeline, void* user_data) {
     if (!pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* In a real implementation, this would analyze the pipeline structure
        to identify dependencies between stages and detect circular references. */
     
     /* For now, just return success */
     return NEXUS_SUCCESS;
 }
 
 NexusResult nlink_pipeline_optimizer_pass(NlinkPipeline* pipeline, void* user_data) {
     if (!pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* In a real implementation, this would optimize the pipeline by:
        1. Rearranging stages for better data locality
        2. Combining compatible stages
        3. Applying stage-specific optimizations */
     
     /* For now, just return success */
     return NEXUS_SUCCESS;
 }
 
 NexusResult nlink_pipeline_validator_pass(NlinkPipeline* pipeline, void* user_data) {
     if (!pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* In a real implementation, this would validate the pipeline against:
        1. Schema constraints
        2. Design rules
        3. Compatible data formats between stages */
     
     /* For now, just return success */
     return NEXUS_SUCCESS;
 }