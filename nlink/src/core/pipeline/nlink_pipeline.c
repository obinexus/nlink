/**
 * @file nlink_pipeline.c
 * @brief Implementation of the unified pipeline interface
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/pipeline/nlink_pipeline.h"
#include "nlink/core/common/nexus_core.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

NlinkPipelineConfig nlink_pipeline_default_config(void) {
    NlinkPipelineConfig config;
    config.mode = NLINK_PIPELINE_MODE_AUTO;
    config.enable_optimization = true;
    config.enable_caching = true;
    config.max_iterations = 10;  /* Default to 10 iterations max */
    config.schema_path = NULL;
    return config;
}

NlinkPipeline* nlink_pipeline_create(NexusContext* ctx, const NlinkPipelineConfig* config) {
    if (!ctx) {
        return NULL;
    }
    
    NlinkPipeline* pipeline = (NlinkPipeline*)malloc(sizeof(NlinkPipeline));
    if (!pipeline) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline");
        return NULL;
    }
    
    /* Initialize with default values */
    memset(pipeline, 0, sizeof(NlinkPipeline));
    pipeline->ctx = ctx;
    
    /* Apply configuration if provided */
    if (config) {
        pipeline->config = *config;
    } else {
        pipeline->config = nlink_pipeline_default_config();
    }
    
    /* Start with auto mode, will be determined during execution */
    pipeline->active_mode = NLINK_PIPELINE_MODE_AUTO;
    
    nexus_log(ctx, NEXUS_LOG_INFO, "Created pipeline in %s mode", 
              pipeline->config.mode == NLINK_PIPELINE_MODE_SINGLE_PASS ? "single-pass" :
              pipeline->config.mode == NLINK_PIPELINE_MODE_MULTI_PASS ? "multi-pass" : "auto");
    
    return pipeline;
}

