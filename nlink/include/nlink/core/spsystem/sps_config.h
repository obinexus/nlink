/**
 * @file sps_config.h
 * @brief Pipeline configuration for single-pass systems
 *
 * Defines structures and functions for managing pipeline configurations
 * in single-pass systems.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_SPS_CONFIG_H
#define NLINK_SPS_CONFIG_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pipeline component configuration
 */
typedef struct NexusPipelineComponentConfig {
    const char* component_id;          /**< Component identifier */
    const char* version_constraint;    /**< Version constraint */
    void* component_config;            /**< Component-specific configuration */
    bool optional;                     /**< Whether this component is optional */
} NexusPipelineComponentConfig;

/**
 * @brief Pipeline configuration structure
 */
typedef struct NexusPipelineConfig {
    const char* pipeline_id;                                /**< Pipeline identifier */
    const char* description;                               /**< Pipeline description */
    NexusPipelineComponentConfig** components;             /**< Array of component configurations */
    size_t component_count;                                /**< Number of components */
    const char* input_format;                              /**< Input data format */
    const char* output_format;                             /**< Output data format */
    bool allow_partial_processing;                         /**< Allow partial pipeline execution */
    void* (*component_config_creator)(const char* json);   /**< Function to create component config from JSON */
    void (*component_config_destructor)(void* config);     /**< Function to destroy component config */
} NexusPipelineConfig;

/**
 * @brief Parse a pipeline configuration from a JSON file
 *
 * @param ctx NexusLink context
 * @param config_path Path to the configuration file
 * @return NexusPipelineConfig* Parsed configuration or NULL on failure
 */
NexusPipelineConfig* sps_parse_pipeline_config(NexusContext* ctx, const char* config_path);

/**
 * @brief Create a default pipeline configuration
 *
 * @return NexusPipelineConfig* New default configuration
 */
NexusPipelineConfig* sps_create_default_pipeline_config(void);

/**
 * @brief Validate a pipeline configuration
 *
 * @param ctx NexusLink context
 * @param config Configuration to validate
 * @return NexusResult Validation result
 */
NexusResult sps_validate_pipeline_config(NexusContext* ctx, NexusPipelineConfig* config);

/**
 * @brief Free pipeline configuration resources
 *
 * @param config Configuration to free
 */
void sps_free_pipeline_config(NexusPipelineConfig* config);

/**
 * @brief Save a pipeline configuration to a JSON file
 *
 * @param ctx NexusLink context
 * @param config Configuration to save
 * @param path File path to save to
 * @return NexusResult Operation result
 */
NexusResult sps_save_pipeline_config(NexusContext* ctx, const NexusPipelineConfig* config, const char* path);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPS_CONFIG_H */
