/**
 * @file sps_pipeline.c
 * @brief Core pipeline management for single-pass systems
 *
 * Implements the pipeline management functionality for single-pass
 * systems, including initialization, execution, and cleanup.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/spsystem/sps_pipeline.h"
 #include "nlink/spsystem/sps_dependency.h"
 #include "nlink/spsystem/sps_lifecycle.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/nexus_loader.h"
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 /* Forward declarations for internal functions */
 static NexusResult load_components(NexusContext* ctx, NexusPipeline* pipeline);
 static NexusResult initialize_components(NexusContext* ctx, NexusPipeline* pipeline);
 static NexusResult execute_component(NexusContext* ctx, 
                                     NexusPipelineComponent* component,
                                     NexusDataStream* input,
                                     NexusDataStream* output);
 static NexusResult terminate_components(NexusContext* ctx, NexusPipeline* pipeline);
 static void default_error_handler(NexusPipeline* pipeline, 
                                  NexusResult result, 
                                  const char* component_id, 
                                  const char* message);
 static NexusResult abort_components(NexusContext* ctx, NexusPipeline* pipeline);
 
 /**
  * Create a new pipeline from configuration
  */
 NexusPipeline* sps_pipeline_create(NexusContext* ctx, NexusPipelineConfig* config) {
     if (!ctx || !config) {
         return NULL;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Creating pipeline '%s'", 
              config->pipeline_id ? config->pipeline_id : "unnamed");
     
     // Validate configuration
     NexusResult result = sps_validate_pipeline_config(ctx, config);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Invalid pipeline configuration: %d", result);
         return NULL;
     }
     
     // Allocate pipeline
     NexusPipeline* pipeline = (NexusPipeline*)calloc(1, sizeof(NexusPipeline));
     if (!pipeline) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline");
         return NULL;
     }
     
     // Set initial properties
     pipeline->pipeline_id = config->pipeline_id;
     pipeline->config = config;
     pipeline->is_initialized = false;
     pipeline->error_handler = default_error_handler;
     
     // Build dependency graph
     NexusDependencyGraph* graph = sps_create_dependency_graph(ctx, config);
     if (!graph) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create dependency graph");
         free(pipeline);
         return NULL;
     }
     
     // Resolve dependencies to get component order
     const char** ordered_components = NULL;
     size_t component_count = 0;
     
     result = sps_resolve_dependencies(ctx, graph, &ordered_components, &component_count);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to resolve dependencies: %d", result);
         sps_free_dependency_graph(graph);
         free(pipeline);
         return NULL;
     }
     
     // Allocate component array
     pipeline->components = (NexusPipelineComponent**)calloc(
         component_count, sizeof(NexusPipelineComponent*)
     );
     
     if (!pipeline->components) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate component array");
         free(ordered_components);
         sps_free_dependency_graph(graph);
         free(pipeline);
         return NULL;
     }
     
     // Create component structures in dependency order
     for (size_t i = 0; i < component_count; i++) {
         const char* component_id = ordered_components[i];
         
         // Find component config
         NexusPipelineComponentConfig* comp_config = NULL;
         for (size_t j = 0; j < config->component_count; j++) {
             if (strcmp(config->components[j]->component_id, component_id) == 0) {
                 comp_config = config->components[j];
                 break;
             }
         }
         
         // Skip if not found (shouldn't happen)
         if (!comp_config) {
             continue;
         }
         
         // Create component
         NexusPipelineComponent* component = (NexusPipelineComponent*)calloc(
             1, sizeof(NexusPipelineComponent)
         );
         
         if (!component) {
             continue;
         }
         
         // Initialize component properties
         component->component_id = comp_config->component_id;
         component->last_result = NEXUS_SUCCESS;
         component->is_initialized = false;
         
         // Add to pipeline
         pipeline->components[pipeline->component_count++] = component;
     }
     
     // Clean up
     free(ordered_components);
     sps_free_dependency_graph(graph);
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Created pipeline with %zu components", 
              pipeline->component_count);
     
     return pipeline;
 }
 
 /**
  * Initialize all components in the pipeline
  */
 NexusResult sps_pipeline_initialize(NexusContext* ctx, NexusPipeline* pipeline) {
     if (!ctx || !pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Initializing pipeline '%s'", 
              pipeline->pipeline_id ? pipeline->pipeline_id : "unnamed");
     
     // Don't initialize twice
     if (pipeline->is_initialized) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Pipeline already initialized");
         return NEXUS_SUCCESS;
     }
     
     // Load components
     NexusResult result = load_components(ctx, pipeline);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load components: %d", result);
         return result;
     }
     
     // Initialize components
     result = initialize_components(ctx, pipeline);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to initialize components: %d", result);
         return result;
     }
     
     // Mark as initialized
     pipeline->is_initialized = true;
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline initialized successfully");
     return NEXUS_SUCCESS;
 }
 
 /**
  * Load components from their libraries
  */
 static NexusResult load_components(NexusContext* ctx, NexusPipeline* pipeline) {
     for (size_t i = 0; i < pipeline->component_count; i++) {
         NexusPipelineComponent* component = pipeline->components[i];
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Loading component '%s'", component->component_id);
         
         // Find component config
         NexusPipelineComponentConfig* comp_config = NULL;
         for (size_t j = 0; j < pipeline->config->component_count; j++) {
             if (strcmp(pipeline->config->components[j]->component_id, component->component_id) == 0) {
                 comp_config = pipeline->config->components[j];
                 break;
             }
         }
         
         // Skip if not found
         if (!comp_config) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Configuration not found for component '%s'", 
                      component->component_id);
             return NEXUS_NOT_FOUND;
         }
         
         // Construct component path
         char path[256];
         snprintf(path, sizeof(path), "components/%s/lib%s.so", 
                 component->component_id, component->component_id);
         
         // Load component
         component->component = nexus_load_component(ctx, path, component->component_id);
         if (!component->component) {
             // Skip if optional
             if (comp_config->optional) {
                 nexus_log(ctx, NEXUS_LOG_WARNING, 
                          "Optional component '%s' could not be loaded", 
                          component->component_id);
                 continue;
             }
             
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load component '%s'", 
                      component->component_id);
             return NEXUS_COMPONENT_LOAD_FAILED;
         }
         
         // Get processing function
         char process_symbol[256];
         snprintf(process_symbol, sizeof(process_symbol), "%s_process", 
                 component->component_id);
         
         component->process_func = (NexusProcessFunc)nexus_resolve_component_symbol(
             ctx, component->component, process_symbol
         );
         
         if (!component->process_func) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to resolve processing function for component '%s'", 
                      component->component_id);
             
             // Skip if optional
             if (comp_config->optional) {
                 nexus_log(ctx, NEXUS_LOG_WARNING, 
                          "Using optional component '%s' without processing function", 
                          component->component_id);
                 continue;
             }
             
             return NEXUS_SYMBOL_NOT_FOUND;
         }
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Component '%s' loaded successfully", 
                  component->component_id);
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Initialize components
  */
 static NexusResult initialize_components(NexusContext* ctx, NexusPipeline* pipeline) {
     for (size_t i = 0; i < pipeline->component_count; i++) {
         NexusPipelineComponent* component = pipeline->components[i];
         
         // Skip components that weren't loaded
         if (!component->component) {
             continue;
         }
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Initializing component '%s'", 
                  component->component_id);
         
         // Call lifecycle initialization
         NexusResult result = sps_component_initialize(ctx, component);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to initialize component '%s': %d", 
                      component->component_id, result);
             
             // Check if component is optional
             bool is_optional = false;
             for (size_t j = 0; j < pipeline->config->component_count; j++) {
                 if (strcmp(pipeline->config->components[j]->component_id, component->component_id) == 0) {
                     is_optional = pipeline->config->components[j]->optional;
                     break;
                 }
             }
             
             if (is_optional) {
                 nexus_log(ctx, NEXUS_LOG_WARNING, 
                          "Skipping optional component '%s' due to initialization failure", 
                          component->component_id);
                 continue;
             }
             
             return result;
         }
         
         component->is_initialized = true;
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Execute a component
  */
 static NexusResult execute_component(NexusContext* ctx, 
                                     NexusPipelineComponent* component,
                                     NexusDataStream* input,
                                     NexusDataStream* output) {
     if (!component || !input || !output) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Skip if not initialized
     if (!component->is_initialized) {
         return NEXUS_SUCCESS;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Executing component '%s'", component->component_id);
     
     // Record start time
     struct timespec start, end;
     clock_gettime(CLOCK_MONOTONIC, &start);
     
     // Execute the component
     NexusResult result = sps_component_execute(ctx, component, input, output);
     
     // Record end time
     clock_gettime(CLOCK_MONOTONIC, &end);
     
     // Calculate execution time
     double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                        (end.tv_nsec - start.tv_nsec) / 1000000.0;
     
     component->last_execution_time_ms = elapsed_ms;
     component->last_result = result;
     
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, 
                  "Component '%s' execution failed: %d", 
                  component->component_id, result);
     } else {
         nexus_log(ctx, NEXUS_LOG_DEBUG, 
                  "Component '%s' executed in %.2f ms", 
                  component->component_id, elapsed_ms);
     }
     
     return result;
 }
 
 /**
  * Execute the pipeline with input data
  */
 NexusResult sps_pipeline_execute(NexusContext* ctx, 
                                 NexusPipeline* pipeline, 
                                 NexusDataStream* input, 
                                 NexusDataStream* output) {
     if (!ctx || !pipeline || !input || !output) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Executing pipeline '%s'", 
              pipeline->pipeline_id ? pipeline->pipeline_id : "unnamed");
     
     // Make sure pipeline is initialized
     if (!pipeline->is_initialized) {
         NexusResult result = sps_pipeline_initialize(ctx, pipeline);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to initialize pipeline: %d", result);
             return result;
         }
     }
     
     // Record start time
     struct timespec start, end;
     clock_gettime(CLOCK_MONOTONIC, &start);
     
     // Create intermediate streams for component communication
     NexusDataStream** streams = NULL;
     
     if (pipeline->component_count > 1) {
         streams = (NexusDataStream**)calloc(pipeline->component_count - 1, sizeof(NexusDataStream*));
         if (!streams) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate intermediate streams");
             return NEXUS_OUT_OF_MEMORY;
         }
         
         // Initialize intermediate streams
         for (size_t i = 0; i < pipeline->component_count - 1; i++) {
             streams[i] = sps_stream_create(input->capacity > 0 ? input->capacity : 4096);
             if (!streams[i]) {
                 // Free already allocated streams
                 for (size_t j = 0; j < i; j++) {
                     sps_stream_destroy(streams[j]);
                 }
                 free(streams);
                 
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create intermediate stream %zu", i);
                 return NEXUS_OUT_OF_MEMORY;
             }
             
             // Set format based on component outputs/inputs
             if (i == 0 && pipeline->config->input_format) {
                 streams[i]->format = strdup(pipeline->config->input_format);
             } else {
                 // In a real system, we'd determine this from component metadata
                 streams[i]->format = strdup("binary");
             }
         }
     }
     
     // Process each component
     NexusResult result = NEXUS_SUCCESS;
     NexusResult final_result = NEXUS_SUCCESS;
     
     for (size_t i = 0; i < pipeline->component_count; i++) {
         NexusPipelineComponent* component = pipeline->components[i];
         
         // Skip components that weren't loaded or initialized
         if (!component->is_initialized || !component->component) {
             continue;
         }
         
         // Determine input/output streams
         NexusDataStream* comp_input;
         NexusDataStream* comp_output;
         
         if (i == 0) {
             // First component uses pipeline input
             comp_input = input;
         } else {
             // Use previous intermediate stream
             comp_input = streams[i-1];
         }
         
         if (i == pipeline->component_count - 1) {
             // Last component outputs to pipeline output
             comp_output = output;
         } else {
             // Output to intermediate stream
             comp_output = streams[i];
         }
         
         // Execute component
         result = execute_component(ctx, component, comp_input, comp_output);
         
         if (result != NEXUS_SUCCESS) {
             // Handle error
             sps_handle_pipeline_error(ctx, pipeline, result, component->component_id);
             
             // Record the error but continue if partial processing is allowed
             if (final_result == NEXUS_SUCCESS) {
                 final_result = result; // Only store the first error
             }
             
             // Stop execution if not allowing partial processing
             if (!pipeline->config->allow_partial_processing) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, 
                          "Stopping pipeline execution due to component failure");
                 break;
             } else {
                 nexus_log(ctx, NEXUS_LOG_WARNING, 
                          "Continuing pipeline execution despite component failure");
                 result = NEXUS_SUCCESS; // Reset result to allow continued execution
             }
         }
     }
     
     // Record end time
     clock_gettime(CLOCK_MONOTONIC, &end);
     
     // Calculate execution time
     double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                       (end.tv_nsec - start.tv_nsec) / 1000000.0;
     
     nexus_log(ctx, NEXUS_LOG_INFO, 
              "Pipeline executed in %.2f ms", elapsed_ms);
     
     // Clean up intermediate streams
     if (streams) {
         for (size_t i = 0; i < pipeline->component_count - 1; i++) {
             if (streams[i]) {
                 sps_stream_destroy(streams[i]);
             }
         }
         free(streams);
     }
     
     return final_result == NEXUS_SUCCESS ? result : final_result;
 }
 
 /**
  * Clean up pipeline resources
  */
 void sps_pipeline_destroy(NexusContext* ctx, NexusPipeline* pipeline) {
     if (!ctx || !pipeline) {
         return;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Destroying pipeline '%s'", 
              pipeline->pipeline_id ? pipeline->pipeline_id : "unnamed");
     
     // Terminate components if initialized
     if (pipeline->is_initialized) {
         terminate_components(ctx, pipeline);
     }
     
     // Free component structures
     if (pipeline->components) {
         for (size_t i = 0; i < pipeline->component_count; i++) {
             if (pipeline->components[i]) {
                 // Unload component library
                 if (pipeline->components[i]->component) {
                     nexus_unload_component(ctx, pipeline->components[i]->component);
                 }
                 
                 // Free component structure
                 free(pipeline->components[i]);
             }
         }
         
         free(pipeline->components);
     }
     
     // Note: We don't free pipeline->config since it's owned by the caller
     
     // Free pipeline structure
     free(pipeline);
 }
 
 /**
  * Terminate components
  */
 static NexusResult terminate_components(NexusContext* ctx, NexusPipeline* pipeline) {
     NexusResult final_result = NEXUS_SUCCESS;
     
     for (size_t i = 0; i < pipeline->component_count; i++) {
         NexusPipelineComponent* component = pipeline->components[i];
         
         // Skip components that weren't initialized
         if (!component->is_initialized) {
             continue;
         }
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Terminating component '%s'", 
                  component->component_id);
         
         // Call terminate hook
         NexusResult result = sps_component_terminate(ctx, component);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to terminate component '%s': %d", 
                      component->component_id, result);
             
             if (final_result == NEXUS_SUCCESS) {
                 final_result = result;
             }
         }
         
         component->is_initialized = false;
     }
     
     return final_result;
 }
 
 /**
  * Abort components
  */
 static NexusResult abort_components(NexusContext* ctx, NexusPipeline* pipeline) {
     NexusResult final_result = NEXUS_SUCCESS;
     
     for (size_t i = 0; i < pipeline->component_count; i++) {
         NexusPipelineComponent* component = pipeline->components[i];
         
         // Skip components that weren't initialized
         if (!component->is_initialized) {
             continue;
         }
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Aborting component '%s'", 
                  component->component_id);
         
         // Call abort hook
         NexusResult result = sps_component_abort(ctx, component);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to abort component '%s': %d", 
                      component->component_id, result);
             
             if (final_result == NEXUS_SUCCESS) {
                 final_result = result;
             }
         }
         
         component->is_initialized = false;
     }
     
     return final_result;
 }
 
 /**
  * Default error handler
  */
 static void default_error_handler(NexusPipeline* pipeline, 
                                  NexusResult result, 
                                  const char* component_id, 
                                  const char* message) {
     // This is just a placeholder - in a real system, this would be more sophisticated
     // The default handler doesn't do anything since errors are already logged
     (void)pipeline;
     (void)result;
     (void)component_id;
     (void)message;
 }
 
 /**
  * Get a component from the pipeline by ID
  */
 NexusPipelineComponent* sps_pipeline_get_component(NexusPipeline* pipeline, const char* component_id) {
     if (!pipeline || !component_id) {
         return NULL;
     }
     
     for (size_t i = 0; i < pipeline->component_count; i++) {
         if (pipeline->components[i] && 
             strcmp(pipeline->components[i]->component_id, component_id) == 0) {
             return pipeline->components[i];
         }
     }
     
     return NULL;
 }
 
 /**
  * Add a component to the pipeline dynamically
  */
 NexusResult sps_pipeline_add_component(NexusContext* ctx, 
                                       NexusPipeline* pipeline, 
                                       const char* component_id,
                                       const char* before_component) {
     if (!ctx || !pipeline || !component_id) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if component already exists
     for (size_t i = 0; i < pipeline->component_count; i++) {
         if (pipeline->components[i] && 
             strcmp(pipeline->components[i]->component_id, component_id) == 0) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Component '%s' already exists in pipeline", 
                      component_id);
             return NEXUS_DUPLICATE_COMPONENT;
         }
     }
     
     // Find the insertion point if specified
     size_t insert_idx = pipeline->component_count; // Default to end
     
     if (before_component) {
         for (size_t i = 0; i < pipeline->component_count; i++) {
             if (pipeline->components[i] && 
                 strcmp(pipeline->components[i]->component_id, before_component) == 0) {
                 insert_idx = i;
                 break;
             }
         }
     }
     
     // Resize component array
     NexusPipelineComponent** new_components = (NexusPipelineComponent**)realloc(
         pipeline->components,
         (pipeline->component_count + 1) * sizeof(NexusPipelineComponent*)
     );
     
     if (!new_components) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to resize component array");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     pipeline->components = new_components;
     
     // Shift components if needed
     if (insert_idx < pipeline->component_count) {
         for (size_t i = pipeline->component_count; i > insert_idx; i--) {
             pipeline->components[i] = pipeline->components[i-1];
         }
     }
     
     // Create the new component
     NexusPipelineComponent* component = (NexusPipelineComponent*)calloc(
         1, sizeof(NexusPipelineComponent)
     );
     
     if (!component) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate component");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize component
     component->component_id = strdup(component_id);
     if (!component->component_id) {
         free(component);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Add to the pipeline
     pipeline->components[insert_idx] = component;
     pipeline->component_count++;
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Added component '%s' to pipeline", component_id);
     
     // If pipeline is already initialized, load and initialize the component
     if (pipeline->is_initialized) {
         // Construct component path
         char path[256];
         snprintf(path, sizeof(path), "components/%s/lib%s.so", 
                 component_id, component_id);
         
         // Load component
         component->component = nexus_load_component(ctx, path, component_id);
         if (!component->component) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load component '%s'", 
                      component_id);
             return NEXUS_COMPONENT_LOAD_FAILED;
         }
         
         // Get processing function
         char process_symbol[256];
         snprintf(process_symbol, sizeof(process_symbol), "%s_process", 
                 component_id);
         
         component->process_func = (NexusProcessFunc)nexus_resolve_component_symbol(
             ctx, component->component, process_symbol
         );
         
         if (!component->process_func) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to resolve processing function for component '%s'", 
                      component_id);
             return NEXUS_SYMBOL_NOT_FOUND;
         }
         
         // Initialize the component
         NexusResult result = sps_component_initialize(ctx, component);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to initialize component '%s': %d", 
                      component_id, result);
             return result;
         }
         
         component->is_initialized = true;
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Remove a component from the pipeline dynamically
  */
 NexusResult sps_pipeline_remove_component(NexusContext* ctx, 
                                          NexusPipeline* pipeline, 
                                          const char* component_id) {
     if (!ctx || !pipeline || !component_id) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Find the component
     size_t idx = pipeline->component_count;
     
     for (size_t i = 0; i < pipeline->component_count; i++) {
         if (pipeline->components[i] && 
             strcmp(pipeline->components[i]->component_id, component_id) == 0) {
             idx = i;
             break;
         }
     }
     
     if (idx == pipeline->component_count) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Component '%s' not found in pipeline", 
                  component_id);
         return NEXUS_NOT_FOUND;
     }
     
     // Terminate the component if initialized
     NexusPipelineComponent* component = pipeline->components[idx];
     
     if (component->is_initialized) {
         NexusResult result = sps_component_terminate(ctx, component);
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Failed to terminate component '%s': %d", 
                      component_id, result);
             return result;
         }
     }
     
     // Unload the component if loaded
     if (component->component) {
         nexus_unload_component(ctx, component->component);
     }
     
     // Free component resources
     free((void*)component->component_id);
     free(component);
     
     // Shift remaining components
     for (size_t i = idx; i < pipeline->component_count - 1; i++) {
         pipeline->components[i] = pipeline->components[i+1];
     }
     
     // Update count and NULL the last slot
     pipeline->component_count--;
     pipeline->components[pipeline->component_count] = NULL;
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Removed component '%s' from pipeline", 
              component_id);
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Set pipeline-level error handler
  */
 void sps_pipeline_set_error_handler(NexusPipeline* pipeline, NexusPipelineErrorHandler handler) {
     if (!pipeline) {
         return;
     }
     
     pipeline->error_handler = handler ? handler : default_error_handler;
 }