NexusResult nlink_pipeline_add_stage(NlinkPipeline* pipeline, 
                                    const char* name,
                                    NlinkPipelineStageFunc func,
                                    void* user_data) {
    if (!pipeline || !name || !func) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    /* Create new stage */
    NlinkPipelineStage* stage = (NlinkPipelineStage*)malloc(sizeof(NlinkPipelineStage));
    if (!stage) {
        nexus_log(pipeline->ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline stage");
        return NEXUS_OUT_OF_MEMORY;
    }
    
    /* Initialize stage */
    stage->name = strdup(name);
    if (!stage->name) {
        free(stage);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    stage->func = func;
    stage->user_data = user_data;
    stage->next = NULL;
    
    /* Add to pipeline */
    if (!pipeline->first_stage) {
        pipeline->first_stage = stage;
    } else {
        pipeline->last_stage->next = stage;
    }
    
    pipeline->last_stage = stage;
    pipeline->stage_count++;
    
    /* Pipeline is no longer optimized after adding a stage */
    pipeline->is_optimized = false;
    
    nexus_log(pipeline->ctx, NEXUS_LOG_DEBUG, "Added stage '%s' to pipeline", name);
    return NEXUS_SUCCESS;
}

static NexusResult execute_single_pass(NlinkPipeline* pipeline, void* input, void* output) {
    NexusContext* ctx = pipeline->ctx;
    NlinkPipelineStage* current = pipeline->first_stage;
    void* stage_input = input;
    void* stage_output = NULL;
    NexusResult result = NEXUS_SUCCESS;
    
    /* Only need a middle buffer if we have more than one stage */
    if (pipeline->stage_count > 1) {
        stage_output = malloc(1024);  /* Simplified: would use appropriate buffer size in real impl */
        if (!stage_output) {
            nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate stage output buffer");
            return NEXUS_OUT_OF_MEMORY;
        }
    } else {
        stage_output = output;  /* Single stage, output directly to final output */
    }
    
    /* Process each stage */
    while (current && result == NEXUS_SUCCESS) {
        nexus_log(ctx, NEXUS_LOG_DEBUG, "Executing stage '%s'", current->name);
        
        /* Last stage outputs directly to the final output */
        void* current_output = current->next ? stage_output : output;
        
        /* Execute the stage */
        result = current->func(stage_input, current_output, current->user_data);
        
        if (result != NEXUS_SUCCESS) {
            nexus_log(ctx, NEXUS_LOG_ERROR, "Stage '%s' failed with result %d", 
                      current->name, result);
            break;
        }
        
        /* Prepare for next stage */
        if (current->next) {
            stage_input = stage_output;
        }
        
        current = current->next;
    }
    
    /* Clean up intermediate buffer */
    if (pipeline->stage_count > 1 && stage_output != output) {
        free(stage_output);
    }
    
    return result;
}

static NexusResult execute_multi_pass(NlinkPipeline* pipeline, void* input, void* output) {
    NexusContext* ctx = pipeline->ctx;
    NexusResult result = NEXUS_SUCCESS;
    unsigned iterations = 0;
    bool converged = false;
    
    /* Create two buffers for ping-pong processing */
    void* buffer_a = malloc(1024);  /* Simplified: would use appropriate buffer size in real impl */
    void* buffer_b = malloc(1024);
    
    if (!buffer_a || !buffer_b) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate processing buffers");
        free(buffer_a);
        free(buffer_b);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    /* Initialize buffer_a with input data */
    memcpy(buffer_a, input, 1024);  /* Simplified; would use actual size */
    
    /* Process until convergence or max iterations */
    while (!converged && iterations < pipeline->config.max_iterations) {
        iterations++;
        void* source = (iterations % 2 == 1) ? buffer_a : buffer_b;
        void* target = (iterations % 2 == 1) ? buffer_b : buffer_a;
        
        /* Process each stage */
        NlinkPipelineStage* current = pipeline->first_stage;
        while (current && result == NEXUS_SUCCESS) {
            nexus_log(ctx, NEXUS_LOG_DEBUG, "Iteration %u: Executing stage '%s'", 
                      iterations, current->name);
            
            result = current->func(source, target, current->user_data);
            
            if (result != NEXUS_SUCCESS) {
                nexus_log(ctx, NEXUS_LOG_ERROR, "Stage '%s' failed with result %d in iteration %u", 
                          current->name, result, iterations);
                break;
            }
            
            current = current->next;
        }
        
        /* Check for convergence (simplified: would use actual comparison) */
        if (iterations > 1) {
            /* Compare source and target to detect convergence */
            if (memcmp(source, target, 1024) == 0) {
                converged = true;
                nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline converged after %u iterations", iterations);
            }
        }
    }
    
    /* Copy final result to output */
    if (result == NEXUS_SUCCESS) {
        void* final_buffer = (iterations % 2 == 0) ? buffer_a : buffer_b;
        memcpy(output, final_buffer, 1024);  /* Simplified; would use actual size */
        
        if (!converged) {
            nexus_log(ctx, NEXUS_LOG_WARNING, "Pipeline reached maximum iterations (%u) without converging",
                      pipeline->config.max_iterations);
        }
    }
    
    /* Store iteration count for statistics */
    pipeline->last_iterations = iterations;
    
    /* Clean up */
    free(buffer_a);
    free(buffer_b);
    
    return result;
}

NexusResult nlink_pipeline_execute(NlinkPipeline* pipeline, void* input, void* output) {
    if (!pipeline || !input || !output) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    NexusContext* ctx = pipeline->ctx;
    NexusResult result;
    struct timespec start, end;
    
    /* Check if we have any stages */
    if (!pipeline->first_stage) {
        nexus_log(ctx, NEXUS_LOG_WARNING, "Executing empty pipeline");
        return NEXUS_SUCCESS;  /* Nothing to do */
    }
    
    /* Determine execution mode if set to auto */
    if (pipeline->config.mode == NLINK_PIPELINE_MODE_AUTO) {
        /* Simplified logic: use multi-pass if we have more than 3 stages */
        pipeline->active_mode = (pipeline->stage_count > 3) ? 
                               NLINK_PIPELINE_MODE_MULTI_PASS : 
                               NLINK_PIPELINE_MODE_SINGLE_PASS;
    } else {
        pipeline->active_mode = pipeline->config.mode;
    }
    
    /* Start timing */
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /* Execute the pipeline based on the active mode */
    if (pipeline->active_mode == NLINK_PIPELINE_MODE_SINGLE_PASS) {
        nexus_log(ctx, NEXUS_LOG_INFO, "Executing pipeline in single-pass mode");
        result = execute_single_pass(pipeline, input, output);
        pipeline->last_iterations = 1;  /* Always one iteration in single-pass mode */
    } else {
        nexus_log(ctx, NEXUS_LOG_INFO, "Executing pipeline in multi-pass mode");
        result = execute_multi_pass(pipeline, input, output);
    }
    
    /* End timing */
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    /* Calculate execution time */
    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                    (end.tv_nsec - start.tv_nsec) / 1000000.0;
    pipeline->last_execution_time_ms = time_ms;
    
    nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline execution completed in %.2f ms with %u iteration(s)",
              time_ms, pipeline->last_iterations);
    
    return result;
}

NlinkPipelineMode nlink_pipeline_get_mode(const NlinkPipeline* pipeline) {
    if (!pipeline) {
        return NLINK_PIPELINE_MODE_AUTO;  /* Default */
    }
    
    return pipeline->active_mode;
}

NexusResult nlink_pipeline_get_stats(const NlinkPipeline* pipeline, 
                                   unsigned* iterations,
                                   double* execution_time_ms) {
    if (!pipeline) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    if (iterations) {
        *iterations = pipeline->last_iterations;
    }
    
    if (execution_time_ms) {
        *execution_time_ms = pipeline->last_execution_time_ms;
    }
    
    return NEXUS_SUCCESS;
}

void nlink_pipeline_destroy(NlinkPipeline* pipeline) {
    if (!pipeline) {
        return;
    }
    
    /* Free all stages */
    NlinkPipelineStage* current = pipeline->first_stage;
    while (current) {
        NlinkPipelineStage* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    
    /* Free the pipeline itself */
    free(pipeline);
}