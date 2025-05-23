/**
 * @file mps_lifecycle.h
 * @brief Complex lifecycle with state preservation for multi-pass systems
 *
 * Defines structures and functions for managing the lifecycle of components
 * in a multi-pass pipeline system, with state preservation between iterations.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_MPS_LIFECYCLE_H
#define NLINK_MPS_LIFECYCLE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
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
 * @brief Component initialization function
 */
typedef NexusResult (*NexusMPSComponentInitFunc)(NexusMPSPipelineComponent* component, void* user_data);

/**
 * @brief Component execution function
 */
typedef NexusResult (*NexusMPSComponentExecFunc)(NexusMPSPipelineComponent* component, 
                                                NexusMPSDataStream* input, 
                                                NexusMPSDataStream* output, 
                                                int iteration, 
                                                void* user_data);

/**
 * @brief Component iteration end function
 */
typedef NexusResult (*NexusMPSComponentIterEndFunc)(NexusMPSPipelineComponent* component, 
                                                   int iteration, 
                                                   void* user_data);

/**
 * @brief Component termination function
 */
typedef NexusResult (*NexusMPSComponentTermFunc)(NexusMPSPipelineComponent* component, void* user_data);

/**
 * @brief Component state save function
 */
typedef NexusResult (*NexusMPSComponentSaveStateFunc)(NexusMPSPipelineComponent* component, 
                                                     const char* state_path, 
                                                     void* user_data);

/**
 * @brief Component state load function
 */
typedef NexusResult (*NexusMPSComponentLoadStateFunc)(NexusMPSPipelineComponent* component, 
                                                     const char* state_path, 
                                                     void* user_data);

/**
 * @brief Component lifecycle hooks
 */
typedef struct NexusMPSComponentLifecycle {
    NexusMPSComponentInitFunc init_func;          /**< Initialization hook */
    NexusMPSComponentExecFunc exec_func;          /**< Execution hook */
    NexusMPSComponentIterEndFunc iter_end_func;   /**< Iteration end hook */
    NexusMPSComponentTermFunc term_func;          /**< Termination hook */
    NexusMPSComponentTermFunc abort_func;         /**< Abort hook */
    NexusMPSComponentSaveStateFunc save_state_func; /**< State save hook */
    NexusMPSComponentLoadStateFunc load_state_func; /**< State load hook */
    void* user_data;                              /**< User-defined data */
} NexusMPSComponentLifecycle;

/**
 * @brief Register lifecycle hooks for a component
 *
 * @param ctx NexusLink context
 * @param component Component to register hooks for
 * @param lifecycle Lifecycle hooks to register
 * @return NexusResult Operation result
 */
NexusResult mps_register_component_lifecycle(NexusContext* ctx, 
                                           NexusMPSPipelineComponent* component,
                                           NexusMPSComponentLifecycle* lifecycle);

/**
 * @brief Call initialization hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to initialize
 * @return NexusResult Operation result
 */
NexusResult mps_component_initialize(NexusContext* ctx, NexusMPSPipelineComponent* component);

/**
 * @brief Call execution hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to execute
 * @param input Input data stream
 * @param output Output data stream
 * @param iteration Current iteration number
 * @return NexusResult Operation result
 */
NexusResult mps_component_execute(NexusContext* ctx, 
                                 NexusMPSPipelineComponent* component,
                                 NexusMPSDataStream* input,
                                 NexusMPSDataStream* output,
                                 int iteration);

/**
 * @brief Call iteration end hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to notify
 * @param iteration Iteration that just completed
 * @return NexusResult Operation result
 */
NexusResult mps_component_end_iteration(NexusContext* ctx, 
                                       NexusMPSPipelineComponent* component,
                                       int iteration);

/**
 * @brief Call termination hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to terminate
 * @return NexusResult Operation result
 */
NexusResult mps_component_terminate(NexusContext* ctx, NexusMPSPipelineComponent* component);

/**
 * @brief Call abort hook for a component
 *
 * @param ctx NexusLink context
 * @param component Component to abort
 * @return NexusResult Operation result
 */
NexusResult mps_component_abort(NexusContext* ctx, NexusMPSPipelineComponent* component);

/**
 * @brief Handle error during pipeline execution
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline where the error occurred
 * @param error Error result
 * @param component_id ID of the component that caused the error
 * @param iteration Current iteration number
 * @return NexusResult Operation result
 */
NexusResult mps_handle_pipeline_error(NexusContext* ctx, 
                                     NexusMPSPipeline* pipeline,
                                     NexusResult error,
                                     const char* component_id,
                                     int iteration);

/**
 * @brief Save component state for resuming later
 *
 * @param ctx NexusLink context
 * @param component Component to save state for
 * @param state_path Path to save state to
 * @return NexusResult Operation result
 */
NexusResult mps_component_save_state(NexusContext* ctx,
                                    NexusMPSPipelineComponent* component,
                                    const char* state_path);

/**
 * @brief Load component state for resuming
 *
 * @param ctx NexusLink context
 * @param component Component to load state for
 * @param state_path Path to load state from
 * @return NexusResult Operation result
 */
NexusResult mps_component_load_state(NexusContext* ctx,
                                    NexusMPSPipelineComponent* component,
                                    const char* state_path);

/**
 * @brief Create a pipeline checkpoint (all component states)
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to checkpoint
 * @param checkpoint_dir Directory to save checkpoint to
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_create_checkpoint(NexusContext* ctx,
                                          NexusMPSPipeline* pipeline,
                                          const char* checkpoint_dir);

/**
 * @brief Restore a pipeline from a checkpoint
 *
 * @param ctx NexusLink context
 * @param pipeline Pipeline to restore
 * @param checkpoint_dir Directory to load checkpoint from
 * @return NexusResult Operation result
 */
NexusResult mps_pipeline_restore_checkpoint(NexusContext* ctx,
                                           NexusMPSPipeline* pipeline,
                                           const char* checkpoint_dir);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPS_LIFECYCLE_H */

