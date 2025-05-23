/**
 * @file sps_lifecycle.c
 * @brief Lifecycle hooks and management for single-pass systems
 *
 * Implements the lifecycle management functions for components in a
 * single-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/spsystem/sps_lifecycle.h"
 #include "nlink/spsystem/sps_pipeline.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/nexus_loader.h"
 #include <string.h>
 #include <stdlib.h>
 
 /* Internal structure for component lifecycle data */
 typedef struct {
     NexusComponentLifecycle lifecycle;
     NexusPipelineComponent* component;
 } ComponentLifecycleData;
 
 /* Forward declarations for internal functions */
 static ComponentLifecycleData* get_lifecycle_data(NexusPipelineComponent* component);
 
 /**
  * Register lifecycle hooks for a component
  */
 NexusResult sps_register_component_lifecycle(NexusContext* ctx, 
                                            NexusPipelineComponent* component,
                                            NexusComponentLifecycle* lifecycle) {
     if (!ctx || !component || !lifecycle) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Registering lifecycle hooks for component '%s'", 
              component->component_id);
     
     // Check if component already has lifecycle data
     ComponentLifecycleData* existing_data = get_lifecycle_data(component);
     
     if (existing_data) {
         // Update existing lifecycle hooks
         existing_data->lifecycle = *lifecycle;
         return NEXUS_SUCCESS;
     }
     
     // Create new lifecycle data
     ComponentLifecycleData* data = (ComponentLifecycleData*)malloc(sizeof(ComponentLifecycleData));
     if (!data) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate lifecycle data");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Copy lifecycle hooks and set component reference
     data->lifecycle = *lifecycle;
     data->component = component;
     
     // Store in component state
     component->component_state = data;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Call initialization hook for a component
  */
 NexusResult sps_component_initialize(NexusContext* ctx, NexusPipelineComponent* component) {
     if (!ctx || !component) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Initializing component '%s'", 
              component->component_id);
     
     // Get lifecycle data
     ComponentLifecycleData* data = get_lifecycle_data(component);
     
     // Skip if already initialized
     if (component->is_initialized) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Component '%s' already initialized", 
                  component->component_id);
         return NEXUS_SUCCESS;
     }
     
     // Call init function if registered
     if (data && data->lifecycle.init_func) {
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Calling init function for component '%s'", 
                  component->component_id);
         
         NexusResult result = data->lifecycle.init_func(component, data->lifecycle.user_data);
         
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Initialization failed for component '%s': %d", 
                      component->component_id, result);
             return result;
         }
     } else {
         nexus_log(ctx, NEXUS_LOG_DEBUG, 
                  "No init function registered for component '%s'", 
                  component->component_id);
     }
     
     // Mark as initialized
     component->is_initialized = true;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Call execution hook for a component
  */
 NexusResult sps_component_execute(NexusContext* ctx, 
                                  NexusPipelineComponent* component,
                                  NexusDataStream* input,
                                  NexusDataStream* output) {
     if (!ctx || !component || !input || !output) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if initialized
     if (!component->is_initialized) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Component '%s' not initialized", 
                  component->component_id);
         return NEXUS_COMPONENT_NOT_INITIALIZED;
     }
     
     // Call process function
     if (component->process_func) {
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Executing component '%s'", 
                  component->component_id);
         
         NexusResult result = component->process_func(component, input, output);
         
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Execution failed for component '%s': %d", 
                      component->component_id, result);
         }
         
         return result;
     }
     
     nexus_log(ctx, NEXUS_LOG_ERROR, "No processing function available for component '%s'", 
              component->component_id);
     return NEXUS_SYMBOL_NOT_FOUND;
 }
 
 /**
  * Call termination hook for a component
  */
 NexusResult sps_component_terminate(NexusContext* ctx, NexusPipelineComponent* component) {
     if (!ctx || !component) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Terminating component '%s'", 
              component->component_id);
     
     // Skip if not initialized
     if (!component->is_initialized) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Component '%s' not initialized", 
                  component->component_id);
         return NEXUS_SUCCESS;
     }
     
     // Get lifecycle data
     ComponentLifecycleData* data = get_lifecycle_data(component);
     
     // Call term function if registered
     if (data && data->lifecycle.term_func) {
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Calling termination function for component '%s'", 
                  component->component_id);
         
         NexusResult result = data->lifecycle.term_func(component, data->lifecycle.user_data);
         
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Termination failed for component '%s': %d", 
                      component->component_id, result);
             return result;
         }
     } else {
         nexus_log(ctx, NEXUS_LOG_DEBUG, 
                  "No termination function registered for component '%s'", 
                  component->component_id);
     }
     
     // Mark as not initialized
     component->is_initialized = false;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Call abort hook for a component
  */
 NexusResult sps_component_abort(NexusContext* ctx, NexusPipelineComponent* component) {
     if (!ctx || !component) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Aborting component '%s'", 
              component->component_id);
     
     // Skip if not initialized
     if (!component->is_initialized) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Component '%s' not initialized", 
                  component->component_id);
         return NEXUS_SUCCESS;
     }
     
     // Get lifecycle data
     ComponentLifecycleData* data = get_lifecycle_data(component);
     
     // Call abort function if registered
     if (data && data->lifecycle.abort_func) {
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Calling abort function for component '%s'", 
                  component->component_id);
         
         NexusResult result = data->lifecycle.abort_func(component, data->lifecycle.user_data);
         
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Abort failed for component '%s': %d", 
                      component->component_id, result);
             return result;
         }
     } else if (data && data->lifecycle.term_func) {
         // Fall back to term function if abort not registered
         nexus_log(ctx, NEXUS_LOG_DEBUG, 
                  "No abort function registered, using term function for component '%s'", 
                  component->component_id);
         
         NexusResult result = data->lifecycle.term_func(component, data->lifecycle.user_data);
         
         if (result != NEXUS_SUCCESS) {
             nexus_log(ctx, NEXUS_LOG_ERROR, 
                      "Termination failed during abort for component '%s': %d", 
                      component->component_id, result);
             return result;
         }
     } else {
         nexus_log(ctx, NEXUS_LOG_DEBUG, 
                  "No abort or termination function registered for component '%s'", 
                  component->component_id);
     }
     
     // Mark as not initialized
     component->is_initialized = false;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Handle error during pipeline execution
  */
 NexusResult sps_handle_pipeline_error(NexusContext* ctx, 
                                      NexusPipeline* pipeline,
                                      NexusResult error,
                                      const char* component_id) {
     if (!ctx || !pipeline) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_ERROR, 
              "Pipeline error: component '%s' failed with result %d", 
              component_id ? component_id : "unknown", error);
     
     // Format error message
     char message[256];
     snprintf(message, sizeof(message), 
             "Component '%s' failed with result %d", 
             component_id ? component_id : "unknown", error);
     
     // Call error handler if registered
     if (pipeline->error_handler) {
         pipeline->error_handler(pipeline, error, component_id, message);
     }
     
     return error;
 }
 
 /**
  * Get the lifecycle data for a component
  */
 static ComponentLifecycleData* get_lifecycle_data(NexusPipelineComponent* component) {
     if (!component) {
         return NULL;
     }
     
     return (ComponentLifecycleData*)component->component_state;
 }