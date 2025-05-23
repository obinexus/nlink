# NexusLink Pipeline System Implementation Guide

## Introduction

This document provides technical implementation guidance for the NexusLink Pipeline Systems (SPS and MPS) modules. It focuses on core algorithms, data structures, and integration points to facilitate proper implementation of the pipeline architecture.

## Core Implementation Considerations

### 1. Dependency Resolution Implementation

#### Single-Pass System (SPS)

The SPS dependency resolution is implemented using a topological sort algorithm:

```c
static NexusResult sps_topological_sort(NexusContext* ctx, 
                                      NexusDependencyNode** nodes, 
                                      size_t node_count,
                                      NexusDependencyNode*** sorted_nodes) {
    // Allocate result array
    *sorted_nodes = (NexusDependencyNode**)malloc(node_count * sizeof(NexusDependencyNode*));
    if (!*sorted_nodes) return NEXUS_OUT_OF_MEMORY;
    
    // Initialize tracking structures
    bool* visited = (bool*)calloc(node_count, sizeof(bool));
    bool* temp_marks = (bool*)calloc(node_count, sizeof(bool));
    if (!visited || !temp_marks) {
        free(visited);
        free(temp_marks);
        free(*sorted_nodes);
        *sorted_nodes = NULL;
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Index for inserting nodes in result array (in reverse order)
    size_t index = node_count;
    
    // Helper function for depth-first search (captures local variables)
    NexusResult visit_node(size_t node_idx) {
        if (temp_marks[node_idx]) {
            // Cycle detected
            return NEXUS_DEPENDENCY_ERROR;
        }
        
        if (!visited[node_idx]) {
            temp_marks[node_idx] = true;
            
            // Visit dependencies
            NexusDependencyNode* node = nodes[node_idx];
            for (size_t i = 0; i < node->dependency_count; i++) {
                const char* dep_id = node->dependencies[i];
                
                // Find index of dependency
                size_t dep_idx = node_count; // Invalid value
                for (size_t j = 0; j < node_count; j++) {
                    if (strcmp(nodes[j]->component_id, dep_id) == 0) {
                        dep_idx = j;
                        break;
                    }
                }
                
                if (dep_idx == node_count) {
                    // Dependency not found
                    return NEXUS_NOT_FOUND;
                }
                
                // Recursively visit dependency
                NexusResult result = visit_node(dep_idx);
                if (result != NEXUS_SUCCESS) {
                    return result;
                }
            }
            
            // Mark node as visited
            temp_marks[node_idx] = false;
            visited[node_idx] = true;
            
            // Add to sorted list (in reverse order)
            index--;
            (*sorted_nodes)[index] = nodes[node_idx];
        }
        
        return NEXUS_SUCCESS;
    }
    
    // Visit all nodes
    for (size_t i = 0; i < node_count; i++) {
        if (!visited[i]) {
            NexusResult result = visit_node(i);
            if (result != NEXUS_SUCCESS) {
                free(visited);
                free(temp_marks);
                free(*sorted_nodes);
                *sorted_nodes = NULL;
                return result;
            }
        }
    }
    
    // If index is not 0, something went wrong
    if (index != 0) {
        free(visited);
        free(temp_marks);
        free(*sorted_nodes);
        *sorted_nodes = NULL;
        return NEXUS_INVALID_OPERATION;
    }
    
    // Clean up
    free(visited);
    free(temp_marks);
    
    return NEXUS_SUCCESS;
}
```

#### Multi-Pass System (MPS)

The MPS dependency resolution is more complex and uses Tarjan's algorithm to find strongly connected components:

