/**
 * @file pipeline_stage.h
 * @brief Pipeline stage definitions for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_PIPELINE_STAGE_H
#define NLINK_PIPELINE_STAGE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/nexus_result.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pipeline stage function type
 *
 * @param ctx Context for execution
 * @param buffer Buffer containing input/output data
 * @return NexusResult Result of stage execution
 */
typedef NexusResult (*PipelineStage)(NexusContext* ctx, NexusBuffer* buffer);

/**
 * @brief Stateful pipeline stage
 */
typedef struct {
    void* internal_state;
    NexusResult (*execute)(NexusContext* ctx, NexusBuffer* buffer, void* internal_state);
} StatefulStage;

/**
 * @brief Compose two pipeline stages
 *
 * @param a First stage
 * @param b Second stage
 * @return PipelineStage Composed stage
 */
PipelineStage compose(PipelineStage a, PipelineStage b);

/**
 * @brief Fold multiple pipeline stages into one
 *
 * @param stages Array of stages
 * @param count Number of stages
 * @return PipelineStage Combined stage
 */
PipelineStage fold(PipelineStage* stages, size_t count);

/**
 * @brief Create a stateful stage
 *
 * @param execute Execution function
 * @param state Internal state
 * @return StatefulStage New stateful stage
 */
StatefulStage create_stateful_stage(
    NexusResult (*execute)(NexusContext*, NexusBuffer*, void*),
    void* state
);

/**
 * @brief Execute a stateful stage
 *
 * @param stage Stateful stage
 * @param ctx Context for execution
 * @param buffer Buffer containing input/output data
 * @return NexusResult Result of stage execution
 */
NexusResult execute_stateful_stage(
    StatefulStage* stage,
    NexusContext* ctx,
    NexusBuffer* buffer
);

/**
 * @brief Free resources associated with a stateful stage
 *
 * @param stage Stateful stage
 * @param free_state Function to free internal state
 */
void free_stateful_stage(
    StatefulStage* stage,
    void (*free_state)(void*)
);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_PIPELINE_STAGE_H */