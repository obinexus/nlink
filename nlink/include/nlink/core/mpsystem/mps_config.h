/**
 * @file mps_config.h
 * @brief Pipeline configuration for multi-pass systems
 *
 * Defines structures and functions for managing configurations
 * of multi-pass pipeline systems with bidirectional data flows.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_MPS_CONFIG_H
#define NLINK_MPS_CONFIG_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Connection direction
 */
typedef enum NexusConnectionDirection {
    NEXUS_DIRECTION_FORWARD,        /**< Forward connection (default) */
    NEXUS_DIRECTION_BACKWARD,       /**< Backward connection */
    NEXUS_DIRECTION_BIDIRECTIONAL   /**< Bidirectional connection */
} NexusConnectionDirection;

/**
 * @brief Connection between components
 */
typedef struct NexusComponentConnection {
    const char* source_id;          /**< Source component ID */
    const char* target_id;          /**< Target component ID */
    NexusConnectionDirection direction; /**< Connection direction */
    const char* data_format;        /**< Data format for this connection */
    bool optional;                  /**< Whether this connection is optional */
} NexusComponentConnection;

/**
 * @brief Multi-pass pipeline component configuration
 */
typedef struct NexusMPSComponentConfig {
    const char* component_id;       /**< Component identifier */
    const char* version_constraint; /**< Version constraint */
    void* component_config;         /**< Component-specific configuration */
    bool optional;                  /**< Whether this component is optional */
    bool supports_reentrance;       /**< Whether component supports multiple passes */
    int max_passes;                 /**< Maximum number of passes (0 = unlimited) */
} NexusMPSComponentConfig;

/**
 * @brief Cycle information for validation
 */
typedef struct NexusCycleInfo {
    const char** component_ids;     /**< Array of component IDs in the cycle */
    size_t component_count;         /**< Number of components in the cycle */
} NexusCycleInfo;

/**
 * @brief Multi-pass pipeline configuration structure
 */
typedef struct NexusMPSConfig {
    const char* pipeline_id;        /**< Pipeline identifier */
    const char* description;        /**< Pipeline description */
    NexusMPSComponentConfig** components; /**< Array of component configurations */
    size_t component_count;         /**< Number of components */
    NexusComponentConnection** connections; /**< Array of component connections */
    size_t connection_count;        /**< Number of connections */
    bool allow_cycles;              /**< Whether cycles are allowed in the pipeline */
    int max_iteration_count;        /**< Maximum iteration count (0 = unlimited) */
    bool allow_partial_processing;  /**< Allow partial pipeline execution */
    void* (*component_config_creator)(const char* json);   /**< Function to create component config from JSON */
    void (*component_config_destructor)(void* config);     /**< Function to destroy component config */
} NexusMPSConfig;

/**
 * @brief Parse a multi-pass pipeline configuration from a JSON file
 *
 * @param ctx NexusLink context
 * @param config_path Path to the configuration file
 * @return NexusMPSConfig* Parsed configuration or NULL on failure
 */
NexusMPSConfig* mps_parse_pipeline_config(NexusContext* ctx, const char* config_path);

/**
 * @brief Validate a multi-pass pipeline configuration, including cycle detection
 *
 * @param ctx NexusLink context
 * @param config Configuration to validate
 * @return NexusResult Validation result
 */
NexusResult mps_validate_pipeline_config(NexusContext* ctx, NexusMPSConfig* config);

/**
 * @brief Check for cycles in component dependencies
 *
 * @param ctx NexusLink context
 * @param config Configuration to check
 * @param cycles Output parameter for cycle information
 * @param cycle_count Output parameter for number of cycles found
 * @return bool True if cycles were found, false otherwise
 */
bool mps_detect_cycles(NexusContext* ctx, NexusMPSConfig* config, NexusCycleInfo** cycles, size_t* cycle_count);

/**
 * @brief Create a default multi-pass pipeline configuration
 *
 * @return NexusMPSConfig* New default configuration
 */
NexusMPSConfig* mps_create_default_pipeline_config(void);

/**
 * @brief Free multi-pass pipeline configuration resources
 *
 * @param config Configuration to free
 */
void mps_free_pipeline_config(NexusMPSConfig* config);

/**
 * @brief Save a multi-pass pipeline configuration to a JSON file
 *
 * @param ctx NexusLink context
 * @param config Configuration to save
 * @param path File path to save to
 * @return NexusResult Operation result
 */
NexusResult mps_save_pipeline_config(NexusContext* ctx, const NexusMPSConfig* config, const char* path);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPS_CONFIG_H */
