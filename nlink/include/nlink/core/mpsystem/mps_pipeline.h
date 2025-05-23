/**
 * @file mps_pipeline.h
 * @brief Pipeline with feedback loops for multi-pass systems
 *
 * Defines structures and functions for managing multi-pass pipelines,
 * including initialization, execution, and cleanup with feedback loops.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_MPS_PIPELINE_H
#define NLINK_MPS_PIPELINE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include "nlink/mpsystem/mps_config.h"
#include "nlink/mpsystem/mps_dependency.h"
#include "nlink/mpsystem/mps_stream.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declarations
 */
typedef struct NexusMPSPipeline NexusMPSPipeline;
typedef struct NexusMPSPipelineComponent NexusMPSPipelineComponent;

/**
 * @brief Component processing function
 */
typedef NexusResult (*NexusMPSProcessFunc)(NexusMPSPipelineComponent* component,
                                          NexusMPSDataStream* input,
                                          NexusMPSDataStream* output);

/**
 * @brief Pipeline error handler
 */
typedef void (*NexusMPSPipelineErrorHandler)(NexusMPSPipeline* pipeline,
                                            NexusResult result,
                                            const char* component_id,
                                            const char* message);

/**
 * @brief Pipeline statistics
 */
typedef struct NexusMPSPipelineStats {
    int total_iterations;           /**< Total iterations executed */
    int total_component_executions; /**< Total number of component executions */
    double total_execution_time_ms; /**< Total execution time in milliseconds */
    double avg_iteration_time_ms;   /**< Average iteration time in milliseconds */
    int max_group_size;             /**< Maximum execution group size */
    int component_count;            /**< Number of components */
    int cycle_count;                /**< Number of cycles */
} NexusMPSPipelineStats;

/**
 * @brief Multi-pass pipeline component structure
 */
struct NexusMPSPipelineComponent {
    const char* component_id;       /**< Component identifier */
    NexusComponent* component;      /**< Loaded component */
    NexusMPSProcessFunc process_func; /**< Processing function */
    void* component_state;          /**< Component-specific state */
    NexusResult last_result;        /**< Last execution result */
    bool is_initialized;            /**< Whether component is initialized */
    int execution_count;            /**< Number of times executed */
    double last_execution_time_ms;  /**< Last execution time in milliseconds */
    bool supports_reentrance;       /**< Whether component supports multiple passes */
};

/**
 * @brief Multi-pass pipeline structure
 */
struct NexusMPSPipeline {
    const char* pipeline_id;        /**< Pipeline identifier */
    NexusMPSConfig* config;         /**< Pipeline configuration */
    NexusMPSPipelineComponent** components; /**< Array of pipeline components */
    size_t component_count;         /**< Number of components */
    NexusExecutionGroup** groups;   /**< Execution groups */
    size_t group_count;             /**< Number of execution groups */
    NexusMPSDependencyGraph* graph; /**< Dependency graph */
    bool is_initialized;            /**< Whether pipeline is initialized */
    NexusMPSPipelineErrorHandler error_handler; /**< Error handler function */
    void* user_data;                /**< User-defined data */
    int current_iteration;          /**< Current iteration counter */
    int max_iterations;             /**< Maximum iterations (0 = unlimited) */
    NexusMPSPipelineStats stats;    /**< Execution statistics */
};

/**
 * @brief Create a new multi-pass pipeline from configuration
 *
 * @param ctx NexusLink context
 * @param config Pipeline configuration
 * @return NexusMPSPipeline* New pipeline or NULL on failure
 */
NexusMPSPipeline* mps_pipeline_create(NexusContext* ctx, NexusMPSConfig* config);

/**
 * @brief Initialize all components in the multi-pass pipeline
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to initialize
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_initialize(NexusContext* ctx, NexusMPSPipeline* pipeline);

/**
 * @brief Execute the multi-pass pipeline with input data
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to execute
 * @param input Input data stream
 * @param output Output data stream
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_execute(NexusContext* ctx, 
                               NexusMPSPipeline* pipeline, 
                               NexusMPSDataStream* input, 
                               NexusMPSDataStream* output);

/**
 * @brief Clean up multi-pass pipeline resources
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to destroy
 */
void mps_pipeline_destroy(NexusContext* ctx, NexusMPSPipeline* pipeline);

/**
 * @brief Execute a specific component group in the pipeline
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline containing the group
 * @param group Execution group to execute
 * @param streams Data stream map for component I/O
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_execute_group(NexusContext* ctx,
                                      NexusMPSPipeline* pipeline,
                                      NexusExecutionGroup* group,
                                      NexusMPSDataStreamMap* streams);

/**
 * @brief Get a component from the pipeline by ID
 *
 * @param pipeline Pipeline to search
 * @param component_id Component ID to find
 * @return NexusMPSPipelineComponent* Found component or NULL if not found
 */
NexusMPSPipelineComponent* mps_pipeline_get_component(NexusMPSPipeline* pipeline, const char* component_id);

/**
 * @brief Add a component to the pipeline dynamically
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param component_id Component ID to add
 * @param version_constraint Version constraint for the component
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_add_component(NexusContext* ctx, 
                                      NexusMPSPipeline* pipeline, 
                                      const char* component_id,
                                      const char* version_constraint);

/**
 * @brief Remove a component from the pipeline dynamically
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param component_id Component ID to remove
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_remove_component(NexusContext* ctx, 
                                         NexusMPSPipeline* pipeline, 
                                         const char* component_id);

/**
 * @brief Add a connection between components
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param source_id Source component ID
 * @param target_id Target component ID
 * @param direction Connection direction
 * @param data_format Data format for the connection
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_add_connection(NexusContext* ctx,
                                       NexusMPSPipeline* pipeline,
                                       const char* source_id,
                                       const char* target_id,
                                       NexusConnectionDirection direction,
                                       const char* data_format);

/**
 * @brief Remove a connection between components
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param source_id Source component ID
 * @param target_id Target component ID
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_remove_connection(NexusContext* ctx,
                                          NexusMPSPipeline* pipeline,
                                          const char* source_id,
                                          const char* target_id);

/**
 * @brief Set pipeline-level error handler
 *
 * @param pipeline Pipeline to modify
 * @param handler Error handler function
 */
void mps_pipeline_set_error_handler(NexusMPSPipeline* pipeline, NexusMPSPipelineErrorHandler handler);

/**
 * @brief Set pipeline iteration limit
 *
 * @param pipeline Pipeline to modify
 * @param max_iterations Maximum number of iterations (0 = unlimited)
 */
void mps_pipeline_set_iteration_limit(NexusMPSPipeline* pipeline, int max_iterations);

/**
 * @brief Get pipeline execution statistics
 *
 * @param pipeline Pipeline to get statistics from
 * @param stats Output parameter for statistics
 */
void mps_pipeline_get_stats(NexusMPSPipeline* pipeline, NexusMPSPipelineStats* stats);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPS_PIPELINE_H */
