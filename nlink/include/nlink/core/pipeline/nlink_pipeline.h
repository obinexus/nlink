/**
 * @file nlink_pipeline.h
 * @brief Unified pipeline interface for NexusLink
 * 
 * Provides a common interface for both single-pass and multi-pass pipeline systems,
 * allowing for selective optimization based on application requirements.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_PIPELINE_H
 #define NLINK_PIPELINE_H
 
 #include <stdbool.h>
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Pipeline processing mode
  */
 typedef enum NlinkPipelineMode {
     NLINK_PIPELINE_MODE_AUTO,      /**< Automatically determine optimal mode */
     NLINK_PIPELINE_MODE_SINGLE_PASS, /**< Force single-pass mode */
     NLINK_PIPELINE_MODE_MULTI_PASS   /**< Force multi-pass mode */
 } NlinkPipelineMode;
/**
 * @brief Pipeline stage function prototype
 */
typedef NexusResult (*NlinkPipelineStageFunc)(void* input, void* output, void* user_data);
 
 /**
  * @brief Pipeline configuration
  */
 typedef struct NlinkPipelineConfig {
     NlinkPipelineMode mode;       /**< Pipeline processing mode */
     bool enable_optimization;     /**< Enable pipeline optimization */
     bool enable_caching;          /**< Enable result caching between stages */
     unsigned max_iterations;      /**< Maximum iterations for multi-pass mode */
     const char* schema_path;      /**< Path to pipeline schema definition */
 } NlinkPipelineConfig;
 
/**
 * @brief Pipeline stage structure
 */
typedef struct NlinkPipelineStage {
    char* name;                     /**< Stage name */
    NlinkPipelineStageFunc func;    /**< Stage function */
    void* user_data;                /**< User data for stage function */
    struct NlinkPipelineStage* next; /**< Next stage in the pipeline */
} NlinkPipelineStage;

/**
 * @brief Pipeline implementation
 */
struct NlinkPipeline {
    NlinkPipelineConfig config;     /**< Pipeline configuration */
    NlinkPipelineMode active_mode;  /**< Currently active mode */
    NlinkPipelineStage* first_stage; /**< First stage in the pipeline */
    NlinkPipelineStage* last_stage;  /**< Last stage in the pipeline */
    unsigned stage_count;           /**< Number of stages */
    NexusContext* ctx;              /**< NexusLink context */
    
    /* Statistics for last execution */
    unsigned last_iterations;       /**< Number of iterations in last execution */
    double last_execution_time_ms;  /**< Execution time in milliseconds */
    bool is_optimized;              /**< Whether the pipeline has been optimized */
};

 /**
  * @brief Pipeline interface
  */
 typedef struct NlinkPipeline NlinkPipeline;
 
 /**
  * @brief Pipeline stage function prototype
  */
 typedef NexusResult (*NlinkPipelineStageFunc)(void* input, void* output, void* user_data);
 
 /**
  * @brief Create a new pipeline with the specified configuration
  * 
  * @param ctx NexusLink context
  * @param config Pipeline configuration
  * @return NlinkPipeline* New pipeline or NULL on failure
  */
 NlinkPipeline* nlink_pipeline_create(NexusContext* ctx, const NlinkPipelineConfig* config);
 
 /**
  * @brief Add a stage to the pipeline
  * 
  * @param pipeline Target pipeline
  * @param name Stage name
  * @param func Stage function
  * @param user_data User data passed to the stage function
  * @return NexusResult Result code
  */
 NexusResult nlink_pipeline_add_stage(NlinkPipeline* pipeline, 
                                     const char* name,
                                     NlinkPipelineStageFunc func,
                                     void* user_data);
 
 /**
  * @brief Execute the pipeline with the given input and output
  * 
  * @param pipeline Pipeline to execute
  * @param input Input data
  * @param output Output data
  * @return NexusResult Result code
  */
 NexusResult nlink_pipeline_execute(NlinkPipeline* pipeline, void* input, void* output);
 
 /**
  * @brief Get the actual mode used by the pipeline
  * 
  * @param pipeline Pipeline to query
  * @return NlinkPipelineMode The active pipeline mode
  */
 NlinkPipelineMode nlink_pipeline_get_mode(const NlinkPipeline* pipeline);
 
 /**
  * @brief Get statistics for the last pipeline execution
  * 
  * @param pipeline Pipeline to query
  * @param iterations Pointer to store iteration count (can be NULL)
  * @param execution_time_ms Pointer to store execution time in milliseconds (can be NULL)
  * @return NexusResult Result code
  */
 NexusResult nlink_pipeline_get_stats(const NlinkPipeline* pipeline, 
                                     unsigned* iterations,
                                     double* execution_time_ms);
 
 /**
  * @brief Destroy a pipeline and free its resources
  * 
  * @param pipeline Pipeline to destroy
  */
 void nlink_pipeline_destroy(NlinkPipeline* pipeline);
 
 /**
  * @brief Get the default pipeline configuration
  * 
  * @return NlinkPipelineConfig Default configuration
  */
 NlinkPipelineConfig nlink_pipeline_default_config(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_PIPELINE_H */