```c
static NexusResult mps_find_strongly_connected_components_impl(
    NexusContext* ctx,
    NexusMPSDependencyGraph* graph,
    NexusComponent*** components,
    size_t** component_counts,
    size_t* group_count) {
    
    // Implementation of Tarjan's algorithm for finding strongly connected components
    
    // Initialize data structures
    int* index = (int*)calloc(graph->node_count, sizeof(int));
    int* lowlink = (int*)calloc(graph->node_count, sizeof(int));
    bool* on_stack = (bool*)calloc(graph->node_count, sizeof(bool));
    size_t* stack = (size_t*)malloc(graph->node_count * sizeof(size_t));
    size_t stack_size = 0;
    int current_index = 0;
    
    // Temporary storage for components
    NexusComponent*** temp_components = (NexusComponent***)malloc(graph->node_count * sizeof(NexusComponent**));
    size_t* temp_counts = (size_t*)calloc(graph->node_count, sizeof(size_t));
    size_t temp_group_count = 0;
    
    if (!index || !lowlink || !on_stack || !stack || !temp_components || !temp_counts) {
        // Memory allocation failed
        free(index);
        free(lowlink);
        free(on_stack);
        free(stack);
        free(temp_components);
        free(temp_counts);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Recursive function for Tarjan's algorithm
    void strongconnect(size_t v) {
        // Set the depth index for v
        index[v] = current_index;
        lowlink[v] = current_index;
        current_index++;
        stack[stack_size++] = v;
        on_stack[v] = true;
        
        // Consider successors of v
        for (size_t i = 0; i < graph->nodes[v].outgoing_count; i++) {
            size_t edge_idx = graph->nodes[v].outgoing_edges[i];
            size_t w = graph->edges[edge_idx].target_idx;
            
            if (index[w] == 0) {
                // Successor w has not yet been visited; recurse on it
                strongconnect(w);
                lowlink[v] = (lowlink[v] < lowlink[w]) ? lowlink[v] : lowlink[w];
            } else if (on_stack[w]) {
                // Successor w is in stack and hence in the current SCC
                lowlink[v] = (lowlink[v] < index[w]) ? lowlink[v] : index[w];
            }
        }
        
        // If v is a root node, pop the stack and generate an SCC
        if (lowlink[v] == index[v]) {
            // Start a new strongly connected component
            size_t component_size = 0;
            NexusComponent** component = (NexusComponent**)malloc(graph->node_count * sizeof(NexusComponent*));
            if (!component) {
                // Memory allocation failed
                // In a real implementation, we'd need to handle this error properly
                return;
            }
            
            size_t w;
            do {
                w = stack[--stack_size];
                on_stack[w] = false;
                
                // Add node to component
                component[component_size++] = (NexusComponent*)graph->nodes[w].config;
                
            } while (w != v);
            
            // Store the component
            temp_components[temp_group_count] = component;
            temp_counts[temp_group_count] = component_size;
            temp_group_count++;
        }
    }
    
    // Initialize all nodes as not visited
    for (size_t i = 0; i < graph->node_count; i++) {
        index[i] = 0; // 0 means not visited
    }
    
    // Visit all nodes
    for (size_t i = 0; i < graph->node_count; i++) {
        if (index[i] == 0) {
            strongconnect(i);
        }
    }
    
    // Copy results to output parameters
    *components = (NexusComponent**)malloc(temp_group_count * sizeof(NexusComponent*));
    *component_counts = (size_t*)malloc(temp_group_count * sizeof(size_t));
    if (!*components || !*component_counts) {
        // Memory allocation failed
        free(index);
        free(lowlink);
        free(on_stack);
        free(stack);
        for (size_t i = 0; i < temp_group_count; i++) {
            free(temp_components[i]);
        }
        free(temp_components);
        free(temp_counts);
        free(*components);
        free(*component_counts);
        *components = NULL;
        *component_counts = NULL;
        return NEXUS_OUT_OF_MEMORY;
    }
    
    for (size_t i = 0; i < temp_group_count; i++) {
        (*components)[i] = temp_components[i];
        (*component_counts)[i] = temp_counts[i];
    }
    
    *group_count = temp_group_count;
    
    // Clean up
    free(index);
    free(lowlink);
    free(on_stack);
    free(stack);
    free(temp_components);
    free(temp_counts);
    
    return NEXUS_SUCCESS;
}
```

### 2. Stream Implementation

The stream implementation should be memory-efficient and support incremental processing:

#### SPS Stream Management

```c
NexusDataStream* sps_stream_create(size_t initial_capacity) {
    NexusDataStream* stream = (NexusDataStream*)malloc(sizeof(NexusDataStream));
    if (!stream) {
        return NULL;
    }
    
    // Initialize the stream
    stream->data = malloc(initial_capacity);
    if (!stream->data) {
        free(stream);
        return NULL;
    }
    
    stream->capacity = initial_capacity;
    stream->size = 0;
    stream->position = 0;
    stream->format = NULL;
    stream->metadata = NULL;
    stream->owns_data = true;
    
    return stream;
}

NexusResult sps_stream_write(NexusDataStream* stream, const void* data, size_t size) {
    if (!stream || !data) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Check if we need to resize
    if (stream->position + size > stream->capacity) {
        size_t new_capacity = stream->capacity * 2;
        while (new_capacity < stream->position + size) {
            new_capacity *= 2;
        }
        
        void* new_data = realloc(stream->data, new_capacity);
        if (!new_data) {
            return NEXUS_OUT_OF_MEMORY;
        }
        
        stream->data = new_data;
        stream->capacity = new_capacity;
    }
    
    // Copy the data
    memcpy((uint8_t*)stream->data + stream->position, data, size);
    stream->position += size;
    
    // Update the size if needed
    if (stream->position > stream->size) {
        stream->size = stream->position;
    }
    
    return NEXUS_SUCCESS;
}
```

#### MPS Stream Map Management

```c
NexusMPSDataStreamMap* mps_stream_map_create(size_t initial_capacity) {
    NexusMPSDataStreamMap* map = (NexusMPSDataStreamMap*)malloc(sizeof(NexusMPSDataStreamMap));
    if (!map) {
        return NULL;
    }
    
    // Initialize the map
    map->entries = (MPSStreamMapEntry*)malloc(initial_capacity * sizeof(MPSStreamMapEntry));
    if (!map->entries) {
        free(map);
        return NULL;
    }
    
    map->capacity = initial_capacity;
    map->count = 0;
    
    return map;
}

NexusResult mps_stream_map_add(NexusMPSDataStreamMap* map, 
                              const char* source_id, 
                              const char* target_id, 
                              NexusMPSDataStream* stream) {
    if (!map || !source_id || !target_id || !stream) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Check if we need to resize
    if (map->count >= map->capacity) {
        size_t new_capacity = map->capacity * 2;
        MPSStreamMapEntry* new_entries = (MPSStreamMapEntry*)realloc(map->entries, new_capacity * sizeof(MPSStreamMapEntry));
        if (!new_entries) {
            return NEXUS_OUT_OF_MEMORY;
        }
        
        map->entries = new_entries;
        map->capacity = new_capacity;
    }
    
    // Check if the entry already exists
    for (size_t i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].key.source_id, source_id) == 0 &&
            strcmp(map->entries[i].key.target_id, target_id) == 0) {
            // Replace the existing stream
            map->entries[i].stream = stream;
            return NEXUS_SUCCESS;
        }
    }
    
    // Add the new entry
    map->entries[map->count].key.source_id = strdup(source_id);
    map->entries[map->count].key.target_id = strdup(target_id);
    map->entries[map->count].stream = stream;
    
    if (!map->entries[map->count].key.source_id || !map->entries[map->count].key.target_id) {
        free(map->entries[map->count].key.source_id);
        free(map->entries[map->count].key.target_id);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    map->count++;
    return NEXUS_SUCCESS;
}
```

