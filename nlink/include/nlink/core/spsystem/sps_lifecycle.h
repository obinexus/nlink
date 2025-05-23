/**
 * @file sps_lifecycle.h
 * @brief Lifecycle hooks and management for single-pass systems
 *
 * Defines structures and functions for managing the lifecycle of components
 * in a single-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_SPS_LIFECYCLE_H
#define NLINK_SPS_LIFECYCLE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
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
 * @brief Component initialization function
 */
typedef NexusResult (*NexusComponentInitFunc)(NexusPipelineComponent* component, void* user_data);

/**
 * @brief Component termination function
 */
typedef NexusResult (*NexusComponentTermFunc)(NexusPipelineComponent* component, void* user_data);

/**
 * @brief Component lifecycle hooks
 */
typedef struct NexusComponentLifecycle {
    NexusComponentInitFunc init_func;     /**< Initialization hook */
    NexusComponentTermFunc term_func;     /**< Termination hook */
    NexusComponentTermFunc abort_func;    /**< Abort hook */
    void* user_data;                      /**< User-defined data */
} NexusComponentLifecycle;

/**
 * @brief Register lifecycle hooks for a component
 *
 * @param ctx NexusLink context
 * @param component Component to register hooks for
 * @param lifecycle Lifecycle hooks to register
 * @return NexusResult Operation result
 */
NexusResult sps_register_component_lifecycle(NexusContext* ctx, 
                                           NexusPipelineComponent* component,
                                           NexusComponentLifecycle* lifecycle);

/**
 * @brief Call initialization hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to initialize
 * @return NexusResult Operation result
 */
NexusResult sps_component_initialize(NexusContext* ctx, NexusPipelineComponent* component);

/**
 * @brief Call execution hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to execute
 * @param input Input data stream
 * @param output Output data stream
 * @return NexusResult Operation result
 */
NexusResult sps_component_execute(NexusContext* ctx, 
                                 NexusPipelineComponent* component,
                                 NexusDataStream* input,
                                 NexusDataStream* output);

/**
 * @brief Call termination hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to terminate
 * @return NexusResult Operation result
 */
NexusResult sps_component_terminate(NexusContext* ctx, NexusPipelineComponent* component);

/**
 * @brief Call abort hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to abort
 * @return NexusResult Operation result
 */
NexusResult sps_component_abort(NexusContext* ctx, NexusPipelineComponent* component);

/**
 * @brief Handle error during pipeline execution
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline where the error occurred
 * @param error Error result
 * @param component_id ID of the component that caused the error
 * @return NexusResult Operation result
 */
NexusResult sps_handle_pipeline_error(NexusContext* ctx, 
                                     NexusPipeline* pipeline,
                                     NexusResult error,
                                     const char* component_id);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPS_LIFECYCLE_H */
