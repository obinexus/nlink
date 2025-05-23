/**
 * @file pipeline_detector.h
 * @brief Pipeline type detection and optimization system for NexusLink
 *
 * Provides functionality to detect pipeline types (single-pass or multi-pass)
 * and automatically apply appropriate optimizations using the Okpala minimizer.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CORE_PIPELINE_DETECTOR_H
 #define NLINK_CORE_PIPELINE_DETECTOR_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/types.h"
 #include "nlink/core/minimizer/nexus_minimizer.h"
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Pipeline type enumeration
  */
 typedef enum NexusPipelineType {
     NEXUS_PIPELINE_TYPE_UNKNOWN = 0,    /**< Unknown pipeline type */
     NEXUS_PIPELINE_TYPE_SINGLE_PASS,    /**< Single-pass pipeline */
     NEXUS_PIPELINE_TYPE_MULTI_PASS,     /**< Multi-pass pipeline */
     NEXUS_PIPELINE_TYPE_HYBRID          /**< Hybrid pipeline with both types */
 } NexusPipelineType;
 
 /**
  * @brief Pipeline detection configuration
  */
 typedef struct NexusPipelineDetectorConfig {
     bool auto_optimize;              /**< Automatically apply optimization */
     NexusMinimizerLevel min_level;   /**< Minimum minimizer level to apply */
     NexusMinimizerLevel max_level;   /**< Maximum minimizer level to apply */
     bool collect_metrics;            /**< Collect optimization metrics */
     bool verbose;                    /**< Enable verbose logging */
     const char* metrics_output_path; /**< Path to write metrics (NULL for none) */
 } NexusPipelineDetectorConfig;
 
 /**
  * @brief Pipeline detection result
  */
 typedef struct NexusPipelineDetectionResult {
     NexusPipelineType detected_type;           /**< Detected pipeline type */
     char* component_path;                      /**< Path to the component */
     size_t component_count;                    /**< Number of components in pipeline */
     bool has_cycles;                           /**< Whether pipeline has cycles */
     bool optimization_applied;                 /**< Whether optimization was applied */
     NexusMinimizationMetrics* metrics;         /**< Optimization metrics (if collected) */
     size_t metrics_count;                      /**< Number of metrics entries */
 } NexusPipelineDetectionResult;
 
 /**
  * @brief Initialize the pipeline detector
  * 
  * @param ctx NexusLink context
  * @param config Configuration for pipeline detection
  * @return NexusResult Operation result
  */
 NexusResult nexus_pipeline_detector_initialize(NexusContext* ctx, 
                                               const NexusPipelineDetectorConfig* config);
 
 /**
  * @brief Get default pipeline detector configuration
  * 
  * @return NexusPipelineDetectorConfig Default configuration
  */
 NexusPipelineDetectorConfig nexus_pipeline_detector_default_config(void);
 
 /**
  * @brief Detect pipeline type from a component
  * 
  * @param ctx NexusLink context
  * @param component_path Path to the component to analyze
  * @param result Output parameter for detection result
  * @return NexusResult Operation result
  */
 NexusResult nexus_pipeline_detect(NexusContext* ctx, 
                                  const char* component_path,
                                  NexusPipelineDetectionResult** result);
 
 /**
  * @brief Apply optimization to detected pipeline
  * 
  * @param ctx NexusLink context
  * @param result Detection result containing pipeline information
  * @return NexusResult Operation result
  */
 NexusResult nexus_pipeline_optimize(NexusContext* ctx, 
                                    NexusPipelineDetectionResult* result);
 
 /**
  * @brief Free pipeline detection result resources
  * 
  * @param result Detection result to free
  */
 void nexus_pipeline_detection_result_free(NexusPipelineDetectionResult* result);
 
 /**
  * @brief Clean up pipeline detector resources
  * 
  * @param ctx NexusLink context
  */
 void nexus_pipeline_detector_cleanup(NexusContext* ctx);
 
 /**
  * @brief Utility function to get pipeline type name as string
  * 
  * @param type Pipeline type enum value
  * @return const char* String representation of the type
  */
 const char* nexus_pipeline_type_to_string(NexusPipelineType type);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CORE_PIPELINE_DETECTOR_H */