### 3. Pipeline Execution Implementation

#### SPS Pipeline Execution

```c
NexusResult sps_pipeline_execute(NexusContext* ctx, 
                                NexusPipeline* pipeline, 
                                NexusDataStream* input, 
                                NexusDataStream* output) {
    if (!ctx || !pipeline || !input || !output) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    if (!pipeline->is_initialized) {
        return NEXUS_NOT_INITIALIZED;
    }
    
    // Reset output stream
    sps_stream_reset(output);
    
    // Create temporary stream for intermediate results
    NexusDataStream* temp_input = sps_stream_clone(input);
    NexusDataStream* temp_output = sps_stream_create(input->capacity);
    
    if (!temp_input || !temp_output) {
        sps_stream_destroy(temp_input);
        sps_stream_destroy(temp_output);
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Execute each component in order
    for (size_t i = 0; i < pipeline->component_count; i++) {
        NexusPipelineComponent* component = pipeline->components[i];
        
        // Reset temp output for this component
        sps_stream_reset(temp_output);
        
        // Execute the component
        NexusResult result = sps_component_execute(ctx, component, temp_input, temp_output);
        component->last_result = result;
        
        if (result != NEXUS_SUCCESS) {
            if (pipeline->error_handler) {
                pipeline->error_handler(pipeline, result, component->component_id, 
                                       "Component execution failed");
            }
            
            if (!pipeline->config->allow_partial_processing) {
                sps_stream_destroy(temp_input);
                sps_stream_destroy(temp_output);
                return result;
            }
        }
        
        // Swap input and output for the next component
        NexusDataStream* swap = temp_input;
        temp_input = temp_output;
        temp_output = swap;
    }
    
    // Copy the final result to the output stream
    sps_stream_reset(output);
    uint8_t buffer[4096];
    size_t bytes_read;
    
    sps_stream_reset(temp_input);
    while ((bytes_read = sps_stream_read(temp_input, buffer, sizeof(buffer), &bytes_read)) > 0) {
        NexusResult result = sps_stream_write(output, buffer, bytes_read);
        if (result != NEXUS_SUCCESS) {
            sps_stream_destroy(temp_input);
            sps_stream_destroy(temp_output);
            return result;
        }
    }
    
    // Clean up
    sps_stream_destroy(temp_input);
    sps_stream_destroy(temp_output);
    
    return NEXUS_SUCCESS;
}
```

#### MPS Pipeline Execution

