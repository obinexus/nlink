/**
 * @file pipeline.c
 * @brief Implementation of pipeline commands for NexusLink CLI
 * 
 * Provides CLI commands for creating, configuring, and executing pipelines.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/pipeline.h"
 #include "nlink/cli/command_registration.h"  // Include the central registration header
 #include "nlink/core/pipeline/nlink_pipeline.h"
 #include "nlink/core/pipeline/pipeline_pass.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /* Pipeline command global definition */
 static NexusCommand pipeline_command;
 
 /* Function to simulate data processing for pipeline stage */
 static NexusResult dummy_stage_func(void* input, void* output, void* user_data) {
     /* In a real implementation, this would process data */
     /* For now, just memcpy input to output */
     memcpy(output, input, 1024);  /* Simplified, would use actual size */
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Create pipeline command handler
  */
 static NexusResult pipeline_create_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Get command data */
     PipelineCommandData* data = (PipelineCommandData*)pipeline_command.data;
     if (!data) {
         data = (PipelineCommandData*)malloc(sizeof(PipelineCommandData));
         if (!data) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline command data");
             return NEXUS_OUT_OF_MEMORY;
         }
         
         /* Initialize data */
         memset(data, 0, sizeof(PipelineCommandData));
         data->forced_mode = NLINK_PIPELINE_MODE_AUTO;
         data->enable_optimization = true;
         
         /* Store the data */
         pipeline_command.data = data;
     }
     
     /* Clean up existing pipeline */
     if (data->pipeline) {
         nlink_pipeline_destroy(data->pipeline);
         data->pipeline = NULL;
     }
     
     /* Get parameters */
     const char* mode_str = nlink_command_params_get(params, "mode");
     const char* config_path = nlink_command_params_get(params, "config");
     const char* opt_str = nlink_command_params_get(params, "optimization");
     
     /* Parse mode */
     NlinkPipelineMode mode = NLINK_PIPELINE_MODE_AUTO;
     if (mode_str) {
         if (strcmp(mode_str, "single") == 0 || strcmp(mode_str, "single-pass") == 0) {
             mode = NLINK_PIPELINE_MODE_SINGLE_PASS;
         } else if (strcmp(mode_str, "multi") == 0 || strcmp(mode_str, "multi-pass") == 0) {
             mode = NLINK_PIPELINE_MODE_MULTI_PASS;
         } else {
             nexus_log(ctx, NEXUS_LOG_WARNING, "Unknown pipeline mode '%s', using auto", mode_str);
         }
     }
     
     /* Parse optimization flag */
     bool enable_optimization = data->enable_optimization;
     if (opt_str) {
         if (strcmp(opt_str, "enabled") == 0 || strcmp(opt_str, "on") == 0 || 
             strcmp(opt_str, "yes") == 0 || strcmp(opt_str, "true") == 0 || 
             strcmp(opt_str, "1") == 0) {
             enable_optimization = true;
         } else if (strcmp(opt_str, "disabled") == 0 || strcmp(opt_str, "off") == 0 || 
                    strcmp(opt_str, "no") == 0 || strcmp(opt_str, "false") == 0 || 
                    strcmp(opt_str, "0") == 0) {
             enable_optimization = false;
         } else {
             nexus_log(ctx, NEXUS_LOG_WARNING, "Unknown optimization value '%s', using default", opt_str);
         }
     }
     
     /* Update stored values */
     data->forced_mode = mode;
     data->enable_optimization = enable_optimization;
     
     if (config_path) {
         /* Store config path */
         if (data->config_path) {
             free(data->config_path);
         }
         data->config_path = strdup(config_path);
     }
     
     /* Create pipeline configuration */
     NlinkPipelineConfig config = nlink_pipeline_default_config();
     config.mode = mode;
     config.enable_optimization = enable_optimization;
     config.schema_path = data->config_path;
     
     /* Create pipeline */
     data->pipeline = nlink_pipeline_create(ctx, &config);
     if (!data->pipeline) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create pipeline");
         return NEXUS_FAILURE;
     }
     
     /* Create pass manager if needed */
     if (enable_optimization && !data->pass_manager) {
         data->pass_manager = nlink_pass_manager_create(ctx);
         if (!data->pass_manager) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create pass manager");
             nlink_pipeline_destroy(data->pipeline);
             data->pipeline = NULL;
             return NEXUS_FAILURE;
         }
         
         /* Add standard passes */
         NlinkPipelinePass* analysis_pass = nlink_pipeline_pass_create(
             "dependency-analysis", 
             NLINK_PASS_ANALYSIS, 
             nlink_dependency_analysis_pass, 
             NULL
         );
         
         NlinkPipelinePass* validator_pass = nlink_pipeline_pass_create(
             "validator", 
             NLINK_PASS_ANALYSIS, 
             nlink_pipeline_validator_pass, 
             NULL
         );
         
         NlinkPipelinePass* optimizer_pass = nlink_pipeline_pass_create(
             "optimizer", 
             NLINK_PASS_OPTIMIZATION, 
             nlink_pipeline_optimizer_pass, 
             NULL
         );
         
         nlink_pass_manager_add_pass(data->pass_manager, analysis_pass);
         nlink_pass_manager_add_pass(data->pass_manager, validator_pass);
         nlink_pass_manager_add_pass(data->pass_manager, optimizer_pass);
     }
     
     /* Log success */
     nexus_log(ctx, NEXUS_LOG_INFO, "Created pipeline in %s mode with optimization %s",
              mode == NLINK_PIPELINE_MODE_SINGLE_PASS ? "single-pass" :
              mode == NLINK_PIPELINE_MODE_MULTI_PASS ? "multi-pass" : "auto",
              enable_optimization ? "enabled" : "disabled");
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Add stage to pipeline command handler
  */
 static NexusResult pipeline_add_stage_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Get command data */
     PipelineCommandData* data = (PipelineCommandData*)pipeline_command.data;
     if (!data || !data->pipeline) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "No active pipeline. Create one first with 'pipeline create'");
         return NEXUS_NOT_INITIALIZED;
     }
     
     /* Get parameters */
     const char* name = nlink_command_params_get(params, "name");
     if (!name) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Stage name required");
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Add the stage with our dummy function */
     NexusResult result = nlink_pipeline_add_stage(data->pipeline, name, dummy_stage_func, NULL);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to add stage '%s'", name);
         return result;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Added stage '%s' to pipeline", name);
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Execute pipeline command handler
  */
 static NexusResult pipeline_execute_handler(NexusContext* ctx, NlinkCommandParams* params) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Get command data */
     PipelineCommandData* data = (PipelineCommandData*)pipeline_command.data;
     if (!data || !data->pipeline) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "No active pipeline. Create one first with 'pipeline create'");
         return NEXUS_NOT_INITIALIZED;
     }
     
     /* Create input and output buffers */
     void* input = malloc(1024);   /* Simplified, would use appropriate size */
     void* output = malloc(1024);  /* Simplified, would use appropriate size */
     
     if (!input || !output) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate I/O buffers");
         free(input);
         free(output);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* In a real implementation, we would fill the input buffer with actual data */
     /* For this example, we'll use dummy data */
     memset(input, 0xAA, 1024);
     
     /* Run optimizations if enabled */
     if (data->enable_optimization && data->pass_manager) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Running pipeline optimizations...");
         NexusResult result = nlink_pass_manager_run(data->pass_manager, data->pipeline);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline optimization failed");
             free(input);
             free(output);
             return result;
         }
     }
     
     /* Execute the pipeline */
     nexus_log(ctx, NEXUS_LOG_INFO, "Executing pipeline...");
     NexusResult result = nlink_pipeline_execute(data->pipeline, input, output);
     
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline execution failed");
         free(input);
         free(output);
         return result;
     }
     
     /* Get and display execution statistics */
     unsigned iterations;
     double execution_time_ms;
     nlink_pipeline_get_stats(data->pipeline, &iterations, &execution_time_ms);
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline executed successfully in %.2f ms with %u iteration(s)",
              execution_time_ms, iterations);
     
     /* In a real implementation, we would do something with the output data */
     /* For this example, we'll just print a success message */
     
     /* Clean up */
     free(input);
     free(output);
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Cleanup pipeline command data
  */
 static void pipeline_free_data(void* command_data) {
     if (!command_data) {
         return;
     }
     
     PipelineCommandData* data = (PipelineCommandData*)command_data;
     
     /* Clean up resources */
     if (data->pipeline) {
         nlink_pipeline_destroy(data->pipeline);
     }
     
     if (data->pass_manager) {
         nlink_pass_manager_destroy(data->pass_manager);
     }
     
     if (data->config_path) {
         free(data->config_path);
     }
     
     free(data);
 }
 
 /**
  * @brief Print help for the pipeline command
  */
 static void pipeline_print_help(void) {
     printf("Usage: pipeline <subcommand> [options]\n\n");
     printf("Subcommands:\n");
     printf("  create [options]    Create a new pipeline\n");
     printf("  add-stage <name>    Add a stage to the pipeline\n");
     printf("  execute             Execute the pipeline\n");
     printf("\n");
     printf("Create options:\n");
     printf("  mode=<auto|single|multi>       Set pipeline mode\n");
     printf("  config=<path>                  Path to pipeline config file\n");
     printf("  optimization=<enabled|disabled> Enable or disable optimizations\n");
     printf("\n");
     printf("Examples:\n");
     printf("  pipeline create mode=single optimization=enabled\n");
     printf("  pipeline add-stage tokenizer\n");
     printf("  pipeline add-stage parser\n");
     printf("  pipeline execute\n");
 }
 
 /**
  * @brief Initialize pipeline command structure
  */
 static void init_pipeline_command(void) {
     /* Initialize pipeline command structure */
     pipeline_command.name = "pipeline";
     pipeline_command.description = "Create and manage processing pipelines";
     pipeline_command.handler = NULL;
     pipeline_command.handler_with_params = pipeline_create_handler;  /* Default to create handler */
     pipeline_command.execute = NULL;
     pipeline_command.data = NULL;
 }
 
 /**
  * @brief Get the pipeline command
  */
 NexusCommand* nlink_get_pipeline_command(void) {
     // Initialize the command structure if needed
     if (pipeline_command.name == NULL) {
         init_pipeline_command();
     }
     return &pipeline_command;
 }