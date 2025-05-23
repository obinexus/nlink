/**
 * @file sps_pipeline.h
 * @brief Core pipeline management for single-pass systems
 *
 * Defines structures and functions for managing single-pass pipelines,
 * including initialization, execution, and cleanup.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_SPS_PIPELINE_H
#define NLINK_SPS_PIPELINE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include "nlink/spsystem/sps_config.h"
#include "nlink/spsystem/sps_stream.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declarations
 */
typedef struct NexusPipeline NexusPipeline;
typedef struct NexusPipelineComponent NexusPipelineComponent;

/**
 * @brief Component processing function
 */
typedef NexusResult (*NexusProcessFunc)(NexusPipelineComponent* component,
                                       NexusDataStream* input,
                                       NexusDataStream* output);

/**
 * @brief Pipeline error handler
 */
typedef void (*NexusPipelineErrorHandler)(NexusPipeline* pipeline,
                                         NexusResult result,
                                         const char* component_id,
                                         const char* message);

/**
 * @brief Pipeline component structure
 */
struct NexusPipelineComponent {
    const char* component_id;            /**< Component identifier */
    NexusComponent* component;           /**< Loaded component */
    NexusProcessFunc process_func;       /**< Processing function */
    void* component_state;               /**< Component-specific state */
    NexusResult last_result;             /**< Last execution result */
    bool is_initialized;                 /**< Whether component is initialized */
    double last_execution_time_ms;       /**< Last execution time in milliseconds */
};

/**
 * @brief Pipeline structure
 */
struct NexusPipeline {
    const char* pipeline_id;             /**< Pipeline identifier */
    NexusPipelineConfig* config;         /**< Pipeline configuration */
    NexusPipelineComponent** components; /**< Array of pipeline components */
    size_t component_count;              /**< Number of components */
    bool is_initialized;                 /**< Whether pipeline is initialized */
    NexusPipelineErrorHandler error_handler; /**< Error handler function */
    void* user_data;                     /**< User-defined data */
};

/**
 * @brief Create a new pipeline from configuration
 *
 * @param ctx NexusLink context
 * @param config Pipeline configuration
 * @return NexusPipeline* New pipeline or NULL on failure
 */
NexusPipeline* sps_pipeline_create(NexusContext* ctx, NexusPipelineConfig* config);

/**
 * @brief Initialize all components in the pipeline
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to initialize
 * @return NexusResult Operation result
 */
NexusResult sps_pipeline_initialize(NexusContext* ctx, NexusPipeline* pipeline);

/**
 * @brief Execute the pipeline with input data
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to execute
 * @param input Input data stream
 * @param output Output data stream
 * @return NexusResult Operation result
 */
NexusResult sps_pipeline_execute(NexusContext* ctx, 
                                NexusPipeline* pipeline, 
                                NexusDataStream* input, 
                                NexusDataStream* output);

/**
 * @brief Clean up pipeline resources
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to destroy
 */
void sps_pipeline_destroy(NexusContext* ctx, NexusPipeline* pipeline);

/**
 * @brief Get a component from the pipeline by ID
 *
 * @param pipeline Pipeline to search
 * @param component_id Component ID to find
 * @return NexusPipelineComponent* Found component or NULL if not found
 */
NexusPipelineComponent* sps_pipeline_get_component(NexusPipeline* pipeline, const char* component_id);

/**
 * @brief Add a component to the pipeline dynamically
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param component_id Component ID to add
 * @param before_component Component ID to insert before (NULL for end)
 * @return NexusResult Operation result
 */
NexusResult sps_pipeline_add_component(NexusContext* ctx, 
                                      NexusPipeline* pipeline, 
                                      const char* component_id,
                                      const char* before_component);

/**
 * @brief Remove a component from the pipeline dynamically
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to modify
 * @param component_id Component ID to remove
 * @return NexusResult Operation result
 */
NexusResult sps_pipeline_remove_component(NexusContext* ctx, 
                                         NexusPipeline* pipeline, 
                                         const char* component_id);

/**
 * @brief Set pipeline-level error handler
 *
 * @param pipeline Pipeline to modify
 * @param handler Error handler function
 */
void sps_pipeline_set_error_handler(NexusPipeline* pipeline, NexusPipelineErrorHandler handler);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPS_PIPELINE_H */