```c
NexusResult mps_pipeline_execute(NexusContext* ctx, 
                               NexusMPSPipeline* pipeline, 
                               NexusMPSDataStream* input, 
                               NexusMPSDataStream* output) {
    if (!ctx || !pipeline || !input || !output) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    if (!pipeline->is_initialized) {
        return NEXUS_NOT_INITIALIZED;
    }
    
    // Reset output stream
    mps_stream_reset(output);
    
    // Create stream map
    NexusMPSDataStreamMap* streams = mps_stream_map_create(pipeline->component_count * 2);
    if (!streams) {
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Add input and output streams
    if (pipeline->component_count > 0) {
        // Determine first and last components (assuming they're properly ordered in groups)
        const char* first_component_id = NULL;
        const char* last_component_id = NULL;
        
        if (pipeline->groups && pipeline->group_count > 0) {
            // First component is in the first group
            first_component_id = pipeline->groups[0]->component_ids[0];
            
            // Last component is in the last group
            NexusExecutionGroup* last_group = pipeline->groups[pipeline->group_count - 1];
            last_component_id = last_group->component_ids[last_group->component_count - 1];
        }
        
        if (first_component_id && last_component_id) {
            // Create a copy of the input stream for the first component
            NexusMPSDataStream* input_copy = mps_stream_clone(input);
            if (!input_copy) {
                mps_stream_map_destroy(streams);
                return NEXUS_OUT_OF_MEMORY;
            }
            
            // Add the input stream to the map
            NexusResult result = mps_stream_map_add(streams, "input", first_component_id, input_copy);
            if (result != NEXUS_SUCCESS) {
                mps_stream_destroy(input_copy);
                mps_stream_map_destroy(streams);
                return result;
            }
            
            // Create an output stream for the last component
            NexusMPSDataStream* output_stream = mps_stream_create(input->capacity);
            if (!output_stream) {
                mps_stream_map_destroy(streams);
                return NEXUS_OUT_OF_MEMORY;
            }
            
            // Add the output stream to the map
            result = mps_stream_map_add(streams, last_component_id, "output", output_stream);
            if (result != NEXUS_SUCCESS) {
                mps_stream_destroy(output_stream);
                mps_stream_map_destroy(streams);
                return result;
            }
        }
    }
    
    // Create streams for all component connections
    for (size_t i = 0; i < pipeline->config->connection_count; i++) {
        NexusComponentConnection* connection = pipeline->config->connections[i];
        
        // Create stream for this connection
        NexusMPSDataStream* stream = mps_stream_create(input->capacity);
        if (!stream) {
            mps_stream_map_destroy(streams);
            return NEXUS_OUT_OF_MEMORY;
        }
        
        // Add the stream to the map
        NexusResult result = mps_stream_map_add(streams, connection->source_id, connection->target_id, stream);
        if (result != NEXUS_SUCCESS) {
            mps_stream_destroy(stream);
            mps_stream_map_destroy(streams);
            return result;
        }
        
        // For bidirectional connections, add a reverse stream
        if (connection->direction == NEXUS_DIRECTION_BIDIRECTIONAL) {
            NexusMPSDataStream* reverse_stream = mps_stream_create(input->capacity);
            if (!reverse_stream) {
                mps_stream_map_destroy(streams);
                return NEXUS_OUT_OF_MEMORY;
            }
            
            result = mps_stream_map_add(streams, connection->target_id, connection->source_id, reverse_stream);
            if (result != NEXUS_SUCCESS) {
                mps_stream_destroy(reverse_stream);
                mps_stream_map_destroy(streams);
                return result;
            }
        }
    }
    
    // Execute the pipeline iterations
    pipeline->current_iteration = 0;
    bool continue_execution = true;
    
    while (continue_execution && 
           (pipeline->max_iterations == 0 || pipeline->current_iteration < pipeline->max_iterations)) {
        
        // Execute each group
        for (size_t i = 0; i < pipeline->group_count; i++) {
            NexusExecutionGroup* group = pipeline->groups[i];
            
            // Execute this group
            NexusResult result = mps_pipeline_execute_group(ctx, pipeline, group, streams);
            
            if (result != NEXUS_SUCCESS) {
                if (!pipeline->config->allow_partial_processing) {
                    mps_stream_map_destroy(streams);
                    return result;
                }
            }
        }
        
        // Check if we should continue to the next iteration
        // This would be based on component return values or stream state
        // For this example, we'll just stop after one iteration
        continue_execution = false;
        
        pipeline->current_iteration++;
    }
    
    // Copy output stream to the provided output
    NexusMPSDataStream* output_stream = mps_stream_map_get(streams, pipeline->components[pipeline->component_count - 1]->component_id, "output");
    if (output_stream) {
        // Copy data from output_stream to output
        uint8_t buffer[4096];
        size_t bytes_read;
        
        mps_stream_reset(output_stream);
        while (mps_stream_read(output_stream, buffer, sizeof(buffer), &bytes_read) == NEXUS_SUCCESS && bytes_read > 0) {
            mps_stream_write(output, buffer, bytes_read);
        }
    }
    
    // Update statistics
    pipeline->stats.total_iterations = pipeline->current_iteration;
    
    // Clean up
    mps_stream_map_destroy(streams);
    
    return NEXUS_SUCCESS;
}
```

### 4. JSON Configuration Parsing

Configuration parsing should use the built-in JSON parser to convert configuration files into internal structures:

