/**
 * @file config_pipeline.h
 * @brief Configuration processing pipeline
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_CONFIG_PIPELINE_H
#define NLINK_CONFIG_PIPELINE_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/nexus_result.h"
#include "nlink/core/pipeline/pipeline_stage.h"
#include "nlink/core/token/token_value.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration tokenization stage
 *
 * @param ctx Context for execution
 * @param buffer Buffer containing input/output data
 * @return NexusResult Result of tokenization
 */
NexusResult config_tokenize_stage(NexusContext* ctx, NexusBuffer* buffer);

/**
 * @brief Configuration validation stage
 *
 * @param ctx Context for execution
 * @param buffer Buffer containing input/output data
 * @return NexusResult Result of validation
 */
NexusResult config_validate_stage(NexusContext* ctx, NexusBuffer* buffer);

/**
 * @brief Pattern resolution stage for configuration
 *
 * @param ctx Context for execution
 * @param buffer Buffer containing input/output data
 * @return NexusResult Result of pattern resolution
 */
NexusResult config_resolve_patterns_stage(NexusContext* ctx, NexusBuffer* buffer);

/**
 * @brief Create a default configuration pipeline
 *
 * @return PipelineStage Complete pipeline
 */
PipelineStage create_default_config_pipeline(void);

/**
 * @brief Process a configuration file through the pipeline
 *
 * @param filename Configuration file to process
 * @param ctx Context for execution
 * @param out_root Pointer to store the root configuration section
 * @return NexusResult Result of processing
 */
NexusResult process_config_file(
    const char* filename,
    NexusContext* ctx,
    nlink_token_config_section** out_root
);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_CONFIG_PIPELINE_H */