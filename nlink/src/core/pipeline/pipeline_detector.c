/**
 * @file pipeline_detector.c
 * @brief Implementation of pipeline type detection and optimization
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/pipeline/pipeline_detector.h"
 #include "nlink/core/minimizer/nexus_minimizer.h"
 #include "nlink/core/minimizer/okpala_automaton.h"
 #include "nlink/spsystem/sps_config.h"
 #include "nlink/spsystem/sps_dependency.h"
 #include "nlink/mpsystem/mps_config.h"
 #include "nlink/mpsystem/mps_dependency.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <sys/stat.h>
 
 /* Internal structure for pipeline detector state */
 typedef struct PipelineDetectorContext {
     NexusPipelineDetectorConfig config;
     bool initialized;
 } PipelineDetectorContext;
 
 /* Global detector context */
 static PipelineDetectorContext* g_detector_ctx = NULL;
 
 /* Helper function to get file size */
 static size_t get_file_size(const char* path) {
     struct stat st;
     if (stat(path, &st) == 0) {
         return (size_t)st.st_size;
     }
     return 0;
 }
 
 /* Helper function to check if file exists */
 static bool file_exists(const char* path) {
     struct stat st;
     return (stat(path, &st) == 0);
 }
 
 /* Helper function to measure time */
 static double get_current_time_ms(void) {
     struct timespec ts;
     clock_gettime(CLOCK_MONOTONIC, &ts);
     return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
 }
 
 /* Helper function to check for multi-pass characteristics */
 static bool has_multipass_characteristics(NexusContext* ctx, const char* component_path) {
     // Check for certain patterns associated with multi-pass systems
     
     // First, attempt to load as a multi-pass configuration
     NexusMPSConfig* mps_config = mps_parse_pipeline_config(ctx, component_path);
     if (mps_config) {
         // Check for bidirectional connections
         for (size_t i = 0; i < mps_config->connection_count; i++) {
             if (mps_config->connections[i]->direction == NEXUS_DIRECTION_BIDIRECTIONAL) {
                 mps_free_pipeline_config(mps_config);
                 return true;
             }
         }
         
         // Check for cycles in the dependency graph
         NexusCycleInfo* cycles = NULL;
         size_t cycle_count = 0;
         bool has_cycles = mps_detect_cycles(ctx, mps_config, &cycles, &cycle_count);
         
         // Clean up
         mps_free_pipeline_config(mps_config);
         
         if (has_cycles) {
             return true;
         }
     }
     
     return false;
 }
 
 /* Helper function to check for single-pass characteristics */
 static bool has_singlepass_characteristics(NexusContext* ctx, const char* component_path) {
     // Check for certain patterns associated with single-pass systems
     
     // Attempt to load as a single-pass configuration
     NexusPipelineConfig* sps_config = sps_parse_pipeline_config(ctx, component_path);
     if (sps_config) {
         // Create a dependency graph
         NexusDependencyGraph* graph = sps_create_dependency_graph(ctx, sps_config);
         if (graph) {
             // Check for linear dependency pattern
             const char** ordered_components = NULL;
             size_t component_count = 0;
             NexusResult result = sps_resolve_dependencies(ctx, graph, &ordered_components, &component_count);
             
             bool is_linear = (result == NEXUS_SUCCESS && ordered_components != NULL);
             
             // Clean up
             if (ordered_components) {
                 free(ordered_components);
             }
             sps_free_dependency_graph(graph);
             sps_free_pipeline_config(sps_config);
             
             if (is_linear) {
                 return true;
             }
         } else {
             sps_free_pipeline_config(sps_config);
         }
     }
     
     return false;
 }
 
 /* Initialize the pipeline detector */
 NexusResult nexus_pipeline_detector_initialize(NexusContext* ctx, 
                                               const NexusPipelineDetectorConfig* config) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if already initialized
     if (g_detector_ctx) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Pipeline detector already initialized");
         return NEXUS_SUCCESS;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Initializing pipeline detector");
     
     // Initialize the minimizer system
     NexusResult result = nexus_minimizer_initialize(ctx);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to initialize minimizer: %d", result);
         return result;
     }
     
     // Allocate detector context
     g_detector_ctx = (PipelineDetectorContext*)malloc(sizeof(PipelineDetectorContext));
     if (!g_detector_ctx) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline detector context");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize context
     memset(g_detector_ctx, 0, sizeof(PipelineDetectorContext));
     g_detector_ctx->initialized = true;
     
     // Set configuration
     if (config) {
         g_detector_ctx->config = *config;
         
         // Copy metrics output path if provided
         if (config->metrics_output_path) {
             g_detector_ctx->config.metrics_output_path = strdup(config->metrics_output_path);
             if (!g_detector_ctx->config.metrics_output_path) {
                 free(g_detector_ctx);
                 g_detector_ctx = NULL;
                 return NEXUS_OUT_OF_MEMORY;
             }
         }
     } else {
         // Use default config
         g_detector_ctx->config = nexus_pipeline_detector_default_config();
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline detector initialized successfully");
     return NEXUS_SUCCESS;
 }
 
 /* Get default pipeline detector configuration */
 NexusPipelineDetectorConfig nexus_pipeline_detector_default_config(void) {
     NexusPipelineDetectorConfig config;
     config.auto_optimize = true;
     config.min_level = NEXUS_MINIMIZE_BASIC;
     config.max_level = NEXUS_MINIMIZE_STANDARD;
     config.collect_metrics = true;
     config.verbose = false;
     config.metrics_output_path = NULL;
     return config;
 }
 
 /* Detect pipeline type from a component */
 NexusResult nexus_pipeline_detect(NexusContext* ctx, 
                                  const char* component_path,
                                  NexusPipelineDetectionResult** result) {
     if (!ctx || !component_path || !result) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if detector is initialized
     if (!g_detector_ctx || !g_detector_ctx->initialized) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline detector not initialized");
         return NEXUS_ERROR_NOT_INITIALIZED;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Detecting pipeline type for: %s", component_path);
     
     // Check if file exists
     if (!file_exists(component_path)) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Component file not found: %s", component_path);
         return NEXUS_FILE_NOT_FOUND;
     }
     
     // Allocate detection result
     NexusPipelineDetectionResult* detection_result = 
         (NexusPipelineDetectionResult*)malloc(sizeof(NexusPipelineDetectionResult));
     if (!detection_result) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate detection result");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize detection result
     memset(detection_result, 0, sizeof(NexusPipelineDetectionResult));
     detection_result->detected_type = NEXUS_PIPELINE_TYPE_UNKNOWN;
     detection_result->component_path = strdup(component_path);
     if (!detection_result->component_path) {
         free(detection_result);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Start detection process
     double start_time = get_current_time_ms();
     
     // First check for multi-pass characteristics
     bool is_multi_pass = has_multipass_characteristics(ctx, component_path);
     
     // Then check for single-pass characteristics
     bool is_single_pass = has_singlepass_characteristics(ctx, component_path);
     
     // Determine the pipeline type based on the detected characteristics
     if (is_multi_pass && is_single_pass) {
         detection_result->detected_type = NEXUS_PIPELINE_TYPE_HYBRID;
         nexus_log(ctx, NEXUS_LOG_INFO, "Detected hybrid pipeline (both single-pass and multi-pass characteristics)");
     } else if (is_multi_pass) {
         detection_result->detected_type = NEXUS_PIPELINE_TYPE_MULTI_PASS;
         nexus_log(ctx, NEXUS_LOG_INFO, "Detected multi-pass pipeline");
     } else if (is_single_pass) {
         detection_result->detected_type = NEXUS_PIPELINE_TYPE_SINGLE_PASS;
         nexus_log(ctx, NEXUS_LOG_INFO, "Detected single-pass pipeline");
     } else {
         detection_result->detected_type = NEXUS_PIPELINE_TYPE_UNKNOWN;
         nexus_log(ctx, NEXUS_LOG_INFO, "Unable to determine pipeline type");
     }
     
     // Record detection time
     double detection_time = get_current_time_ms() - start_time;
     if (g_detector_ctx->config.verbose) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Detection completed in %.2f ms", detection_time);
     }
     
     // Set output parameter
     *result = detection_result;
     
     // Apply optimization if configured
     if (g_detector_ctx->config.auto_optimize && 
         detection_result->detected_type != NEXUS_PIPELINE_TYPE_UNKNOWN) {
         return nexus_pipeline_optimize(ctx, detection_result);
     }
     
     return NEXUS_SUCCESS;
 }
 
 /* Apply optimization to detected pipeline */
 NexusResult nexus_pipeline_optimize(NexusContext* ctx, 
                                    NexusPipelineDetectionResult* result) {
     if (!ctx || !result) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if detector is initialized
     if (!g_detector_ctx || !g_detector_ctx->initialized) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline detector not initialized");
         return NEXUS_ERROR_NOT_INITIALIZED;
     }
     
     // Skip unknown pipeline types
     if (result->detected_type == NEXUS_PIPELINE_TYPE_UNKNOWN) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Cannot optimize unknown pipeline type");
         return NEXUS_ERROR_INVALID_PIPELINE;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Optimizing %s pipeline: %s",
              nexus_pipeline_type_to_string(result->detected_type),
              result->component_path);
     
     // Determine appropriate minimization level based on pipeline type
     NexusMinimizerLevel level = g_detector_ctx->config.min_level;
     
     switch (result->detected_type) {
         case NEXUS_PIPELINE_TYPE_SINGLE_PASS:
             // Single-pass can use standard optimization
             level = (g_detector_ctx->config.max_level > NEXUS_MINIMIZE_STANDARD) ? 
                     NEXUS_MINIMIZE_STANDARD : g_detector_ctx->config.max_level;
             break;
             
         case NEXUS_PIPELINE_TYPE_MULTI_PASS:
             // Multi-pass benefits from aggressive optimization
             level = (g_detector_ctx->config.max_level > NEXUS_MINIMIZE_AGGRESSIVE) ? 
                     NEXUS_MINIMIZE_AGGRESSIVE : g_detector_ctx->config.max_level;
             break;
             
         case NEXUS_PIPELINE_TYPE_HYBRID:
             // Hybrid needs careful optimization
             level = (g_detector_ctx->config.max_level > NEXUS_MINIMIZE_STANDARD) ? 
                     NEXUS_MINIMIZE_STANDARD : g_detector_ctx->config.max_level;
             break;
             
         default:
             break;
     }
     
     // Configure the minimizer
     NexusMinimizerConfig min_config = nexus_minimizer_default_config();
     min_config.level = level;
     min_config.enable_metrics = g_detector_ctx->config.collect_metrics;
     min_config.verbose = g_detector_ctx->config.verbose;
     
     // Allocate metrics if collecting
     NexusMinimizationMetrics* metrics = NULL;
     if (g_detector_ctx->config.collect_metrics) {
         metrics = (NexusMinimizationMetrics*)malloc(sizeof(NexusMinimizationMetrics));
         if (!metrics) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate metrics structure");
             return NEXUS_OUT_OF_MEMORY;
         }
         memset(metrics, 0, sizeof(NexusMinimizationMetrics));
     }
     
     // Apply minimization
     NexusResult min_result = nexus_minimize_component(
         ctx,
         result->component_path,
         min_config,
         metrics
     );
     
     // Update result with optimization status
     result->optimization_applied = (min_result == NEXUS_SUCCESS);
     
     // Store metrics if collected
     if (metrics && g_detector_ctx->config.collect_metrics) {
         result->metrics = metrics;
         result->metrics_count = 1;
         
         // Log metrics if verbose
         if (g_detector_ctx->config.verbose) {
             nexus_print_minimization_metrics(metrics);
         }
         
         // Write metrics to file if configured
         if (g_detector_ctx->config.metrics_output_path) {
             FILE* metrics_file = fopen(g_detector_ctx->config.metrics_output_path, "a");
             if (metrics_file) {
                 fprintf(metrics_file, "Pipeline: %s\n", result->component_path);
                 fprintf(metrics_file, "Type: %s\n", nexus_pipeline_type_to_string(result->detected_type));
                 fprintf(metrics_file, "Optimization Level: %d\n", level);
                 fprintf(metrics_file, "States: %zu -> %zu (%.1f%%)\n", 
                         metrics->original_states, metrics->minimized_states,
                         (1.0 - (double)metrics->minimized_states / metrics->original_states) * 100.0);
                 fprintf(metrics_file, "Size: %.2f KB -> %.2f KB (%.1f%%)\n",
                         metrics->original_size / 1024.0, metrics->minimized_size / 1024.0,
                         (1.0 - (double)metrics->minimized_size / metrics->original_size) * 100.0);
                 fprintf(metrics_file, "Processing Time: %.2f ms\n", metrics->time_taken_ms);
                 fprintf(metrics_file, "Boolean Reduction: %s\n\n", 
                         metrics->boolean_reduction ? "enabled" : "disabled");
                 fclose(metrics_file);
             }
         }
     }
     
     return min_result;
 }
 
 /* Free pipeline detection result resources */
 void nexus_pipeline_detection_result_free(NexusPipelineDetectionResult* result) {
     if (!result) {
         return;
     }
     
     // Free component path
     if (result->component_path) {
         free(result->component_path);
     }
     
     // Free metrics
     if (result->metrics) {
         free(result->metrics);
     }
     
     // Free result structure
     free(result);
 }
 
 /* Clean up pipeline detector resources */
 void nexus_pipeline_detector_cleanup(NexusContext* ctx) {
     if (!g_detector_ctx) {
         return;
     }
     
     if (ctx) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Cleaning up pipeline detector");
     }
     
     // Free metrics output path
     if (g_detector_ctx->config.metrics_output_path) {
         free((void*)g_detector_ctx->config.metrics_output_path);
     }
     
     // Free detector context
     free(g_detector_ctx);
     g_detector_ctx = NULL;
 }
 
 /* Get pipeline type name as string */
 const char* nexus_pipeline_type_to_string(NexusPipelineType type) {
     switch (type) {
         case NEXUS_PIPELINE_TYPE_SINGLE_PASS:
             return "single-pass";
         case NEXUS_PIPELINE_TYPE_MULTI_PASS:
             return "multi-pass";
         case NEXUS_PIPELINE_TYPE_HYBRID:
             return "hybrid";
         case NEXUS_PIPELINE_TYPE_UNKNOWN:
         default:
             return "unknown";
     }
 }