```c
NexusPipelineConfig* sps_parse_pipeline_config(NexusContext* ctx, const char* config_path) {
    if (!ctx || !config_path) {
        return NULL;
    }
    
    // Parse JSON from file
    NexusJsonValue* json = nexus_json_parse_file(config_path);
    if (!json) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to parse pipeline config: %s", config_path);
        return NULL;
    }
    
    // Allocate configuration structure
    NexusPipelineConfig* config = (NexusPipelineConfig*)malloc(sizeof(NexusPipelineConfig));
    if (!config) {
        nexus_json_free(json);
        return NULL;
    }
    
    // Initialize with defaults
    memset(config, 0, sizeof(NexusPipelineConfig));
    
    // Extract basic properties
    const char* pipeline_id = nexus_json_object_get_string(json, "pipeline_id", NULL);
    const char* description = nexus_json_object_get_string(json, "description", NULL);
    const char* input_format = nexus_json_object_get_string(json, "input_format", NULL);
    const char* output_format = nexus_json_object_get_string(json, "output_format", NULL);
    bool allow_partial = nexus_json_object_get_bool(json, "allow_partial_processing", false);
    
    if (pipeline_id) config->pipeline_id = strdup(pipeline_id);
    if (description) config->description = strdup(description);
    if (input_format) config->input_format = strdup(input_format);
    if (output_format) config->output_format = strdup(output_format);
    config->allow_partial_processing = allow_partial;
    
    // Extract components
    NexusJsonValue* components_json = nexus_json_object_get(json, "components");
    if (components_json && components_json->type == NEXUS_JSON_ARRAY) {
        size_t component_count = components_json->data.array.count;
        
        if (component_count > 0) {
            // Allocate component array
            config->components = (NexusPipelineComponentConfig**)malloc(
                component_count * sizeof(NexusPipelineComponentConfig*));
            
            if (!config->components) {
                nexus_json_free(json);
                sps_free_pipeline_config(config);
                return NULL;
            }
            
            // Extract each component
            for (size_t i = 0; i < component_count; i++) {
                NexusJsonValue* comp_json = components_json->data.array.items[i];
                
                if (comp_json && comp_json->type == NEXUS_JSON_OBJECT) {
                    NexusPipelineComponentConfig* comp_config = 
                        (NexusPipelineComponentConfig*)malloc(sizeof(NexusPipelineComponentConfig));
                    
                    if (!comp_config) {
                        nexus_json_free(json);
                        sps_free_pipeline_config(config);
                        return NULL;
                    }
                    
                    memset(comp_config, 0, sizeof(NexusPipelineComponentConfig));
                    
                    const char* component_id = nexus_json_object_get_string(comp_json, "component_id", NULL);
                    const char* version_constraint = nexus_json_object_get_string(comp_json, "version_constraint", NULL);
                    bool optional = nexus_json_object_get_bool(comp_json, "optional", false);
                    
                    if (component_id) comp_config->component_id = strdup(component_id);
                    if (version_constraint) comp_config->version_constraint = strdup(version_constraint);
                    comp_config->optional = optional;
                    
                    // Additional component-specific configuration
                    NexusJsonValue* comp_config_json = nexus_json_object_get(comp_json, "config");
                    if (comp_config_json && config->component_config_creator) {
                        char* config_str = nexus_json_to_string(comp_config_json, false);
                        if (config_str) {
                            comp_config->component_config = config->component_config_creator(config_str);
                            free(config_str);
                        }
                    }
                    
                    // Store the component config
                    config->components[i] = comp_config;
                }
            }
            
            config->component_count = component_count;
        }
    }
    
    // Free JSON structure
    nexus_json_free(json);
    
    return config;
}
```

For the MPS configuration, the parsing adds support for connection definitions:

```c
NexusMPSConfig* mps_parse_pipeline_config(NexusContext* ctx, const char* config_path) {
    // Basic parsing similar to SPS, with additional handling for connections
    
    // Extract connections
    NexusJsonValue* connections_json = nexus_json_object_get(json, "connections");
    if (connections_json && connections_json->type == NEXUS_JSON_ARRAY) {
        size_t connection_count = connections_json->data.array.count;
        
        if (connection_count > 0) {
            // Allocate connection array
            config->connections = (NexusComponentConnection**)malloc(
                connection_count * sizeof(NexusComponentConnection*));
            
            if (!config->connections) {
                nexus_json_free(json);
                mps_free_pipeline_config(config);
                return NULL;
            }
            
            // Extract each connection
            for (size_t i = 0; i < connection_count; i++) {
                NexusJsonValue* conn_json = connections_json->data.array.items[i];
                
                if (conn_json && conn_json->type == NEXUS_JSON_OBJECT) {
                    NexusComponentConnection* conn = 
                        (NexusComponentConnection*)malloc(sizeof(NexusComponentConnection));
                    
                    if (!conn) {
                        nexus_json_free(json);
                        mps_free_pipeline_config(config);
                        return NULL;
                    }
                    
                    memset(conn, 0, sizeof(NexusComponentConnection));
                    
                    const char* source_id = nexus_json_object_get_string(conn_json, "source_id", NULL);
                    const char* target_id = nexus_json_object_get_string(conn_json, "target_id", NULL);
                    const char* direction_str = nexus_json_object_get_string(conn_json, "direction", "forward");
                    const char* data_format = nexus_json_object_get_string(conn_json, "data_format", NULL);
                    bool optional = nexus_json_object_get_bool(conn_json, "optional", false);
                    
                    if (source_id) conn->source_id = strdup(source_id);
                    if (target_id) conn->target_id = strdup(target_id);
                    if (data_format) conn->data_format = strdup(data_format);
                    conn->optional = optional;
                    
                    // Parse direction
                    if (strcmp(direction_str, "forward") == 0) {
                        conn->direction = NEXUS_DIRECTION_FORWARD;
                    } else if (strcmp(direction_str, "backward") == 0) {
                        conn->direction = NEXUS_DIRECTION_BACKWARD;
                    } else if (strcmp(direction_str, "bidirectional") == 0) {
                        conn->direction = NEXUS_DIRECTION_BIDIRECTIONAL;
                    } else {
                        // Default to forward
                        conn->direction = NEXUS_DIRECTION_FORWARD;
                    }
                    
                    // Store the connection
                    config->connections[i] = conn;
                }
            }
            
            config->connection_count = connection_count;
        }
    }
    
    // Extract MPS-specific properties
    config->allow_cycles = nexus_json_object_get_bool(json, "allow_cycles", false);
    config->max_iteration_count = (int)nexus_json_object_get_number(json, "max_iteration_count", 0);
    
    // Free JSON structure
    nexus_json_free(json);
    
    return config;
}
```

## Data Structure Details

### 1. Configuration Structures

For optimal implementation, the two system types should have similar but specialized configuration structures:

#### SPS Configuration

