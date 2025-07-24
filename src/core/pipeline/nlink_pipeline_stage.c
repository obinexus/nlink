/**
 * @file pipeline_stage.c
 * @brief Implementation of pipeline stages
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <nlink/core/pipeline/pipeline_stage.h"
#include <nlink/core/common/nexus_error.h"
#include <stdlib.h>

// Context for composed stages
typedef struct {
    PipelineStage first;
    PipelineStage second;
} ComposedStageContext;

// Implementation of composed stage execution
static NexusResult composed_stage_execute(NexusContext* ctx, NexusBuffer* buffer, void* state) {
    ComposedStageContext* composed = (ComposedStageContext*)state;
    
    // Execute first stage
    NexusResult result = composed->first(ctx, buffer);
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Execute second stage
    return composed->second(ctx, buffer);
}

// Wrapper for stateful stage execution
static NexusResult stateful_stage_wrapper(NexusContext* ctx, NexusBuffer* buffer, void* state) {
    StatefulStage* stage = (StatefulStage*)state;
    return stage->execute(ctx, buffer, stage->internal_state);
}

// Context for folded stages
typedef struct {
    PipelineStage* stages;
    size_t count;
} FoldedStageContext;

// Implementation of folded stage execution
static NexusResult folded_stage_execute(NexusContext* ctx, NexusBuffer* buffer, void* state) {
    FoldedStageContext* folded = (FoldedStageContext*)state;
    
    for (size_t i = 0; i < folded->count; i++) {
        NexusResult result = folded->stages[i](ctx, buffer);
        if (result.status != NEXUS_STATUS_SUCCESS) {
            return result;
        }
    }
    
    return nexus_success(NULL, NULL);
}

// Static storage for composed stages (this is a simplification)
static ComposedStageContext composed_contexts[32];
static size_t composed_context_count = 0;

// Static storage for folded stages (this is a simplification)
static FoldedStageContext folded_contexts[16];
static size_t folded_context_count = 0;

PipelineStage compose(PipelineStage a, PipelineStage b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }
    
    // In a real implementation, this would use dynamic allocation
    if (composed_context_count >= 32) {
        return NULL;  // No more space
    }
    
    ComposedStageContext* context = &composed_contexts[composed_context_count++];
    context->first = a;
    context->second = b;
    
    // Create a static function that captures the context
    // In a real implementation, this would be dynamically created
    return (PipelineStage)composed_stage_execute;
}

PipelineStage fold(PipelineStage* stages, size_t count) {
    if (stages == NULL || count == 0) {
        return NULL;
    }
    
    // Special case for single stage
    if (count == 1) {
        return stages[0];
    }
    
    // In a real implementation, this would use dynamic allocation
    if (folded_context_count >= 16) {
        return NULL;  // No more space
    }
    
    FoldedStageContext* context = &folded_contexts[folded_context_count++];
    context->stages = malloc(count * sizeof(PipelineStage));
    if (context->stages == NULL) {
        return NULL;
    }
    
    // Copy the stages
    for (size_t i = 0; i < count; i++) {
        context->stages[i] = stages[i];
    }
    context->count = count;
    
    // Create a static function that captures the context
    // In a real implementation, this would be dynamically created
    return (PipelineStage)folded_stage_execute;
}

StatefulStage create_stateful_stage(
    NexusResult (*execute)(NexusContext*, NexusBuffer*, void*),
    void* state
) {
    StatefulStage stage;
    stage.execute = execute;
    stage.internal_state = state;
    return stage;
}

NexusResult execute_stateful_stage(
    StatefulStage* stage,
    NexusContext* ctx,
    NexusBuffer* buffer
) {
    if (stage == NULL || stage->execute == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Invalid stateful stage",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    return stage->execute(ctx, buffer, stage->internal_state);
}

void free_stateful_stage(
    StatefulStage* stage,
    void (*free_state)(void*)
) {
    if (stage == NULL) {
        return;
    }
    
    if (free_state != NULL && stage->internal_state != NULL) {
        free_state(stage->internal_state);
    }
    
    stage->internal_state = NULL;
    stage->execute = NULL;
}