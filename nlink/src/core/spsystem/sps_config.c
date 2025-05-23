/**
 * @file sps_config.c
 * @brief Pipeline configuration for single-pass systems
 *
 * Implements the parsing, validation, and management of pipeline
 * configurations in single-pass systems.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/spsystem/sps_config.h"
 #include "nlink/core/common/nexus_json.h"
 #include "nlink/core/common/nexus_core.h"
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 /* Forward declarations for internal functions */
 static NexusResult validate_component_config(NexusContext* ctx, NexusPipelineComponentConfig* config);
 static void free_component_config(NexusPipelineComponentConfig* config, void (*destructor)(void*));
 
 /**
  * Parse a pipeline configuration from a JSON file
  */
 NexusPipelineConfig* sps_parse_pipeline_config(NexusContext* ctx, const char* config_path) {
     if (!ctx || !config_path) {
         return NULL;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Parsing pipeline configuration from %s", config_path);
     
     // Load and parse the JSON file
     NexusJsonDocument* doc = nexus_json_load_file(ctx, config_path);
     if (!doc) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load configuration file: %s", config_path);
         return NULL;
     }
     
     // Allocate pipeline configuration
     NexusPipelineConfig* config = (NexusPipelineConfig*)calloc(1, sizeof(NexusPipelineConfig));
     if (!config) {
         nexus_json_free(doc);
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate pipeline configuration");
         return NULL;
     }
     
     // Parse pipeline ID
     const char* pipeline_id = nexus_json_get_string(doc, "pipeline_id");
     if (pipeline_id) {
         config->pipeline_id = strdup(pipeline_id);
     } else {
         // Generate a default ID if not specified
         char default_id[64];
         snprintf(default_id, sizeof(default_id), "pipeline_%p", (void*)config);
         config->pipeline_id = strdup(default_id);
     }
     
     // Parse pipeline description
     const char* description = nexus_json_get_string(doc, "description");
     if (description) {
         config->description = strdup(description);
     }
     
     // Parse input and output formats
     config->input_format = strdup(nexus_json_get_string(doc, "input_format") ?: "binary");
     config->output_format = strdup(nexus_json_get_string(doc, "output_format") ?: "binary");
     
     // Parse allow_partial_processing flag
     config->allow_partial_processing = nexus_json_get_bool(doc, "allow_partial_processing", false);
     
     // Parse components array
     NexusJsonArray* components_array = nexus_json_get_array(doc, "components");
     if (components_array) {
         size_t component_count = nexus_json_array_size(components_array);
         if (component_count > 0) {
             config->components = (NexusPipelineComponentConfig**)calloc(
                 component_count, sizeof(NexusPipelineComponentConfig*)
             );
             
             if (!config->components) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate component array");
                 nexus_json_free(doc);
                 sps_free_pipeline_config(config);
                 return NULL;
             }
             
             // Parse each component
             for (size_t i = 0; i < component_count; i++) {
                 NexusJsonObject* comp_obj = nexus_json_array_get_object(components_array, i);
                 if (!comp_obj) {
                     continue;
                 }
                 
                 NexusPipelineComponentConfig* comp_config = 
                     (NexusPipelineComponentConfig*)calloc(1, sizeof(NexusPipelineComponentConfig));
                 
                 if (!comp_config) {
                     continue;
                 }
                 
                 // Parse component ID
                 const char* component_id = nexus_json_get_string(comp_obj, "component_id");
                 if (component_id) {
                     comp_config->component_id = strdup(component_id);
                 } else {
                     // Skip components without ID
                     free(comp_config);
                     continue;
                 }
                 
                 // Parse version constraint
                 const char* version = nexus_json_get_string(comp_obj, "version");
                 if (version) {
                     comp_config->version_constraint = strdup(version);
                 }
                 
                 // Parse optional flag
                 comp_config->optional = nexus_json_get_bool(comp_obj, "optional", false);
                 
                 // Parse component-specific configuration
                 NexusJsonObject* comp_config_obj = nexus_json_get_object(comp_obj, "config");
                 if (comp_config_obj && config->component_config_creator) {
                     // Convert to JSON string
                     char* config_json = nexus_json_to_string(comp_config_obj);
                     if (config_json) {
                         // Create component-specific config using provided creator function
                         comp_config->component_config = config->component_config_creator(config_json);
                         free(config_json);
                     }
                 }
                 
                 // Add component to the array
                 config->components[config->component_count++] = comp_config;
             }
         }
     }
     
     // Clean up JSON document
     nexus_json_free(doc);
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Parsed pipeline '%s' with %zu component(s)",
              config->pipeline_id, config->component_count);
     
     return config;
 }
 
 /**
  * Create a default pipeline configuration
  */
 NexusPipelineConfig* sps_create_default_pipeline_config(void) {
     NexusPipelineConfig* config = (NexusPipelineConfig*)calloc(1, sizeof(NexusPipelineConfig));
     if (!config) {
         return NULL;
     }
     
     // Initialize with default values
     config->pipeline_id = strdup("default_pipeline");
     config->description = strdup("Default single-pass pipeline");
     config->input_format = strdup("binary");
     config->output_format = strdup("binary");
     config->allow_partial_processing = false;
     
     return config;
 }
 
 /**
  * Validate a pipeline configuration
  */
 NexusResult sps_validate_pipeline_config(NexusContext* ctx, NexusPipelineConfig* config) {
     if (!ctx || !config) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Validating pipeline configuration '%s'", config->pipeline_id);
     
     // Check for required fields
     if (!config->pipeline_id) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline ID is required");
         return NEXUS_INVALID_CONFIGURATION;
     }
     
     if (!config->input_format) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Input format is required");
         return NEXUS_INVALID_CONFIGURATION;
     }
     
     if (!config->output_format) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Output format is required");
         return NEXUS_INVALID_CONFIGURATION;
     }
     
     // Validate components
     for (size_t i = 0; i < config->component_count; i++) {
         NexusPipelineComponentConfig* comp_config = config->components[i];
         if (!comp_config) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "NULL component at index %zu", i);
             return NEXUS_INVALID_CONFIGURATION;
         }
         
         NexusResult result = validate_component_config(ctx, comp_config);
         if (result != NEXUS_SUCCESS) {
             return result;
         }
     }
     
     // Check for at least one component unless partial processing is allowed
     if (config->component_count == 0 && !config->allow_partial_processing) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Pipeline must have at least one component");
         return NEXUS_INVALID_CONFIGURATION;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline configuration validated successfully");
     return NEXUS_SUCCESS;
 }
 
 /**
  * Validate a component configuration
  */
 static NexusResult validate_component_config(NexusContext* ctx, NexusPipelineComponentConfig* config) {
     if (!config->component_id) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Component ID is required");
         return NEXUS_INVALID_CONFIGURATION;
     }
     
     // Additional validation could be added here
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Free pipeline configuration resources
  */
 void sps_free_pipeline_config(NexusPipelineConfig* config) {
     if (!config) {
         return;
     }
     
     // Free pipeline fields
     free((void*)config->pipeline_id);
     free((void*)config->description);
     free((void*)config->input_format);
     free((void*)config->output_format);
     
     // Free components
     if (config->components) {
         for (size_t i = 0; i < config->component_count; i++) {
             if (config->components[i]) {
                 free_component_config(config->components[i], config->component_config_destructor);
             }
         }
         free(config->components);
     }
     
     // Free the config structure itself
     free(config);
 }
 
 /**
  * Free component configuration resources
  */
 static void free_component_config(NexusPipelineComponentConfig* config, void (*destructor)(void*)) {
     if (!config) {
         return;
     }
     
     free((void*)config->component_id);
     free((void*)config->version_constraint);
     
     // Free component-specific config using provided destructor
     if (config->component_config && destructor) {
         destructor(config->component_config);
     }
     
     free(config);
 }
 
 /**
  * Save a pipeline configuration to a JSON file
  */
 NexusResult sps_save_pipeline_config(NexusContext* ctx, const NexusPipelineConfig* config, const char* path) {
     if (!ctx || !config || !path) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Saving pipeline configuration to %s", path);
     
     // Create a new JSON document
     NexusJsonDocument* doc = nexus_json_create_object();
     if (!doc) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create JSON document");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Add pipeline fields
     if (config->pipeline_id) {
         nexus_json_set_string(doc, "pipeline_id", config->pipeline_id);
     }
     
     if (config->description) {
         nexus_json_set_string(doc, "description", config->description);
     }
     
     if (config->input_format) {
         nexus_json_set_string(doc, "input_format", config->input_format);
     }
     
     if (config->output_format) {
         nexus_json_set_string(doc, "output_format", config->output_format);
     }
     
     nexus_json_set_bool(doc, "allow_partial_processing", config->allow_partial_processing);
     
     // Create components array
     NexusJsonArray* components_array = nexus_json_create_array();
     if (components_array) {
         for (size_t i = 0; i < config->component_count; i++) {
             NexusPipelineComponentConfig* comp_config = config->components[i];
             if (!comp_config) {
                 continue;
             }
             
             NexusJsonObject* comp_obj = nexus_json_create_object();
             if (!comp_obj) {
                 continue;
             }
             
             // Add component fields
             if (comp_config->component_id) {
                 nexus_json_set_string(comp_obj, "component_id", comp_config->component_id);
             }
             
             if (comp_config->version_constraint) {
                 nexus_json_set_string(comp_obj, "version", comp_config->version_constraint);
             }
             
             nexus_json_set_bool(comp_obj, "optional", comp_config->optional);
             
             // TODO: Handle component-specific config serialization
             // This would require cooperation with the config creator/destructor
             
             // Add component to array
             nexus_json_array_append(components_array, comp_obj);
         }
         
         // Add components array to document
         nexus_json_set_array(doc, "components", components_array);
     }
     
     // Save to file
     NexusResult result = nexus_json_save_file(doc, path);
     
     // Clean up
     nexus_json_free(doc);
     
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to save pipeline configuration: %d", result);
     } else {
         nexus_log(ctx, NEXUS_LOG_INFO, "Pipeline configuration saved successfully");
     }
     
     return result;
 }