```c
typedef struct NexusPipelineConfig {
    const char* pipeline_id;                          // Pipeline identifier
    const char* description;                          // Pipeline description
    NexusPipelineComponentConfig** components;        // Array of component configurations
    size_t component_count;                           // Number of components
    const char* input_format;                         // Input data format
    const char* output_format;                        // Output data format
    bool allow_partial_processing;                    // Allow partial pipeline execution
    void* (*component_config_creator)(const char*);   // Function to create component config from JSON
    void (*component_config_destructor)(void*);       // Function to destroy component config
} NexusPipelineConfig;
```

#### MPS Configuration

```c
typedef struct NexusMPSConfig {
    const char* pipeline_id;                          // Pipeline identifier
    const char* description;                          // Pipeline description
    NexusMPSComponentConfig** components;             // Array of component configurations
    size_t component_count;                           // Number of components
    NexusComponentConnection** connections;           // Array of component connections
    size_t connection_count;                          // Number of connections
    bool allow_cycles;                                // Whether cycles are allowed in the pipeline
    int max_iteration_count;                          // Maximum iteration count (0 = unlimited)
    bool allow_partial_processing;                    // Allow partial pipeline execution
    void* (*component_config_creator)(const char*);   // Function to create component config from JSON
    void (*component_config_destructor)(void*);       // Function to destroy component config
} NexusMPSConfig;
```

Key differences:
- MPS adds explicit connections between components
- MPS includes cycle handling and iteration limits
- MPS component config includes reentrance support

### 2. Memory Management Strategy

Proper memory management is critical for these pipeline systems:

1. **Component Ownership**: The pipeline owns its components and is responsible for destroying them
2. **Stream Ownership**: Streams can be owned by either the caller or the pipeline, determined by the `owns_data` flag
3. **Configuration Ownership**: Pipeline takes ownership of configuration structures

Memory cleanup example for SPS:

```c
void sps_pipeline_destroy(NexusContext* ctx, NexusPipeline* pipeline) {
    if (!ctx || !pipeline) {
        return;
    }
    
    // First terminate all components
    for (size_t i = 0; i < pipeline->component_count; i++) {
        if (pipeline->components[i]) {
            sps_component_terminate(ctx, pipeline->components[i]);
            
            // Free component resources
            free((void*)pipeline->components[i]->component_id);
            
            // Unload the component
            if (pipeline->components[i]->component) {
                nexus_unload_component(ctx, pipeline->components[i]->component);
            }
            
            free(pipeline->components[i]);
        }
    }
    
    // Free array of components
    free(pipeline->components);
    
    // Free pipeline ID
    free((void*)pipeline->pipeline_id);
    
    // Free pipeline configuration (if we own it)
    if (pipeline->config) {
        sps_free_pipeline_config(pipeline->config);
    }
    
    // Free user data if a destructor is provided
    if (pipeline->user_data_destructor && pipeline->user_data) {
        pipeline->user_data_destructor(pipeline->user_data);
    }
    
    // Free the pipeline itself
    free(pipeline);
}
```

## Error Handling

Error handling is crucial for robust pipeline operation. Both SPS and MPS systems should implement:

1. **Graceful degradation**: Support for partial processing when enabled
2. **Comprehensive error reporting**: Detailed error information via the context logger
3. **Component-specific error handling**: Each component should report its specific errors

Example of pipeline error handler:

```c
NexusResult sps_handle_pipeline_error(NexusContext* ctx, 
                                     NexusPipeline* pipeline,
                                     NexusResult error,
                                     const char* component_id) {
    if (!ctx || !pipeline) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Log the error
    nexus_log(ctx, NEXUS_LOG_ERROR, 
             "Pipeline error in component '%s': %s",
             component_id ? component_id : "unknown",
             nexus_result_to_string(error));
    
    // Call user-defined error handler if available
    if (pipeline->error_handler) {
        pipeline->error_handler(pipeline, error, component_id, nexus_result_to_string(error));
    }
    
    // Determine if we should continue or abort
    if (pipeline->config && pipeline->config->allow_partial_processing) {
        // Continue with partial processing
        return NEXUS_PARTIAL_SUCCESS;
    } else {
        // Abort processing
        return error;
    }
}
```

## Implementation Notes for Bidirectional Flow (MPS)

The MPS system requires special handling for bidirectional data flow:

1. **Strongly Connected Components**: Use Tarjan's algorithm to identify cycles in the dependency graph
2. **Execution Groups**: Group components that need to be executed together due to cyclic dependencies
3. **Iteration Control**: Implement mechanism to prevent infinite loops and control iteration count

Example execution group implementation:

```c
NexusResult mps_pipeline_execute_group(NexusContext* ctx,
                                      NexusMPSPipeline* pipeline,
                                      NexusExecutionGroup* group,
                                      NexusMPSDataStreamMap* streams) {
    if (!ctx || !pipeline || !group || !streams) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // For cyclic groups, we may need multiple passes
    bool continue_execution = true;
    int pass_count = 0;
    const int max_passes = 10; // Prevent infinite loops
    
    while (continue_execution && pass_count < max_passes) {
        continue_execution = false;
        
        // Execute each component in the group
        for (size_t i = 0; i < group->component_count; i++) {
            const char* component_id = group->component_ids[i];
            
            // Find the component
            NexusMPSPipelineComponent* component = mps_pipeline_get_component(pipeline, component_id);
            if (!component) {
                nexus_log(ctx, NEXUS_LOG_ERROR, "Component not found: %s", component_id);
                continue;
            }
            
            // Get all incoming streams for this component
            NexusMPSDataStream** input_streams = NULL;
            char** source_ids = NULL;
            size_t input_count = 0;
            
            NexusResult result = mps_stream_map_get_incoming(
                streams, component_id, &input_streams, &source_ids, &input_count);
                
            if (result != NEXUS_SUCCESS || input_count == 0) {
                // No inputs, skip this component
                continue;
            }
            
            // Get all outgoing streams for this component
            NexusMPSDataStream** output_streams = NULL;
            char** target_ids = NULL;
            size_t output_count = 0;
            
            result = mps_stream_map_get_outgoing(
                streams, component_id, &output_streams, &target_ids, &output_count);
                
            if (result != NEXUS_SUCCESS || output_count == 0) {
                // No outputs, skip this component
                free(input_streams);
                free(source_ids);
                continue;
            }
            
            // Execute the component for each input-output pair
            for (size_t j = 0; j < input_count; j++) {
                for (size_t k = 0; k < output_count; k++) {
                    // Check if this input-output pair is valid
                    // (we may need more sophisticated matching logic here)
                    
                    // Execute the component
                    result = mps_component_execute(
                        ctx, component, input_streams[j], output_streams[k], pipeline->current_iteration);
                        
                    if (result == NEXUS_SUCCESS) {
                        // Mark that we made progress, may need another pass
                        continue_execution = true;
                    } else {
                        // Handle error
                        mps_handle_pipeline_error(
                            ctx, pipeline, result, component_id, pipeline->current_iteration);
                            
                        if (!pipeline->config->allow_partial_processing) {
                            free(input_streams);
                            free(source_ids);
                            free(output_streams);
                            free(target_ids);
                            return result;
                        }
                    }
                }
            }
            
            // Free resources
            free(input_streams);
            free(source_ids);
            free(output_streams);
            free(target_ids);
        }
        
        pass_count++;
    }
    
    // Notify all components that this iteration has ended
    for (size_t i = 0; i < group->component_count; i++) {
        const char* component_id = group->component_ids[i];
        NexusMPSPipelineComponent* component = mps_pipeline_get_component(pipeline, component_id);
        
        if (component) {
            mps_component_end_iteration(ctx, component, pipeline->current_iteration);
        }
    }
    
    return NEXUS_SUCCESS;
}
```

## Integration with Existing NexusLink Architecture

### Core Module Integration

Both SPS and MPS systems integrate with the core NexusLink architecture through:

1. **Component Loading**: Using `nexus_load_component` from `nexus_loader.c`
2. **Symbol Resolution**: Using `nexus_resolve_component_symbol` for function lookup
3. **Logging**: Using `nexus_log` for consistent error reporting
4. **Context Management**: Using the NexusContext for shared state

Example of component integration:

```c
// Load a component based on pipeline configuration
NexusComponent* load_pipeline_component(NexusContext* ctx, 
                                      NexusPipelineComponentConfig* config,
                                      const char* component_path) {
    if (!ctx || !config) {
        return NULL;
    }
    
    // Build component path if needed
    char* path = NULL;
    if (component_path) {
        // Use provided path
        path = strdup(component_path);
    } else if (ctx->component_path) {
        // Use context's default component path
        size_t len = strlen(ctx->component_path) + strlen(config->component_id) + 10;
        path = (char*)malloc(len);
        
        if (path) {
            snprintf(path, len, "%s/%s.so", ctx->component_path, config->component_id);
        }
    } else {
        // Use just the component ID as path
        path = strdup(config->component_id);
    }
    
    if (!path) {
        return NULL;
    }
    
    // Load the component
    NexusComponent* component = nexus_load_component(ctx, path, config->component_id);
    
    free(path);
    return component;
}
```

### Versioning Integration

Both systems should integrate with NexusLink's versioning system to respect version constraints:

```c
// Validate component version against constraint
bool validate_component_version(NexusContext* ctx,
                               NexusComponent* component,
                               const char* version_constraint) {
    if (!ctx || !component || !version_constraint) {
        return false;
    }
    
    // Resolve the version query function
    NexusComponentVersionFunc version_func = 
        (NexusComponentVersionFunc)nexus_resolve_component_symbol(
            ctx, component, "nexus_component_get_version");
            
    if (!version_func) {
        nexus_log(ctx, NEXUS_LOG_ERROR, 
                 "Component '%s' does not provide version information",
                 component->id);
        return false;
    }
    
    // Get the component version
    const char* version = version_func();
    if (!version) {
        nexus_log(ctx, NEXUS_LOG_ERROR, 
                 "Component '%s' returned NULL version",
                 component->id);
        return false;
    }
    
    // Verify version constraint
    bool satisfies = nexus_version_satisfies_constraint(version, version_constraint);
    
    if (!satisfies) {
        nexus_log(ctx, NEXUS_LOG_ERROR, 
                 "Component '%s' version '%s' does not satisfy constraint '%s'",
                 component->id, version, version_constraint);
    }
    
    return satisfies;
}
```

## State Preservation (MPS)

For MPS, state preservation between iterations is crucial:

```c
NexusResult mps_component_save_state(NexusContext* ctx,
                                    NexusMPSPipelineComponent* component,
                                    const char* state_path) {
    if (!ctx || !component || !state_path) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Get component lifecycle hooks
    NexusMPSComponentLifecycle* lifecycle = 
        (NexusMPSComponentLifecycle*)component->component_state;
        
    if (!lifecycle || !lifecycle->save_state_func) {
        return NEXUS_UNSUPPORTED;
    }
    
    // Call the save state function
    return lifecycle->save_state_func(component, state_path, lifecycle->user_data);
}

NexusResult mps_pipeline_create_checkpoint(NexusContext* ctx,
                                          NexusMPSPipeline* pipeline,
                                          const char* checkpoint_dir) {
    if (!ctx || !pipeline || !checkpoint_dir) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Create the checkpoint directory if it doesn't exist
    // (Platform-specific code would be needed here)
    
    // Save state for each component
    for (size_t i = 0; i < pipeline->component_count; i++) {
        if (pipeline->components[i]) {
            // Create component-specific path
            size_t path_len = strlen(checkpoint_dir) + 
                             strlen(pipeline->components[i]->component_id) + 10;
            char* state_path = (char*)malloc(path_len);
            
            if (!state_path) {
                return NEXUS_OUT_OF_MEMORY;
            }
            
            snprintf(state_path, path_len, "%s/%s.state", 
                    checkpoint_dir, pipeline->components[i]->component_id);
            
            // Save component state
            NexusResult result = mps_component_save_state(
                ctx, pipeline->components[i], state_path);
                
            free(state_path);
            
            // Continue even if one component fails, but log the error
            if (result != NEXUS_SUCCESS && result != NEXUS_UNSUPPORTED) {
                nexus_log(ctx, NEXUS_LOG_ERROR, 
                         "Failed to save state for component '%s': %s",
                         pipeline->components[i]->component_id,
                         nexus_result_to_string(result));
            }
        }
    }
    
    // Save pipeline metadata
    // (Implementation would write iteration count, etc. to a metadata file)
    
    return NEXUS_SUCCESS;
}
```

## Performance Considerations

For both systems, but especially MPS, performance optimizations should include:

1. **Stream Buffering**: Efficient memory management for stream data
2. **Component Caching**: Cache loaded components for reuse
3. **Stream Data Reuse**: Avoid copying stream data when possible
4. **Connection Optimization**: Group connections to minimize data transfers

Example of performance-optimized stream implementation:

```c
NexusResult mps_stream_write(NexusMPSDataStream* stream, const void* data, size_t size) {
    if (!stream || !data) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Fast path: if there's enough space at the current position, just copy
    if (stream->position + size <= stream->capacity) {
        memcpy((uint8_t*)stream->data + stream->position, data, size);
        stream->position += size;
        
        // Update the size if needed
        if (stream->position > stream->size) {
            stream->size = stream->position;
        }
        
        // Update stream state if needed
        if (stream->state == MPS_STREAM_EMPTY && stream->size > 0) {
            stream->state = MPS_STREAM_PARTIAL;
        }
        
        // Increment generation to indicate update
        stream->generation++;
        
        return NEXUS_SUCCESS;
    }
    
    // Slow path: resize the buffer
    size_t new_capacity = stream->capacity * 2;
    while (new_capacity < stream->position + size) {
        new_capacity *= 2;
    }
    
    // Check if we're using a buffer manager
    if (stream->buffer_manager) {
        // Use buffer manager to resize
        // (Implementation would depend on buffer manager interface)
    } else {
        // Standard realloc
        void* new_data = realloc(stream->data, new_capacity);
        if (!new_data) {
            return NEXUS_OUT_OF_MEMORY;
        }
        
        stream->data = new_data;
    }
    
    stream->capacity = new_capacity;
    
    // Now copy the data
    memcpy((uint8_t*)stream->data + stream->position, data, size);
    stream->position += size;
    
    // Update the size
    if (stream->position > stream->size) {
        stream->size = stream->position;
    }
    
    // Update stream state
    if (stream->state == MPS_STREAM_EMPTY && stream->size > 0) {
        stream->state = MPS_STREAM_PARTIAL;
    }
    
    // Increment generation
    stream->generation++;
    
    return NEXUS_SUCCESS;
}
```

## Module Organization

This implementation follows the modular design of NexusLink:

1. **Separate Modules**: Each functionality area (config, dependency, pipeline, stream, lifecycle) is in its own file
2. **Clear Interfaces**: Each module has a well-defined API with common patterns
3. **Minimal Coupling**: Modules interact through clean interfaces defined in header files
4. **Consistent Naming**: `sps_` and `mps_` prefixes for all functions to avoid conflicts

This organization supports incremental implementation, testing, and extension.

## Conclusion

Implementing the Single-Pass System (SPS) and Multi-Pass System (MPS) modules enhances NexusLink with sophisticated pipeline processing capabilities. By following these implementation guidelines, developers can create a robust, efficient, and extensible system that leverages the existing NexusLink architecture while adding new functionality.

Key focus areas during implementation should be:

1. **Robust error handling**: Graceful degradation and comprehensive error reporting
2. **Efficient memory management**: Careful ownership and lifecycle management
3. **Clear data flow**: Well-defined stream interfaces and connection management
4. **Extensibility**: Support for custom component configurations and pipeline topologies

With these considerations in mind, the NexusLink Pipeline Systems implementation will provide a powerful foundation for complex data processing applications.